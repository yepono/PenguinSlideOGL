#version 330 core
out vec4 FragColor; // El color final del píxel

// Entradas (vienen del Vertex Shader)
in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_WorldPos;

// --- Texturas ---
uniform sampler2D texture_diffuse1; 

// --- Iluminación ---
uniform vec3 viewPos; // Posición de la cámara

// Estructura de luz 
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;


void main()
{
    // 1. Normalizar vectores
    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(viewPos - v_WorldPos);
    vec3 lightDir = normalize(light.position - v_WorldPos);

    // 2. Obtener el color de la textura
    vec4 texColor = texture(texture_diffuse1, v_TexCoords);

    // --- Calcular Iluminación ---

    // Ambiente
    vec3 ambient = light.ambient * texColor.rgb;
    
    // Difusa
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor.rgb;
    
    // Especular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 42.0); // 32.0 = shininess
    vec3 specular = light.specular * spec ; 

    // 3. Color Final
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, texColor.a);
}