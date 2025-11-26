#include "TerrainChunk.h"

// Constructor de la clase TerrainChunk
// Inicializa el fragmento de terreno.
// offsetX, offsetZ: Coordenadas mundiales de inicio del fragmento
// noiseGen: Referencia al generador de ruido para la altura
// textures: Vector de texturas a aplicar al terreno
// directory: Directorio base 
TerrainChunk::TerrainChunk(float offsetX, float offsetZ, NoiseGenerator& noiseGen, std::vector<Texture>& textures, std::string directory)
    // Inicializa la clase base Model y los offsets
    : Model(), m_WorldOffsetX(offsetX), m_WorldOffsetZ(offsetZ) {
    
    this->directory = directory;

    // Asegura que el vector attributes (de la clase Model) no este vacio.
    if (this->getModelAttributes()->size() == 0) {
        ModelAttributes attr{0}; 
        this->getModelAttributes()->push_back(attr);
    }

    // Llamada a la funcion de generacion de malla (mesh)
    generateMesh(noiseGen, textures);

    // Despues de que la malla es creada (en generateMesh), se asignan las texturas
    if (!meshes.empty() && !textures.empty()) {
        meshes[0]->textures = textures; 
    }
}

// Funcion para generar la geometria (malla/mesh) del fragmento de terreno
void TerrainChunk::generateMesh(NoiseGenerator& noiseGen, std::vector<Texture>& textures) // <-- RECIBE LAS TEXTURAS 
{
    std::vector<Vertex> vertices; // almacenara los vertices del terreno
    std::vector<unsigned int> indices; // almacenara los indices de los triangulos
    
    // **1. Generacion de Vertices**
    // Bucle CHUNK_SIZE x CHUNK_SIZE para crear una cuadricula de vertices
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            
            // Calcula las coordenadas mundiales globales para la funcion de ruido
            float globalX = (float)x * MESH_SCALE + m_WorldOffsetX;
            float globalZ = (float)z * MESH_SCALE + m_WorldOffsetZ;

            // **Obtiene la Altura del Generador de Ruido**
            // 0.01f es el parametro 'scale' (frecuencia) para el ruido.
            float height = noiseGen.getTerrainHeight(globalX, globalZ, 0.01f);
            
            // Aplica un multiplicador al valor de altura en el rango [0, 1]
            float worldHeight = height * 75.0f; // Altura maxima de 75 unidades

            Vertex vertex;

            // Posicion: Local al espacio del modelo del fragmento
            vertex.Position = glm::vec3(
                (float)x * MESH_SCALE,  // Coordenada X local
                worldHeight,            // Coordenada Y (Altura)
                (float)z * MESH_SCALE   // Coordenada Z local
            );

            // Coordenadas de Textura: Mapea la textura a traves del fragmento
            vertex.TexCoords = glm::vec2(
                (float)x / (CHUNK_SIZE - 1),
                (float)z / (CHUNK_SIZE - 1)
            );

            // Inicializa las normales del vertice a cero
            vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
            
            vertices.push_back(vertex);
        }
    }

    // **2. Generacion de Indices (para triangulos)**
    // Esto crea una cuadricula de triangulos conectando los vertices.
    // Itera sobre los quads (cuadrados) formados por 4 vertices adyacentes
    for (int z = 0; z < CHUNK_SIZE - 1; z++) {
        for (int x = 0; x < CHUNK_SIZE - 1; x++) {
            // Obtiene los indices de las 4 esquinas de un quad
            int topLeft = (z * CHUNK_SIZE) + x;
            int topRight = topLeft + 1;
            int bottomLeft = ((z + 1) * CHUNK_SIZE) + x;
            int bottomRight = bottomLeft + 1;

            // Primer triangulo del quad (topLeft, bottomLeft, topRight)
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Segundo triangulo del quad (topRight, bottomLeft, bottomRight)
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    // **3. Calculo de Normales**
    // Calcula las normales de los vertices para un sombreado suave
    calculateNormals(vertices, indices);

    // **4. Creacion de la Malla (Mesh)**
    // Crea un nuevo objeto Mesh con los datos generados y lo anade al vector 'meshes' del Model.
    std::vector<Material> materials; 
    meshes.emplace_back(new Mesh(vertices, indices, textures, materials,(int) GL_STATIC_DRAW,(int) GL_STATIC_DRAW));    

}

// Funcion para calcular las normales de los vertices
void TerrainChunk::calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    // Itera sobre cada triangulo en el mesh (saltando de 3 en 3 indices)
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        // Obtiene referencias a los 3 vertices del triangulo
        Vertex& v1 = vertices[indices[i]];
        Vertex& v2 = vertices[indices[i + 1]];
        Vertex& v3 = vertices[indices[i + 2]];

        // Calcula los 2 bordes del triangulo
        glm::vec3 edge1 = v2.Position - v1.Position;
        glm::vec3 edge2 = v3.Position - v1.Position;

        // Calcula la normal de la cara (producto cruz)
        glm::vec3 faceNormal = glm::cross(edge1, edge2);
        
        // Suma la normal de esta cara a las normales de los 3 vertices
        // Esto permite el calculo de la normal promedio (suavizado de las normales)
        v1.Normal += faceNormal;
        v2.Normal += faceNormal;
        v3.Normal += faceNormal;
    }

    // Normaliza todas las normales de los vertices
    // Divide por la magnitud para obtener la normal unitaria promedio (suavizada).
    for (auto& vertex : vertices)
    {
        vertex.Normal = glm::normalize(vertex.Normal);
    }
}


// Funcion para dibujar el fragmento de terreno con un shader especifico
void TerrainChunk::Draw(Shader& shader) 
{
    
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, *getTranslate()); 
    
    // Aplica rotaciones si las hay
    if (this->getRotX() != 0)
        model = glm::rotate(model, glm::radians(this->getRotX()), glm::vec3(1, 0, 0));
    if (this->getRotY() != 0)
        model = glm::rotate(model, glm::radians(this->getRotY()), glm::vec3(0, 1, 0));
    if (this->getRotZ() != 0)
        model = glm::rotate(model, glm::radians(this->getRotZ()), glm::vec3(0, 0, 1));
    
    // Envia la matriz model especifica de este fragmento al shader
    shader.setMat4("model", model);

    // 2. LLAMA A LA FUNCION DRAW DE LA CLASE BASE
    // Esto renderizara todas las mallas del fragmento.
    Model::Draw(shader, 0); 
}

// Sobrecarga de la funcion Draw sin shader
void TerrainChunk::Draw() 
{
    // Se deja vacia intencionalmente
    // Esto previene el renderizado sin el shader apropiado para el terreno.
    std::cerr << "ERROR: TerrainChunk::Draw() fue llamada sin shader!" << std::endl;
}