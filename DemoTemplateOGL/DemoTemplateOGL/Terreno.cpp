// Terreno.cpp

#include "Terreno.h"

extern string Directory; 

// Constructor for the Terreno (Terrain) manager
Terreno::Terreno(Camera* camera, NoiseGenerator* noiseGen, 
                 const char* shaderPathVS, const char* shaderPathFS, std::string directory, int renderDistance)
    // Initialize member variables
    : m_Camera(camera),
      m_NoiseGenerator(noiseGen),
      // Initialize the shader object with vertex (VS) and fragment (FS) shader paths
      m_TerrainShader(shaderPathVS, shaderPathFS),
      m_Directory(directory), // Store the directory for texture loading
      m_RenderDistance(renderDistance),
      m_CurrentCameraChunkKey(0, 0) // Initialize the camera's current chunk key
{
    // Ensure critical pointers are valid
    if (m_Camera == nullptr || m_NoiseGenerator == nullptr) {
        std::cerr << "ERROR: Terreno manager created with NULL camera or noise generator!" << std::endl;
    }
}

// Destructor
Terreno::~Terreno() {
    // **CRITICAL:** Clean up all dynamically allocated TerrainChunk objects
    // Iterate through all active chunks in the map
    for (auto const& [key, chunk] : m_ActiveChunks) {
        delete chunk; 
    }
    m_ActiveChunks.clear(); // Clear the map structure
}

// Function to load a texture and add it to the list of terrain textures
void Terreno::LoadTerrainTexture(const char* path, const char* type) {
    Texture texture;
    
    texture.id = TextureFromFile(path, m_Directory); 

#ifdef _WIN32
    strcpy_s(texture.type, 255, type);
    strcpy_s(texture.path, 1024, path);
#else
    strcpy(texture.type, type); 
    strcpy(texture.path, path); 
#endif

    m_TerrainTextures.push_back(texture);
}

// Calculates the grid coordinates (ChunkKey) for any given world position.
ChunkKey Terreno::getChunkKeyFromWorldPos(const glm::vec3& worldPos) {
    // Calculate the total size of a single chunk in world units.
    // (CHUNK_SIZE - 1) because N vertices define N-1 segments.
    const float chunkSize = (TerrainChunk::CHUNK_SIZE - 1) * TerrainChunk::MESH_SCALE;

    // This handles negative world coordinates (e.g., -0.5 / 64 = -0.007, floor = -1).
    int chunkX = (int)std::floor(worldPos.x / chunkSize);
    int chunkZ = (int)std::floor(worldPos.z / chunkSize);

    return std::make_pair(chunkX, chunkZ); // Return the pair (X, Z) as the key
}

// Loads a single chunk at the specified grid coordinates (if it is not already loaded).
void Terreno::loadChunk(int chunkX, int chunkZ) {
    ChunkKey key = std::make_pair(chunkX, chunkZ);

    // Check if this chunk is NOT already loaded in the map
    if (m_ActiveChunks.find(key) == m_ActiveChunks.end()) {
        
        // 1. Calculate the world-space offset (bottom-left corner) for this chunk
        const float chunkSize = (TerrainChunk::CHUNK_SIZE - 1) * TerrainChunk::MESH_SCALE;
        float offsetX = (float)chunkX * chunkSize;
        float offsetZ = (float)chunkZ * chunkSize;

        // 2. Create the new TerrainChunk objec
        TerrainChunk* newChunk = new TerrainChunk(offsetX, offsetZ, *m_NoiseGenerator, m_TerrainTextures, m_Directory);
        
        // 3. Set the chunk's model-matrix translation to its world position.
        // the mesh is built starting at (0, 0, 0) in local space.
        newChunk->setTranslate(new glm::vec3(offsetX, 0.0f, offsetZ));

        // 4. Add the newly created chunk to the map of active chunks
        m_ActiveChunks[key] = newChunk;
    }
}

// Removes chunks that are outside the defined render distance from the camera.
void Terreno::unloadOldChunks() {
    for (auto it = m_ActiveChunks.begin(); it != m_ActiveChunks.end(); /* no increment */) {
        
        ChunkKey key = it->first;
        
        // Calculate the distance (in chunks) from the camera's current chunk key
        int dist_x = std::abs(key.first - m_CurrentCameraChunkKey.first);
        int dist_z = std::abs(key.second - m_CurrentCameraChunkKey.second);

        // Check if the chunk is outside the defined render distance in any direction
        if (dist_x > m_RenderDistance || dist_z > m_RenderDistance) {
            delete it->second; 
            
            // Erase the entry from the map.
            it = m_ActiveChunks.erase(it);
        } else {
            // Chunk is still within range, move to the next element
            ++it;
        }
    }
}

// Main update loop for terrain management (LOD/Streaming)
void Terreno::Update() {
    // 1. Find out which chunk the camera is currently in
    m_CurrentCameraChunkKey = getChunkKeyFromWorldPos(m_Camera->getPosition());

    // 2. Load all chunks *around* the camera, up to the render distance
    // This generates an (2*m_RenderDistance + 1) x (2*m_RenderDistance + 1) grid.
    for (int z = -m_RenderDistance; z <= m_RenderDistance; z++) {
        for (int x = -m_RenderDistance; x <= m_RenderDistance; x++) {
            
            // Calculate the absolute chunk coordinates to load
            int chunkX = m_CurrentCameraChunkKey.first + x;
            int chunkZ = m_CurrentCameraChunkKey.second + z;
            
            // Load the chunk (it will only load if not already active)
            loadChunk(chunkX, chunkZ);
        }
    }

    // 3. Unload any chunks that are now too far away from the camera
    unloadOldChunks();
}


// Drawing function for the entire terrain
void Terreno::Draw() {
    // 1. Activate the single terrain shader for all chunks
    m_TerrainShader.use();

    // 2. Set universal uniforms (camera, lights, etc.) once per frame
    
    // --- Camera/View Setup ---
    m_TerrainShader.setMat4("projection", m_Camera->getProjection());
    m_TerrainShader.setMat4("view", m_Camera->getView());
    m_TerrainShader.setVec3("viewPos", m_Camera->getPosition());
    
    // --- Lighting Setup ---
    glm::vec3 lightColor;
    // Light color is set to white (1.0, 1.0, 1.0)
    lightColor.x = 1.0f;
    lightColor.y = 1.0f;
    lightColor.z = 1.0f;
    
    // Calculate diffuse and ambient color components
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.9f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.5f);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // Static light position

    // Send light properties to the shader
    m_TerrainShader.setVec3("light.ambient", ambientColor);
    m_TerrainShader.setVec3("light.diffuse", diffuseColor);
    m_TerrainShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    m_TerrainShader.setVec3("light.position", lightPos);
    // --- End of Lighting ---

    // 3. Draw all active chunks
    for (auto const& [key, chunk] : m_ActiveChunks) {
        chunk->Draw(m_TerrainShader);
    }
    
}

// Function to calculate the terrain height at any world coordinate
// This is used for camera collision or other physics calculations.
float Terreno::GetWorldHeight(float worldX, float worldZ)
{
    // These values MUST be identical to the constants used in 
    // TerrainChunk::generateMesh()
    const float noiseScale = 0.01f;
    const float noiseHeightMultiplier = 75.0f;
    
    // 1. Get the normalized height [0, 1] from the generator
    // The NoiseGenerator is the single source of truth for height.
    float normalizedHeight = m_NoiseGenerator->getTerrainHeight(worldX, worldZ, noiseScale);
    
    // 2. Convert the normalized height to the final world height
    return normalizedHeight * noiseHeightMultiplier;
}