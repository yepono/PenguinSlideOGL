// GameManager.cpp
#include "GameManager.h"
#include "Scenario.h"
#include "Texto.h"
#include <cstdlib> 
#include <ctime>
#include <iostream>
#include <string>

// Constructor: Carga los 14 modelos prototipo (10 obst, 4 monedas)
GameManager::GameManager(Model* player, Terreno* terrain, Camera* camera, std::string directory, std::vector<Model*>* recyclableObjects, Texto* coinsText, Texto* distanceText)
    : m_Player(player),
      m_Terrain(terrain),
      m_Camera(camera),
      m_Directory(directory),
      m_RecyclableObjects(recyclableObjects),
      m_SpawnTimer(1.0f),
      m_Score(0),
      m_IsGameOver(false),
      m_CoinsText(coinsText),
      m_DistanceText(distanceText),
      m_ResetDistanceRequested(false)
{
    std::srand((unsigned)std::time(nullptr));
}



void GameManager::OnPlayerHit(Model* obj)
{
    if (!obj) return;

    std::string type = obj->getModelType();

    if (type == "coin")
    {
        m_Score++;

        // Desactivar attributes/hitboxes y mover fuera de pantalla
        auto attrs = obj->getModelAttributes();
        if (attrs) {
            for (int ai = 0; ai < (int)attrs->size(); ++ai) {
                obj->setActive(false, ai);
            }
        }
        obj->setActive(false, 0);
        glm::vec3 offscreen(0.0f, -10000.0f, 0.0f);
        obj->setTranslate(&offscreen);
        obj->setNextTranslate(&offscreen);


        if (m_CoinsText) {
            std::wstring s;
            s.resize(64);
            swprintf(&s[0], 64, L"Coins: %d", m_Score);
            m_CoinsText->initTexto(s);
        }
    }
    else
    {

        // Reiniciar contador de monedas
        m_Score = 0;
        if (m_CoinsText) {
            std::wstring zero = L"Coins: 0";
            m_CoinsText->initTexto(zero);
        }

        // Reiniciar texto de distancia en pantalla
        if (m_DistanceText) {
            std::wstring zero = L"Distance: 0";
            m_DistanceText->initTexto(zero);
        }

        // Señal para que Scenario recompute su m_DistanceStartZ en el siguiente update
        m_ResetDistanceRequested = true;

    }
}

bool GameManager::consumeResetDistanceFlag()
{
    if (!m_ResetDistanceRequested) return false;
    m_ResetDistanceRequested = false;
    return true;
}

void GameManager::ResetCounters() {
    // Reiniciar score/contadores internos
    m_Score = 0;
    m_ResetDistanceRequested = true;
    // Actualizar HUD 
    if (m_CoinsText) {
        std::wstring coins = L"Coins: 0";
        m_CoinsText->initTexto(coins);
    }
    if (m_DistanceText) {
        std::wstring dist = L"Distance: 0";
        m_DistanceText->initTexto(dist);
    }
}

