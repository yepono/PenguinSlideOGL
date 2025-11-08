// TerrainChunk.h
#ifndef TERRAIN_CHUNK_H
#define TERRAIN_CHUNK_H

#include "Base/model.h"     // Include your base model class
#include "NoiseGenerator.h" // Include our new noise generator

class TerrainChunk : public Model {
public:
    static const int CHUNK_SIZE = 64; // Vertices along one edge (e.g., 64x64 vertices)
    static constexpr float MESH_SCALE = 1.0f;

private:
    // Offset in world coordinates
    float m_WorldOffsetX;
    float m_WorldOffsetZ;

    /**
     * @brief Internal function to build the vertex and index buffers.
     * @param noiseGen The noise generator used to calculate heights.
     */
    void generateMesh(NoiseGenerator& noiseGen, std::vector<Texture>& textures); // <-- PARÁMETRO AÑADIDO
    /**
     * @brief Calculates smooth normals after all heights are set.
     * @param vertices The list of vertices to update.
     * @param indices The list of indices.
     */
    void calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

public:
    /**
     * @brief Constructs a new terrain chunk at a specific world offset.
     * @param offsetX The starting X coordinate of this chunk in the world.
     * @param offsetZ The starting Z coordinate of this chunk in the world.
     * @param noiseGen A reference to the noise generator.
     * @param textures A vector of textures to apply (e.g., grass, rock).
     */
    TerrainChunk(float offsetX, float offsetZ, NoiseGenerator& noiseGen, std::vector<Texture>& textures, std::string directory); // <-- PARÁMETRO AÑADIDO
    // The Draw(Shader& shader) function is inherited from Model

    virtual void Draw(Shader& shader);

	virtual void Draw() override;
};

#endif // TERRAIN_CHUNK_H