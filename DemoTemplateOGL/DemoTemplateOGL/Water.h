/*#pragma once
#ifndef _wata
#define _wata
#include "Base/Utilities.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Base/glext.h"
#include "Base/wglext.h"
#include "Terreno.h"

class Water : public Terreno {

private:
double WATER_PI_FACTOR = 2.0f * M_PI;
size_t mapSizeX;
static constexpr float waveSpeed = 0.1f;
static constexpr float waveAmplitude = 0.5f;
static constexpr float waveFrequency = 0.5f;
void reloadData(vector<Vertex>& vertices) {
    static float time = 0.f;
    time += waveSpeed;
    
    const float waveTime = waveFrequency * time;

    Vertex* vData = vertices.data();  // Direct pointer access for fast memory operations
    size_t numVerts = vertices.size();

    for (size_t i = 0; i < numVerts; i += 4) {  // Loop unrolling: Process 4 vertices per iteration
        for (int j = 0; j < 4 && (i + j) < numVerts; j++) {
            Vertex &v = vData[i + j];  // Access vertex directly using pointer arithmetic

            float xPos = v.Position.x;
            float zPos = v.Position.z;

            // Fast sine lookup (LUT) instead of costly sin() calls
            int xIndex = ((int)((waveFrequency * xPos + waveTime) * LUT_SIZE / (WATER_PI_FACTOR))) % LUT_SIZE;
            int zIndex = ((int)((waveFrequency * zPos + waveTime) * LUT_SIZE / (WATER_PI_FACTOR))) % LUT_SIZE;
            
            v.Position.y = waveAmplitude * (UTILITIES_OGL::sinLUT[xIndex] + UTILITIES_OGL::sinLUT[zIndex]);
        }
    }
    // If needed, recalculate normals and rebuild KD tree
//    UTILITIES_OGL::calculateNormals(vertices, meshes[0]->indices);
//    buildKDtree();
}
public:
	Water(WCHAR alturas[], WCHAR textura[], float ancho, float prof, Camera* camera)
		: Terreno(alturas, textura, ancho, prof, camera, GL_DYNAMIC_DRAW) {
		gpuDemo = NULL;
//		buildKDtree();
		mapSizeX = getMapAlturaX() * 3;
		for (int i = 0; i < LUT_SIZE; i++) {
			UTILITIES_OGL::sinLUT[i] = sin((float)i / LUT_SIZE * 2.0f * M_PI);
		}
	}

	~Water() {
		//nos aseguramos de disponer de los recursos previamente reservados
	}

	virtual void Draw() {
		if (gpuDemo == NULL) {
			gpuDemo = new Shader("shaders/water.vs", "shaders/water.fs");
			setDefaultShader(true);
		}
		if (getDefaultShader()) {
			gpuDemo->use();
			prepShader(*gpuDemo);
			Draw(*gpuDemo);
			gpuDemo->desuse();
		}
		else Draw(*gpuDemo);
	}

	virtual void Draw(Shader& shader) {
//		reloadData(meshes[0]->vertices);
		static float time = 0.f;
		time += waveSpeed;
		shader.setFloat("time", time);
		Model::Draw(shader,0);
	}

};
#endif */


#ifndef _wata
#define _wata

#include "Base/model.h"
#include "Base/Utilities.h" 
#include <vector>
#include <cmath>

// Definición de constantes para la simulación de olas
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Water : public Model {
private:
    float anchof;
    float proff;
    int verx, verz; // Vértices en X y Z

    // Parámetros de animación
    float time = 0.0f;
    const float waveSpeed = 0.05f; // Velocidad de la animación
    
    // Shader específico para el agua
    Shader* waterShader = nullptr;

public:
    /**
     * @brief Constructor de Agua.
     * @param textName Nombre del archivo de textura (ej: "water.jpg").
     * @param ancho Ancho total del plano de agua.
     * @param prof Profundidad total del plano de agua.
     * @param camera Puntero a la cámara.
     * @param directory Ruta base de recursos (para cargar la textura).
     */
    Water(const char* textName, float ancho, float prof, Camera* camera, std::string directory) 
        : Model() 
    {
        this->cameraDetails = camera;
        this->directory = directory; // Guardamos el directorio heredado de Model
        anchof = ancho;
        proff = prof;
        
        // Resolución de la malla (puedes ajustarlo)
        // Un valor más alto da olas más suaves pero cuesta más rendimiento
        verx = 128; 
        verz = 128;

        // 1. Generar la Malla Plana (Usando Utilities)
        // Creamos un plano simple sin mapa de alturas (altura 0)
        UTILITIES_OGL::Maya planoAgua = UTILITIES_OGL::Plano(verx, verz, ancho, prof);
        
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        // Convertimos la estructura Maya a vectores estándar
        UTILITIES_OGL::vectoresEsfera(planoAgua, vertices, indices, verx * verz * 3, (verx - 1) * (verz - 1) * 6);

        // Limpiamos la memoria temporal de Maya
        delete[] planoAgua.maya;
        delete[] planoAgua.indices;

        // 2. Cargar Textura
        std::vector<Texture> textures;
        Texture t;
        t.id = TextureFromFile(textName, this->directory);
        
        // Importante: Asignar el tipo correcto para que el shader lo reconozca
        // Usaremos "texture_diffuse" para que coincida con tu Mesh.h estándar
        #ifdef _WIN32
            strcpy_s(t.type, 255, "texture_diffuse");
            strcpy_s(t.path, 1024, textName);
        #else
            strcpy(t.type, "texture_diffuse");
            strcpy(t.path, textName);
        #endif
        
        textures.push_back(t);

        // 3. Crear el Mesh
        // Usamos GL_STATIC_DRAW porque vamos a animar los vértices en el Vertex Shader (más eficiente),
        // no en la CPU.
        std::vector<Material> materials;
        meshes.emplace_back(new Mesh(vertices, indices, textures, materials, GL_STATIC_DRAW, GL_STATIC_DRAW));
        
        // Inicializar atributos del modelo base
        if (this->getModelAttributes()->size() == 0) {
            ModelAttributes attr{0};
            this->getModelAttributes()->push_back(attr);
        }
        
        // Cargar el shader específico de agua
        waterShader = new Shader("shaders/water.vs", "shaders/water.fs");
    }

    ~Water() {
        if (waterShader) {
            delete waterShader;
            waterShader = nullptr;
        }
    }

    // Override de Draw para usar el shader de agua y actualizar el tiempo
    virtual void Draw() override {
        if (waterShader) {
            waterShader->use();
            
            // Actualizar tiempo para la animación de olas
            time += waveSpeed;
            waterShader->setFloat("time", time);
            
            // Configurar matrices y luces (usando tu helper prepShader o manual)
            // Como Model::Draw() llama a prepShader con gpuDemo, aquí lo hacemos manual para waterShader
            
            // Matrices
            waterShader->setMat4("projection", cameraDetails->getProjection());
            waterShader->setMat4("view", cameraDetails->getView());
            
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, *getTranslate());
            // (Añadir rotaciones/escala si es necesario)
            waterShader->setMat4("model", modelMat);

            // Iluminación Básica (puedes mejorarla)
            waterShader->setVec3("viewPos", cameraDetails->getPosition());
            waterShader->setVec3("light.position", glm::vec3(100.0f, 100.0f, 100.0f));
            waterShader->setVec3("light.ambient", glm::vec3(0.5f));
            waterShader->setVec3("light.diffuse", glm::vec3(0.8f));
            waterShader->setVec3("light.specular", glm::vec3(1.0f));

            // Dibujar los meshes
            for (unsigned int i = 0; i < meshes.size(); i++) {
                meshes[i]->Draw(*waterShader);
            }
        }
    }
};

#endif