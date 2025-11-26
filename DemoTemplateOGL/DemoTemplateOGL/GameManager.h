// GameManager.h
#pragma once
#include <vector>
#include <string>
#include "Base/model.h"
#include "Terreno.h"
#include "Texto.h"

class GameManager {
public:
    GameManager(Model* player, Terreno* terrain, Camera* camera, std::string directory, std::vector<Model*>* recyclableObjects, Texto* coinsText = nullptr, Texto* distanceText = nullptr);
    ~GameManager() {};
    //void Update(float deltaTime);
    void OnPlayerHit(Model* obj);
    void ResetCounters(); // reinicia monedas/distancia (llamado desde Scenario)
    // Scenario consultará esto para saber cuándo reiniciar m_DistanceStartZ
    bool consumeResetDistanceFlag();

private:
    Model* m_Player;
    Terreno* m_Terrain;
    Camera* m_Camera;
    std::string m_Directory;
    std::vector<Model*>* m_RecyclableObjects;
    float m_SpawnTimer;
    int m_Score;
    bool m_IsGameOver;
    Texto* m_CoinsText; // puntero al texto en pantalla para el contador
    Texto* m_DistanceText; // puntero al texto que muestra la distancia recorrida
    bool m_ResetDistanceRequested = false;
};