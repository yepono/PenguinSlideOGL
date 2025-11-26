#include "Scenario.h"
#include "GameManager.h"
#include "Texto.h"
#include <algorithm>
#include "Water.h"
#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcscat_s(x,y,z) wcscat(x,z)
#endif

// Contador de colisiones local al módulo
static int s_collisionCount = 0;
// Flag externo para pedir la salida del programa
extern bool g_RequestExit;

// declarar las variables de tamaño de ventana definidas en DemoTemplateOGL.cpp
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
Water* water;

Scenario::Scenario(Camera *cam) {
    glm::vec3 translate;
	glm::vec3 scale;
    Model* model = new Model("models/Cube/Cube.obj", cam);
	translate = glm::vec3(0.0f, 0.0f, 3.0f);
	scale = glm::vec3(0.25f, 0.25f, 0.25f);	// it's a bit too big for our scene, so scale it down
	model->setScale(&scale);
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	InitGraph(model);
}
Scenario::Scenario(Model *camIni) {
    InitGraph(camIni);
}
void Scenario::InitGraph(Model *main) {
    float matAmbient[] = { 1,1,1,1 };
	float matDiff[] = { 1,1,1,1 };
	angulo = 0;
	camara = main;
	//creamos el objeto skydome
	sky = new SkyDome(32, 32, 20, (WCHAR*)L"skydome/night.jpg", main->cameraDetails);
	//creamos el terreno
	//terreno = new Terreno((WCHAR*)L"skydome/terreno.jpg", (WCHAR*)L"skyd
	//water = new Water((WCHAR*)L"textures/terreno.bmp", (WCHAR*)L"textures/water.bmp", 20, 20, camara->cameraDetails);
	// 1. Create the shared noise generator
	NoiseGenerator* noiseGen = new NoiseGenerator(1337, 6, 1.2f, 0.45f); // Use any seed

	// 2. Create the terrain manager
	terreno = new Terreno(main->cameraDetails, noiseGen, "shaders/terrain.vs", "shaders/terrain.fs",sky->directory);
	// 3. Load textures into the manager
	terreno->LoadTerrainTexture("skydome/snow.jpg", "texture_diffuse1");
	water = new Water("textures/water.jpg", 100.0f, 70.0f, main->cameraDetails, sky->directory);
	glm::vec3 posAgua(-35.0f, 35.0f, 30.0f); 
	water->setTranslate(&posAgua);
	
	// Anchura/derecha para anclar textos
    const int RIGHT_MARGIN = 40; // ajusta margen derecho
    int xPos = (int)SCR_WIDTH - RIGHT_MARGIN;

    // Crear Texto de distancia (arriba, mismo tamaño)
    // Texto::Texto(WCHAR *texto, float escala, float rotacion, float x, float y, float z, Model* camera)
    Texto *distanceText = new Texto((WCHAR*)L"Distance: 0", 28, 0, xPos, 40, 0, main);
    distanceText->name = "Distance";
    ourText.emplace_back(distanceText);
    this->m_DistanceText = distanceText;

    // Crear Texto para contador de monedas (más grande y a la derecha) y pasarlo al GameManager
    Texto *coinsText = new Texto((WCHAR*)L"Coins: 0", 28, 0, xPos, 10, 0, main);
    coinsText->name = "Coins";
    ourText.emplace_back(coinsText);

    // Guardar Z inicial para distancia
    if (main && main->getTranslate())
        m_DistanceStartZ = main->getTranslate()->z;
    else
        m_DistanceStartZ = 0.0f;

    // Pasamos los punteros al GameManager (coins y distance)
    m_GameManager = new GameManager(main, terreno, main->cameraDetails, sky->directory, getRecyclableObjects(), coinsText, distanceText);

    glm::vec3 translate;
	glm::vec3 scale;
	glm::vec3 rotation;
	//translate = glm::vec3(0.0f, 20.0f, 30.0f);
	//water->setTranslate(&translate);
	// load models
	// -----------
	ourModel.emplace_back(main);



	Model* model;
	model = new Model("models/fogata/campfire/Campfire.fbx", main->cameraDetails);
	translate = glm::vec3(30.0f, terreno->GetWorldHeight(30.0f, 30.0f) + 2, 25.0f);
	scale = glm::vec3(0.10f,0.10f, 0.10f);	
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setModelType("obstacle");
	//rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	//model->setNextRotX(45); // 45� rotation
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

	ModelAttributes m;

	// seal
	model = new Model("models/seal/sealion.obj", main->cameraDetails, false, true);
	translate = glm::vec3(35.0f, terreno->GetWorldHeight(35.0f, 50.0f) + 4, 50.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	model->name = "Seal";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(270);
	model->setNextRotX(270);
	model->setModelType("obstacle");
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

	// tire
	model = new Model("models/tire/Tire.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(33.0f, terreno->GetWorldHeight(33.0f, 30.0f) + 25 , 70.0f);
	scale = glm::vec3(0.20f, 0.20f, 0.20f);	// it's a bit too big for our scene, so scale it down
	model->name = "Tire";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(270);
	model->setNextRotX(270);
	model->setModelType("obstacle");
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

	// seagull
	model = new Model("models/seagull/seagull/seagull.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(33.0f, terreno->GetWorldHeight(33.0f, 90.0f) , 90.0f);
	scale = glm::vec3(0.006f, 0.006f, 0.006f);	// it's a bit too big for our scene, so scale it down
	model->name = "Seagull";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setModelType("obstacle");
	m.translate = translate;
	delete (Model*)model->getModelAttributes()->at(0).hitbox; // cast al tipo de objeto original para que el comp sepa cual destructor llamar
    Node n;
    n.m_center.x = 0.4;
    n.m_center.y = 1;
    n.m_center.z = -1;
    n.m_halfWidth = 2;
    n.m_halfHeight = 1;
    n.m_halfDepth = 2;
    model->getModelAttributes()->at(0).hitbox = CollitionBox::GenerateAABB(translate, n, main->cameraDetails);

	try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/seagull/seagull/seagull.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}

	// Wolf
	model = new Model("models/wolf-animated/source/WOLF_DEMO.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(33.0f, terreno->GetWorldHeight(33.0f, 30.0f) + 15 , 90.0f);
	scale = glm::vec3(0.0004f, 0.0004f, 0.0004f);	// it's a bit too big for our scene, so scale it down
	model->name = "Wolf";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setModelType("obstacle");
	model->setRotY(-45);
	model->setNextRotY(-45);
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

    // Reemplazar AABB por defecto del Wolf por una más grande / mejor centrada
    auto attrs_wolf = model->getModelAttributes();
    if (attrs_wolf && attrs_wolf->size() > 0) {
        // liberar hitbox original si existe
        if ((*attrs_wolf)[0].hitbox) {
            delete (Model*)(*attrs_wolf)[0].hitbox;
            (*attrs_wolf)[0].hitbox = nullptr;
        }
        // Definir nueva AABB (ajusta valores a tu escena)
        Node n;
        n.m_center.x = 0.0f;   // offset local X
        n.m_center.y = 2.0f;   // elevar el centro para cubrir torso/cabeza
        n.m_center.z = 0.0f;   // offset local Z
        n.m_halfWidth  = 2.0f; // total width = 4.0
        n.m_halfHeight = 4.0f; // total height = 12.0
        n.m_halfDepth  = 2.0f; // total depth = 4.0
        // Evitar pasar rvalues: crear posición local y usarla por referencia
        glm::vec3 hitPos = translate + glm::vec3(n.m_center.x, n.m_center.y, n.m_center.z);
        Model* newHit = CollitionBox::GenerateAABB(hitPos, n, main->cameraDetails);
        if (newHit) {
            newHit->name = "Wolf_hitbox";
            newHit->setTranslate(&hitPos);
            newHit->setNextTranslate(&hitPos);
            newHit->setActive(true, 0);
            (*attrs_wolf)[0].hitbox = newHit;
        }
    }

    try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/wolf-animated/source/WOLF_DEMO.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);



	// Golden Fish
	model = new Model("models/goldenFish/Golden_Fish_FBX.FBX", main->cameraDetails, true, true);
	translate = glm::vec3(33.0f, terreno->GetWorldHeight(33.0f, 30.0f) , 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02);	// it's a bit too big for our scene, so scale it down
	model->name = "Golden Fish";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setModelType("coin");
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

	// Cabana
	model = new Model("models/igloo/source/Cabana.glb", main->cameraDetails, false, true);
	translate = glm::vec3(-5.0f, terreno->GetWorldHeight(-5.0f, 5.0f) , 5.0f);
	scale = glm::vec3(8.0f, 8.0f, 8.0f);	// ajusta si hace falta
	model->name = "Cabana";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	model->setRotZ(90);
	model->setNextRotZ(90);
	model->setModelType("building");
	ourModel.emplace_back(model);

	delete model->getModelAttributes()->at(0).hitbox;
	model->getModelAttributes()->at(0).hitbox = NULL;

	// valores base
	float wallHalfHeight = 12.0f;   // altura media de pared
    float wallHalfDepth = 0.20f; // grosor pared
    float wallHalfWidth = 8.0f;    // mitad del ancho de la pared

	//Paredes
	model = new CollitionBox(-7,42,23, wallHalfWidth, wallHalfHeight, wallHalfDepth, main->cameraDetails);
	model->setNextTranslate(model->getTranslate());
	ourModel.emplace_back(model);

	/*model = new CollitionBox(4,42,13, wallHalfWidth, wallHalfHeight, wallHalfDepth, main->cameraDetails);
	model->setNextTranslate(model->getTranslate());
	ourModel.emplace_back(model);*/

	model = new CollitionBox(-8,42,-20, wallHalfWidth, wallHalfHeight, wallHalfDepth, main->cameraDetails);
	model->setNextTranslate(model->getTranslate());
	ourModel.emplace_back(model);

	// Squid
	model = new Model("models/squid/squid.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(12.0f, terreno->GetWorldHeight(12.0f, 30.0f) + 2, 55.0f);
	scale = glm::vec3(0.50f, 0.20f, 0.50f);	// it's a bit too big for our scene, so scale it down
	model->name = "Squid";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	//model->setRotX(-90);
	//model->setNextRotX(-90);
	model->setModelType("coin");
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

	// Christmas Tree
	model = new Model("models/ctree/ChristmasTree.obj", main->cameraDetails, true, true);
	translate = glm::vec3(56.0f, terreno->GetWorldHeight(56.0f, 30.0f) +1 , 60.0f);
	scale = glm::vec3(0.0020f, 0.0020f, 0.0020f);	// it's a bit too big for our scene, so scale it down
	model->name = "Tree";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	model->setModelType("obstacle");
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

	// Obstacle
	model = new Model("models/obstaculo/source/obstacles_hill.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(56.0f, terreno->GetWorldHeight(56.0f, 60.0f) +3 , 60.0f);
	scale = glm::vec3(1, 1, 1);	// it's a bit too big for our scene, so scale it down
	model->name = "obstacle";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	model->setModelType("obstacle");
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

	// Human
	model = new Model("models/55-rp_nathan_animated_003_walking_fbx/walkingHuman.fbx", main->cameraDetails, false, true);
	translate = glm::vec3(56.0f, terreno->GetWorldHeight(56.0f, 60.0f) +1 , 60.0f);
	scale = glm::vec3(0.020f, 0.020f, 0.020f);	// it's a bit too big for our scene, so scale it down
	model->name = "Human";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	//model->setRotX(-90);
	//model->setNextRotX(-90);
	model->setModelType("obstacle");

	    try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/55-rp_nathan_animated_003_walking_fbx/walkingHuman.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);
	
	// Polar Bear 
	model = new Model("models/polar-bear(1)/source/PolarBear.glb", main->cameraDetails, true, true);
	translate = glm::vec3(56.0f, terreno->GetWorldHeight(56.0f, 60.0f) +1 , 60.0f);
	scale = glm::vec3(1, 1, 1);	// it's a bit too big for our scene, so scale it down
	model->name = "Bear";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	//model->setRotX(-90);
	//model->setNextRotX(-90);
	model->setModelType("obstacle");

	    try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/polar-bear(1)/source/PolarBear.glb", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);



	// Ice cube
	model = new Model("models/ice_cube/Ice.fbx", main->cameraDetails);
	translate = glm::vec3(86.0f, terreno->GetWorldHeight(86.0f, 30.0f) + 1, 60.0f);
	scale = glm::vec3(0.5f, 0.5f, 0.5f);	// it's a bit too big for our scene, so scale it down
	model->name = "IceCube";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	model->setModelType("obstacle");
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

	// Stop
	model = new Model("models/stop/model.dae", main->cameraDetails, false);
	translate = glm::vec3(44.0f, terreno->GetWorldHeight(44.0f, 30.0f), 60.0f);
	scale = glm::vec3(0.4f, 0.4f, 0.4f);	// it's a bit too big for our scene, so scale it down
	model->name = "Stop";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	//model->setRotX(-90);
	//model->setNextRotX(-90);
	model->setModelType("obstacle");
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);

	// Cono
	model = new Model("models/cone/Cono.fbx", main->cameraDetails, false);
	translate = glm::vec3(44.0f, terreno->GetWorldHeight(44.0f, 30.0f), 60.0f);
	scale = glm::vec3(6, 6, 6);	// it's a bit too big for our scene, so scale it down
	model->name = "Cone";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	model->setModelType("obstacle");
	ourModel.emplace_back(model);
	m_RecyclableObjects.emplace_back(model);


    // Inicializar datos de respawn para cada objeto reciclable (antes de inicializaBillboards)
    m_RecyclableData.clear();
    // Asegurarnos que los objetos iniciales estén al menos 'minAhead' unidades por delante de la cámara
    float camZ = camara && camara->getTranslate() ? camara->getTranslate()->z : 0.0f;
    const float minAhead = 60.0f;
    const float laneSpacing = 5.0f; // debe coincidir con GameManager
    int idx = 0;
    for (Model* rm : m_RecyclableObjects) {
        RecyclableData d;
        d.model = rm;
        if (rm && rm->getTranslate()) d.respawnPos = *rm->getTranslate();
        else d.respawnPos = glm::vec3(0.0f);

        // Snap X a uno de los 3 carriles: -1,0,1 -> multiplica por laneSpacing
        int lane = (idx % 3) - 1; // 0->-1, 1->0, 2->1
        d.respawnPos.x = lane * laneSpacing;

        // Debug: imprimir posición inicial antes de ajustar
        if (d.model) {
            std::cout << "[Init] obj[" << idx << "] name='" << d.model->name << "' pos before adjust = ("
                      << d.respawnPos.x << ", " << d.respawnPos.y << ", " << d.respawnPos.z << ") camZ=" << camZ << "\n";
        }
        // Si el objeto está por detrás o muy cercano, lo colocamos al menos minAhead por delante
        if (d.respawnPos.z < camZ + minAhead) {
            d.respawnPos.z = camZ + minAhead + idx * 8.0f; // pequeño spacing entre objetos
            if (d.model) {
                d.model->setTranslate(&d.respawnPos);
                d.model->setNextTranslate(&d.respawnPos);
                d.model->setActive(true);
            }

        }
        d.collected = false;
        d.respawnDistance = 60.0f; // ajustar si quieres que reaparezcan antes o después
        d.respawnTimer = 0.0;
        m_RecyclableData.emplace_back(d);
        idx++;
    }

    inicializaBillboards();
//	std::wstring prueba(L"Esta es una prueba");
//	ourText.emplace_back(new Texto(prueba, 20, 0, 0, SCR_HEIGHT, 0, camara));
//	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/awesomeface.png", 6, 6, 100, 200, 0, camara->cameraDetails));
	scale = glm::vec3(100.0f, 100.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
	//billBoard2D.back()->setScale(&scale);
}



void Scenario::inicializaBillboards() {
	float ye = terreno->GetWorldHeight(0, 0);

	ye = terreno->GetWorldHeight(-9, -15);
}


Scene* Scenario::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

	if (this->animacion > 10) { // Timer se ejecuta cada 1000/30 = 33.333 ms
		for (BillboardAnimation *b : billBoardAnim){
			b->nextAnimation();
		}
		this->animacion = 0;
	} else {
		animacion = animacion + (1 * gameTime.deltaTime/100);
	}
	// Decimos que dibuje la media esfera
	sky->Draw();
	// Ahora el terreno
	terreno->Update(); // se encarga de cargar los chunks
	terreno->Draw();
	water->Draw();

	for (int i = 0; i < ourModel.size(); i++) {
        // Solo dibuja si está activo (evita duplicados y objetos "desactivados")
        if (ourModel[i]->getActive()) {
            ourModel[i]->Draw();
        }
    }

	// Dibujamos cada billboard que este cargado en el arreglo de billboards.
	for (int i = 0; i < billBoard.size(); i++)
		billBoard[i]->Draw();
	for (int i = 0; i < billBoardAnim.size(); i++)
		billBoardAnim[i]->Draw();
	for (int i = 0; i < billBoard2D.size(); i++)
		billBoard2D[i]->Draw();
	// Dibujamos cada texto
    for (int i = 0; i < ourText.size(); i++) {
        ourText[i]->Draw();
    }
		// Le decimos a winapi que haga el update en la ventana
	return this;
}

int Scenario::update() 
{

    if (m_GameManager && m_GameManager->consumeResetDistanceFlag()) {
        if (camara && camara->getTranslate()) {
            m_DistanceStartZ = camara->getTranslate()->z;
        } else if (getMainModel() && getMainModel()->getTranslate()) {
            m_DistanceStartZ = getMainModel()->getTranslate()->z;
        } else {
            m_DistanceStartZ = 0.0f;
        }
    }

    // Actualizar distancia en pantalla
    if (m_DistanceText && camara && camara->getTranslate()) {
        float playerZ = camara->getTranslate()->z;
        float dist = playerZ - m_DistanceStartZ;
        if (dist < 0.0f) dist = 0.0f;
        std::wstring s;
        s.resize(64);
        swprintf(&s[0], 64, L"Distance: %d", (int)dist);
        m_DistanceText->initTexto(s);
    }
    // -------------------------
    
    Scene::update();
    
    if (camara) {
        float playerZ = camara->getTranslate() ? camara->getTranslate()->z : 0.0f;

        // Si el jugador "ya pasó" un objeto (está detrás por cierta distancia), 
        // marcarlo como collected para que la lógica de respawn lo reprograme.
        const float RECYCLY_BEHIND = 10.0f;
        for (auto &d : m_RecyclableData) {
            if (!d.model) continue;
            // solo procesar si aún no fue marcado como collected y sigue activo en escena
            if (!d.collected && d.model->getTranslate()) {
                float objZ = d.model->getTranslate()->z;
                if (objZ < playerZ - RECYCLY_BEHIND) {
                    d.collected = true;
                    // guardamos la Z del jugador para medir avance
                    d.respawnPos.z = playerZ;
                    // desactivar modelo y sus hitboxes para evitar colisiones posteriores
                    d.model->setActive(false, 0);
                    auto attrs = d.model->getModelAttributes();
                    if (attrs) {
                        for (int ai = 0; ai < (int)attrs->size(); ++ai)
                            d.model->setActive(false, ai);
                    }
                    std::cout << "[AutoCollect] name='" << d.model->name << "' objZ=" << objZ << " playerZ=" << playerZ << "\n";
                }
            }
        }
        
        for (auto &d : m_RecyclableData) {
            if (!d.model) continue;
            if (d.collected) {
                float dist = playerZ - d.respawnPos.z;

                // Si el jugador avanzó lo suficiente desde que coleccionó -> respawnear delante del jugador
                if (dist > d.respawnDistance) {
                     // calcular nueva posición delante del jugador
                     int lane = (rand() % 3) - 1; // -1,0,1
                     float laneX = lane * 5.0f;
                     const float RESPAWN_AHEAD = 80.0f;
                     const float RANDOM_SPAN = 30.0f;
                     float newZ = playerZ + RESPAWN_AHEAD + (rand() % (int)RANDOM_SPAN);
                     float newY = (d.model->getModelType() == "coin")
                                     ? terreno->GetWorldHeight(laneX, newZ) + 2.0f
                                     : terreno->GetWorldHeight(laneX, newZ);
                     glm::vec3 newPos(laneX, newY, newZ);

                     d.collected = false;
                     d.respawnPos = newPos; // ahora respawnPos guarda la nueva posición real
                     // Reactivar modelo y sus hitboxes/atributos
                     d.model->setTranslate(&newPos);
                     d.model->setNextTranslate(&newPos);
                     d.model->setActive(true, 0);
                     auto attrs = d.model->getModelAttributes();
                     if (attrs) {
                         for (int ai = 0; ai < (int)attrs->size(); ++ai) {
                             d.model->setActive(true, ai);
                         }
                     }
                     std::cout << "[Respawn] Reactivated '" << d.model->name << "' at (" 
                               << newPos.x << ", " << newPos.y << ", " << newPos.z << ")\n";
                }
            }
        }
    }

    return -1; 
}
	
std::vector<Model*> *Scenario::getLoadedModels() {
	return &ourModel;
}
/*std::vector<Model*> *Scenario::getRecyclableObjects() {
    return &m_RecyclableObjects;
}*/
// Scenario.cpp

// Añade esta implementación al final o donde prefieras
bool Scenario::removeCollideModel(Model* collider, int idxCollider) {
    // 1. Avisar al GameManager que hubo un golpe (si existe)
    if (m_GameManager) {
        m_GameManager->OnPlayerHit(collider);
    }

    if (!collider) return false;

    // Si el objeto ya fue desactivado, no hacemos nada (evita logs repetidos)
    if (!collider->getActive()) return false;

    // Debug: imprimir cuando se detecta colisión/colección
    if (collider->getTranslate()) {
        glm::vec3 p = *collider->getTranslate();
        std::cout << "[Collect] name='" << collider->name << "' idxHit=" << idxCollider
                  << " pos=(" << p.x << ", " << p.y << ", " << p.z << ")\n";
    } else {
        std::cout << "[Collect] name='" << collider->name << "' idxHit=" << idxCollider << " pos=(null)\n";
    }

    // Marcar como inactivo para evitar colisiones continuas y logs repetidos
    collider->setActive(false, idxCollider);

    // Cada colisión equivale a pérdida inmediata: mostrar INFO y reiniciar contadores
    std::cout << "[Game] collision detected -> immediate loss\n";
    INFO("Has perdido. Contadores reiniciados. Puedes continuar.", "GAME_COLLISION");
    // Reiniciar contador local (si se usa en otro sitio)
    s_collisionCount = 0;
    // Reiniciar contadores del GameManager (monedas/distancia) si corresponde
    if (m_GameManager) {
        m_GameManager->ResetCounters();
    }

    // Buscar en la tabla de reciclables y marcar como collected
    for (auto &d : m_RecyclableData) {
        if (d.model == collider) {
            d.collected = true;
            // Guardamos la Z del jugador en vez de la Z del objeto, así medimos cuánto avanza el jugador
            if (camara && camara->getTranslate()) {
                d.respawnPos.z = camara->getTranslate()->z;
            } else if (collider->getTranslate()) {
                d.respawnPos.z = collider->getTranslate()->z;
            }
            d.respawnTimer = 0.0;
            // Debug: imprimir el respawnPos guardado (ahora contiene la Z del jugador al momento de coleccionar)
            std::cout << "[Collect] saved playerZ for '" << collider->name << "' = (z=" << d.respawnPos.z << ")\n";
            break;
        }
    }

    return true;
}

std::vector<Billboard*> *Scenario::getLoadedBillboards() {
	return &billBoard;
}
std::vector<Billboard2D*> *Scenario::getLoadedBillboards2D(){
	return &billBoard2D;
}
std::vector<Texto*> *Scenario::getLoadedText(){
	return &ourText;
}
std::vector<BillboardAnimation*> *Scenario::getLoadedBillboardsAnimation(){
	return &billBoardAnim;
}

Model* Scenario::getMainModel() {
	return this->camara;
}
void Scenario::setMainModel(Model* mainModel){
	this->camara = mainModel;
}
float Scenario::getAngulo() {
	return this->angulo;
}
void Scenario::setAngulo(float angulo) {
	this->angulo = angulo;
}
SkyDome* Scenario::getSky() {
	return sky;
}
Terreno* Scenario::getTerreno() {
	return terreno;
}

Scenario::~Scenario() {
	if (this->sky != NULL) {
		delete this->sky;
		this->sky = NULL;
	}
	if (this->terreno != NULL) {
		delete this->terreno;
		this->terreno = NULL;
	}
	if (billBoard.size() > 0)
		for (int i = 0; i < billBoard.size(); i++)
			delete billBoard[i];
	if (billBoardAnim.size() > 0)
		for (int i = 0; i < billBoardAnim.size(); i++)
			delete billBoardAnim[i];
	if (billBoard2D.size() > 0)
		for (int i = 0; i < billBoard2D.size(); i++)
			delete billBoard2D[i];
	this->billBoard.clear();
	if (ourText.size() > 0)
		for (int i = 0; i < ourText.size(); i++)
			if (!(ourText[i]->name.compare("FPSCounter") || ourText[i]->name.compare("Coordenadas")))
				delete ourText[i];
	this->ourText.clear();
	if (ourModel.size() > 0)
		for (int i = 0; i < ourModel.size(); i++)
			if (ourModel[i] != camara)
			delete ourModel[i];
	this->ourModel.clear();
	this->m_RecyclableObjects.clear();
}
