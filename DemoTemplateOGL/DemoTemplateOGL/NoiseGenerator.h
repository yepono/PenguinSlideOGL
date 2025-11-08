#ifndef NOISE_GENERATOR_H
#define NOISE_GENERATOR_H

#include "FastNoiseLite.h" 

class NoiseGenerator {
private:
    // generador de perlin noise
    FastNoiseLite m_Noise;
    
public:
    /* Perlin noise configuration
    s - seed
    octaves - noise maps to add details
    persistence - controls decrease in amplitude of octaves
    lacunarity - controls increase in freq of octave
     */
    NoiseGenerator(unsigned int s, int octaves = 8, float lacunarity = 2.0f, float persistence = 0.5f);

    /**
     * @brief Obtiene la altura del terreno en una coordenada mundial.
     * @param worldX Coordenada X mundial.
     * @param worldZ Coordenada Z mundial.
     * @param scale Frecuencia base (un valor pequeño como 0.01 da montañas grandes).
     * @return float Altura normalizada entre [0.0f, 1.0f].
     */
    float getTerrainHeight(float worldX, float worldZ, float scale);
};

#endif // NOISE_GENERATOR_H