#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time; 

out vec2 v_TexCoords;
out vec3 v_Normal;
out vec3 v_WorldPos;

void main()
{
    // Copia de la posición original
    vec3 pos = aPos;
    
    // --- ANIMACIÓN DE OLAS ---
    // se modifica la altura basado en x z y el tiempo.
    // se suman dos ondas para hacerlo mas natural.
    
    float wave1 = sin(pos.x * 0.5 + time) * 0.5;
    float wave2 = cos(pos.z * 0.3 + time * 0.8) * 0.3;
    
    pos.y += wave1 + wave2;


    v_WorldPos = vec3(model * vec4(pos, 1.0));
    
    // Recalcular normal aproximada 
    v_Normal = mat3(transpose(inverse(model))) * aNormal; 
    
    v_TexCoords = aTexCoords * 5.0; // Tiling (repetir textura 5 veces)
    
    gl_Position = projection * view * vec4(v_WorldPos, 1.0);
}