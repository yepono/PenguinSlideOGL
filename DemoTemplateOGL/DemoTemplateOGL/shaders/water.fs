#version 330 core
out vec4 FragColor;

in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_WorldPos;

uniform sampler2D texture_diffuse1; // Textura del agua
uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

void main()
{
    // Muestrear textura
    vec4 texColor = texture(texture_diffuse1, v_TexCoords);
    
    // Hacerla azulada 
    texColor = texColor * vec4(0.6, 0.8, 1.0, 0.8); 

    // Iluminación Phong básica
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(light.position - v_WorldPos);
    vec3 viewDir = normalize(viewPos - v_WorldPos);
    
    // Ambiente
    vec3 ambient = light.ambient * texColor.rgb;
    
    // Difusa
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor.rgb;
    
    // Especular brillo fuerte
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = light.specular * spec * 0.8; 
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, texColor.a);
}