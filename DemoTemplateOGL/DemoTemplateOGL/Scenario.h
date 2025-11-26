#ifndef GRAPHICSRR_H
#define GRAPHICSRR_H

#ifdef _WIN32 
#include <windows.h>
#include <windowsx.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "Base/camera.h"
#include "Base/Billboard.h"
#include "Base/BillboardAnimation.h"
#include <iostream>
#include "Base/Scene.h"
#include "Texto.h"
#include "Billboard2D.h"
#include "CollitionBox.h"

class GameManager;

class Scenario : public Scene {
private:
    SkyDome* sky;
    Terreno* terreno;
    GameManager* m_GameManager;
    std::vector<Billboard*> billBoard;
    std::vector<BillboardAnimation*> billBoardAnim;
    std::vector<Billboard2D*> billBoard2D;
    std::vector<Model*> ourModel;
    std::vector<Model*> m_RecyclableObjects; // Solo obstáculos y monedas (para GameManager)
    Model* camara;
    //Water* water; desactivado por cambio en terreno.h
    float angulo;
    float animacion = 0;
    std::vector<Texto*> ourText;
	Texto* m_DistanceText = nullptr;
	float m_DistanceStartZ = 0.0f;
	// (m_CoinsText no se necesita aquí porque GameManager lo recibe)

	// Datos para controlar respawn de objetos reciclables
	struct RecyclableData {
		Model* model = nullptr;
		glm::vec3 respawnPos = glm::vec3(0.0f);
		bool collected = false;
		float respawnDistance = 120.0f; // ajustar según nivel
		double respawnTimer = 0.0;
	};
	std::vector<RecyclableData> m_RecyclableData;
public:
	Scenario(Camera *cam);
	Scenario(Model *camIni);
	void InitGraph(Model *main);

	virtual int update();

	void inicializaBillboards();

	//el metodo render toma el dispositivo sobre el cual va a dibujar
	//y hace su tarea ya conocida
	Scene* Render();

	std::vector<Model*> *getLoadedModels();
	std::vector<Model*> *getRecyclableObjects(){
    	return &m_RecyclableObjects;
	}
	std::vector<Billboard*> *getLoadedBillboards();
	std::vector<BillboardAnimation*> *getLoadedBillboardsAnimation();
	std::vector<Billboard2D*> *getLoadedBillboards2D();
	std::vector<Texto*> *getLoadedText();
	Model* getMainModel();
	void setMainModel(Model* mainModel);
	float getAngulo();
	void setAngulo(float angulo);
	SkyDome* getSky();
	Terreno* getTerreno();
    //void update() override;
	// Scenario.h

    bool removeCollideModel(Model* collider, int idxCollider) override;
    ~Scenario();
};

#endif