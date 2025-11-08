// Terreno.cpp
#include "Terreno.h"

// We need the 'Directory' from your 'Model' base, I'll assume it's accessible
// If not, you'll need to adapt the texture loading.
extern string Directory; 

Terreno::Terreno(Camera* camera, NoiseGenerator* noiseGen, 
                 const char* shaderPathVS, const char* shaderPathFS, std::string directory, int renderDistance)
    : m_Camera(camera),
      m_NoiseGenerator(noiseGen),
      m_TerrainShader(shaderPathVS, shaderPathFS),
      m_Directory(directory), // directorio
      m_RenderDistance(renderDistance),
      m_CurrentCameraChunkKey(0, 0) // Initialize to (0,0)
{
    // Ensure pointers are valid
    if (m_Camera == nullptr || m_NoiseGenerator == nullptr) {
        // In a real project, you'd throw an exception or log an error
        std::cerr << "ERROR: Terreno manager created with NULL camera or noise generator!" << std::endl;
    }
}

Terreno::~Terreno() {
    // **CRITICAL:** Clean up all dynamically allocated chunks
    for (auto const& [key, chunk] : m_ActiveChunks) {
        delete chunk; // Delete the TerrainChunk object
    }
    m_ActiveChunks.clear(); // Clear the map
}

void Terreno::LoadTerrainTexture(const char* path, const char* type) {
    Texture texture;
     
    texture.id = TextureFromFile(path, m_Directory); // Esto está bien

    // --- CORRECCIÓN ---
    // No se puede asignar a un array. Debes usar una función de copia de strings
    // para copiar el contenido del puntero 'type' al array 'texture.type'.

#ifdef _WIN32
    // Versión segura de Windows (como en tu código anterior)
    strcpy_s(texture.type, 255, type);
    strcpy_s(texture.path, 1024, path);
#else
    // Versión estándar de C/C++
    strcpy(texture.type, type); // para asignar type 
    strcpy(texture.path, path);
#endif
    // --- FIN DE LA CORRECCIÓN ---

    m_TerrainTextures.push_back(texture);
}

ChunkKey Terreno::getChunkKeyFromWorldPos(const glm::vec3& worldPos) {
    // This calculation finds which grid square the position is in.
    // We use (CHUNK_SIZE - 1) because a 64-vertex chunk has 63 segments.
    const float chunkSize = (TerrainChunk::CHUNK_SIZE - 1) * TerrainChunk::MESH_SCALE;

    int chunkX = (int)std::floor(worldPos.x / chunkSize);
    int chunkZ = (int)std::floor(worldPos.z / chunkSize);

    return std::make_pair(chunkX, chunkZ);
}

void Terreno::loadChunk(int chunkX, int chunkZ) {
    ChunkKey key = std::make_pair(chunkX, chunkZ);

    // Check if this chunk is NOT already loaded
    if (m_ActiveChunks.find(key) == m_ActiveChunks.end()) {
        // It's not loaded, so let's create it.
        
        // 1. Calculate the world-space offset for this chunk
        const float chunkSize = (TerrainChunk::CHUNK_SIZE - 1) * TerrainChunk::MESH_SCALE;
        float offsetX = (float)chunkX * chunkSize;
        float offsetZ = (float)chunkZ * chunkSize;

        // 2. Create the new chunk
        // Pasa m_Directory al constructor del chunk
        TerrainChunk* newChunk = new TerrainChunk(offsetX, offsetZ, *m_NoiseGenerator, m_TerrainTextures, m_Directory);
        // 3. (Important!) Set its model-matrix transform to its world position
        // This assumes your Model class has a setTranslate or similar function.
        // This moves the chunk from its local (0,0,0) to its world (offsetX, 0, offsetZ) position.
        newChunk->setTranslate(new glm::vec3(offsetX, 0.0f, offsetZ));

        // 4. Add it to our map
        m_ActiveChunks[key] = newChunk;
    }
}

void Terreno::unloadOldChunks() {
    // This is the core optimization for RAM.
    // We loop through our map and check the distance of each chunk.
    
    // We must use an iterator to safely delete items from a map while looping
    for (auto it = m_ActiveChunks.begin(); it != m_ActiveChunks.end(); /* no increment */) {
        
        ChunkKey key = it->first;
        
        // Calculate distance (in chunks) from the camera's current chunk
        int dist_x = std::abs(key.first - m_CurrentCameraChunkKey.first);
        int dist_z = std::abs(key.second - m_CurrentCameraChunkKey.second);

        // If the chunk is outside our render distance (in ANY direction), unload it.
        if (dist_x > m_RenderDistance || dist_z > m_RenderDistance) {
            // Delete the chunk object from memory
            delete it->second; 
            
            // Erase the entry from the map.
            // .erase() returns the iterator to the *next* element, which is safe.
            it = m_ActiveChunks.erase(it);
        } else {
            // Chunk is still in range, move to the next one
            ++it;
        }
    }
}

void Terreno::Update() {
    // 1. Find out which chunk the camera is in
    m_CurrentCameraChunkKey = getChunkKeyFromWorldPos(m_Camera->getPosition());

    // 2. Load all chunks *around* the camera
    // This creates an (N*2+1) x (N*2+1) grid of chunks. (e.g., 11x11 if N=5)
    for (int z = -m_RenderDistance; z <= m_RenderDistance; z++) {
        for (int x = -m_RenderDistance; x <= m_RenderDistance; x++) {
            
            int chunkX = m_CurrentCameraChunkKey.first + x;
            int chunkZ = m_CurrentCameraChunkKey.second + z;
            
            loadChunk(chunkX, chunkZ);
        }
    }

    // 3. Unload any chunks that are now too far away
    unloadOldChunks();
}

// In Terreno.cpp


void Terreno::Draw() {
    // 1. Activate the single terrain shader
    m_TerrainShader.use();

    // 2. Set universal uniforms (camera, lights, etc.)
    // --- Camera/View ---
    m_TerrainShader.setMat4("projection", m_Camera->getProjection());
    m_TerrainShader.setMat4("view", m_Camera->getView());
    m_TerrainShader.setVec3("viewPos", m_Camera->getPosition());
     
    // --- Lighting (Copied from your old prepShader) ---
    // Note: You may want to move this calculation out of Draw() if it's slow
    glm::vec3 lightColor;
    lightColor.x = sin(7200 * 2.0f); // Or use a 'gameTime' variable
    lightColor.y = sin(7200 * 0.7f);
    lightColor.z = sin(7200 * 1.3f);
    
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // Static light position

    m_TerrainShader.setVec3("light.ambient", ambientColor);
    m_TerrainShader.setVec3("light.diffuse", diffuseColor);
    m_TerrainShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    m_TerrainShader.setVec3("light.position", lightPos);
    // --- End of Lighting ---

    // 3. Draw all active chunks
    for (auto const& [key, chunk] : m_ActiveChunks) {
        // This call will set the 'model' matrix and draw the mesh
        chunk->Draw(m_TerrainShader);
    }
    
    // 4. (Optional) De-activate the shader if needed
    // m_TerrainShader.desuse(); 
}

float Terreno::GetWorldHeight(float worldX, float worldZ)
{
    // --- !! CRITICAL !! ---
    // These values MUST be identical to the values you are using inside
    // TerrainChunk::generateMesh()
    
    // In our example, we hard-coded these:
    const float noiseScale = 0.01f;
    const float noiseHeightMultiplier = 75.0f;
    
    // 1. Get the normalized height [0, 1] from the generator
    //    We use the manager's m_NoiseGenerator member.
    float normalizedHeight = m_NoiseGenerator->getTerrainHeight(worldX, worldZ, noiseScale);
    
    // 2. Convert it to the final world height and return it
    return normalizedHeight * noiseHeightMultiplier;
}