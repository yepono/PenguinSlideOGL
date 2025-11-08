#define FASTNOISE_IMPLEMENTATION
#include "FastNoiseLite.h"

#include "NoiseGenerator.h"

NoiseGenerator::NoiseGenerator(unsigned int s, int octaves, float lacunarity, float persistence) {
    
    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Noise.SetSeed(s);
    
    m_Noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_Noise.SetFractalOctaves(octaves);
    m_Noise.SetFractalLacunarity(lacunarity);
    
    m_Noise.SetFractalGain(persistence);
    
    // Frecuencia por defecto, se ajusta con el scale en getTerrainHeight
    m_Noise.SetFrequency(1.0f); 
}

// Ruido Fractal (Múltiples Octavas)
float NoiseGenerator::getTerrainHeight(float worldX, float worldZ, float scale) {
    

    float noiseValue = m_Noise.GetNoise(worldX * scale, worldZ * scale);
    
    // FastNoiseLite devuelve valores en el rango [-1, 1].
    return (noiseValue + 1.0f) * 0.5f;
}