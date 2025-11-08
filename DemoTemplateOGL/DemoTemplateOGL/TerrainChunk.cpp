#include "TerrainChunk.h"

TerrainChunk::TerrainChunk(float offsetX, float offsetZ, NoiseGenerator& noiseGen, std::vector<Texture>& textures, std::string directory)
    : Model(), m_WorldOffsetX(offsetX), m_WorldOffsetZ(offsetZ) {
    
    // ¡INICIALIZA LA VARIABLE MIEMBRO HEREDADA!
    this->directory = directory;

// Esto es el código de seguridad de tu antiguo proyecto.
    // Asegura que el vector 'attributes' no esté vacío.
    if (this->getModelAttributes()->size() == 0) {
        ModelAttributes attr{0}; // Asumo que 0 es un ID o índice válido
        this->getModelAttributes()->push_back(attr);
    }
    // --

    // We are passing 'textures' to the base Model class,
    // but the actual mesh generation happens here.
    generateMesh(noiseGen, textures);

    // After the mesh is created (in generateMesh), we can assign the textures
    // This assumes your Mesh class constructor accepts textures
/*    if (!meshes.empty() && !textures.empty()) {
        meshes[0]->textures = textures; 
    }*/
}

void TerrainChunk::generateMesh(NoiseGenerator& noiseGen, std::vector<Texture>& textures) // <-- RECIBE LAS TEXTURAS 
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // **1. Generate Vertices**
    // Loop CHUNK_SIZE times to create a CHUNK_SIZE x CHUNK_SIZE grid of vertices
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            
            // Calculate global world coordinates for the noise function
            float globalX = (float)x * MESH_SCALE + m_WorldOffsetX;
            float globalZ = (float)z * MESH_SCALE + m_WorldOffsetZ;

            // **Get Height from Noise Generator**
            // The 0.01f is the 'scale' parameter we discussed.
            // You should make this a configurable variable!
            float height = noiseGen.getTerrainHeight(globalX, globalZ, 0.01f);
            
            // Apply a multiplier to the [0, 1] height
            float worldHeight = height * 75.0f; // Max height of 50 units

            Vertex vertex;

            // Position: Local to the chunk's model space
            vertex.Position = glm::vec3(
                (float)x * MESH_SCALE,
                worldHeight,
                (float)z * MESH_SCALE
            );

            // Texture Coordinates: Map the texture across the chunk
            vertex.TexCoords = glm::vec2(
                (float)x / (CHUNK_SIZE - 1),
                (float)z / (CHUNK_SIZE - 1)
            );

            // Normals: Start with a basic UP vector.
            // We'll fix this in calculateNormals()
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            
            vertices.push_back(vertex);
        }
    }

    // **2. Generate Indices (for triangles)**
    // This creates a grid of triangles connecting the vertices
    for (int z = 0; z < CHUNK_SIZE - 1; z++) {
        for (int x = 0; x < CHUNK_SIZE - 1; x++) {
            // Get indices of the 4 corners of a quad
            int topLeft = (z * CHUNK_SIZE) + x;
            int topRight = topLeft + 1;
            int bottomLeft = ((z + 1) * CHUNK_SIZE) + x;
            int bottomRight = bottomLeft + 1;

            // First triangle (top-left -> bottom-left -> top-right)
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle (top-right -> bottom-left -> bottom-right)
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    // **3. (Optional) Calculate Normals**
    // This is highly recommended for correct lighting!
    // calculateNormals(vertices, indices); // <-- Uncomment when you implement this

    // **4. Create the Mesh**
    // We create a new mesh with our data and add it to the Model's 'meshes' vector.
    // We use GL_DYNAMIC_DRAW if we ever want to modify the terrain (e.g., digging)
    std::vector<Material> materials; // Assuming your Mesh needs this
    meshes.emplace_back(new Mesh(vertices, indices, textures, materials,(int) GL_STATIC_DRAW,(int) GL_STATIC_DRAW));    
    // Note: The 'textures' parameter from the constructor should be passed
    // to the new Mesh() call, depending on how your Mesh class is structured.
    // My example `new Mesh(vertices, indices, {}, ...)` assumes an empty texture list
    // to start, but you should pass the 'textures' parameter here.
}


void TerrainChunk::calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    // 1. Reset all normals to zero
    for (auto& vertex : vertices) {
        vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // 2. Loop through all triangles (indices in sets of 3)
    for (size_t i = 0; i < indices.size(); i += 3) {
        // Get the vertices of the triangle
        Vertex& v1 = vertices[indices[i]];
        Vertex& v2 = vertices[indices[i + 1]];
        Vertex& v3 = vertices[indices[i + 2]];

        // Calculate the two edges of the triangle
        glm::vec3 edge1 = v2.Position - v1.Position;
        glm::vec3 edge2 = v3.Position - v1.Position;

        // Calculate the normal of the triangle's face
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        // Add this face's normal to all 3 vertices' normals
        v1.Normal += faceNormal;
        v2.Normal += faceNormal;
        v3.Normal += faceNormal;
    }

    // 3. Normalize all vertex normals
    for (auto& vertex : vertices) {
        vertex.Normal = glm::normalize(vertex.Normal);
    }
}

// In TerrainChunk.cpp

void TerrainChunk::Draw(Shader& shader) 
{
    // 1. SET THE 'MODEL' MATRIX (This chunk's local transform)
    // This logic is copied from your old Terreno::prepShader.
    // We assume TerrainChunk inherits getTranslate() and getRotX/Y/Z() from Model.
    
    glm::mat4 model = glm::mat4(1.0f);
    
    // getTranslate() was set in Terreno::loadChunk()
    model = glm::translate(model, *getTranslate()); 
    
    // Add any rotations if your chunks need them (usually they don't)
    if (this->getRotX() != 0)
        model = glm::rotate(model, glm::radians(this->getRotX()), glm::vec3(1, 0, 0));
    if (this->getRotY() != 0)
        model = glm::rotate(model, glm::radians(this->getRotY()), glm::vec3(0, 1, 0));
    if (this->getRotZ() != 0)
        model = glm::rotate(model, glm::radians(this->getRotZ()), glm::vec3(0, 0, 1));
    
    // Send this chunk's specific model matrix to the shader
    shader.setMat4("model", model);

    // 2. NOW CALL THE BASE CLASS DRAW FUNCTION
    // This will call Model::Draw(shader, 0), which calls meshes[i]->Draw(shader)
    Model::Draw(shader, 0); 
}

void TerrainChunk::Draw() 
{
    // Hacemos nada a propósito para evitar que se renderice con el shader equivocado.
    // O puedes poner un mensaje de error si lo prefieres:
    std::cerr << "ERROR: TerrainChunk::Draw() fue llamada sin shader!" << std::endl;
}