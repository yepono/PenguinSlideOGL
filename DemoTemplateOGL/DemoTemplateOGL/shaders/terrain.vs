#version 330 core
layout (location = 0) in vec3 aPos;      // Posición del vertice (desde VBO)
layout (location = 1) in vec2 aTexCoords; // Coordenadas de textura
layout (location = 2) in vec3 aNormal;   // Normal del vertice

// Matrices (uniforms globales)
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Salidas se envian al Fragment Shader
out vec2 v_TexCoords;
out vec3 v_Normal;
out vec3 v_WorldPos; // Posicion en el mundo 

void main()
{
    // Calcula la posición en el mundo
    v_WorldPos = vec3(model * vec4(aPos, 1.0));
    
    // Calcula la normal en el mundo (importante para la iluminación)
    v_Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Pasa las coordenadas de textura
    v_TexCoords = aTexCoords;
    
    // Calcula la posición final en la pantalla
    gl_Position = projection * view * vec4(v_WorldPos, 1.0);
}