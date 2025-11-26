#define FASTNOISE_IMPLEMENTATION
#include "FastNoiseLite.h"

#include "NoiseGenerator.h"

// Constructor de la clase NoiseGenerator
// Inicializa el generador de ruido con los parametros dados.
// s: semilla (seed) para la generacion de ruido
// octaves: numero de octavas para el ruido fractal
// lacunarity: lacunaridad para el ruido fractal
// persistence: persistencia (gain) para el ruido fractal
NoiseGenerator::NoiseGenerator(unsigned int s, int octaves, float lacunarity, float persistence) {
    
    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Noise.SetSeed(s);
    
    m_Noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_Noise.SetFractalOctaves(octaves);
    m_Noise.SetFractalLacunarity(lacunarity);
    
    m_Noise.SetFractalGain(persistence);
    
    // Frecuencia por defecto, se ajusta con el scale en getTerrainHeight
    // Esto establece la frecuencia inicial del ruido.
    m_Noise.SetFrequency(1.0f); 
}

// Ruido Fractal (Multiples Octavas)
// Calcula la altura del terreno (un valor de ruido) en las coordenadas dadas.
// worldX: coordenada X en el mundo
// worldZ: coordenada Z en el mundo
// scale: factor de escala para la frecuencia del ruido
float NoiseGenerator::getTerrainHeight(float worldX, float worldZ, float scale) {
    
    // Obtiene el valor de ruido para las coordenadas escaladas
    // Al multiplicar por 'scale', se controla el nivel de detalle (zoom) del ruido
    float noiseValue = m_Noise.GetNoise(worldX * scale, worldZ * scale);
    
    // FastNoiseLite devuelve valores en el rango [-1, 1].
    // Se mapea el valor de ruido del rango [-1, 1] al rango [0, 1]
    // 1. Se suma 1.0f para obtener el rango [0, 2]
    
    return (noiseValue + 1.0f) * 0.5f;
}