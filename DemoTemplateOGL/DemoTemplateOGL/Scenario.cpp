#include "Scenario.h"
#ifdef __linux__ 
#define ZeroMemory(x,y) memset(x,0,y)
#define wcscpy_s(x,y,z) wcscpy(x,z)
#define wcscat_s(x,y,z) wcscat(x,z)
#endif

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
	sky = new SkyDome(32, 32, 20, (WCHAR*)L"skydome/skydome1.jpg", main->cameraDetails);
	//creamos el terreno
	//terreno = new Terreno((WCHAR*)L"skydome/terreno.jpg", (WCHAR*)L"skydome/textnieve.jpg", 400, 400, main->cameraDetails);
	//water = new Water((WCHAR*)L"textures/terreno.bmp", (WCHAR*)L"textures/water.bmp", 20, 20, camara->cameraDetails);
	// 1. Create the shared noise generator
	NoiseGenerator* noiseGen = new NoiseGenerator(1337); // Use any seed

	// 2. Create the terrain manager
	terreno = new Terreno(main->cameraDetails, noiseGen, "shaders/terrain.vs", "shaders/terrain.fs",sky->directory);

	// 3. Load textures into the manager
	terreno->LoadTerrainTexture("skydome/nieve.jpg", "texture_diffuse1");
//	terreno->LoadTerrainTexture("skydome/texterr.jpg", "texture_diffuse2");
	
	
	
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
	translate = glm::vec3(0.0f, 10.0f, 25.0f);
	scale = glm::vec3(0.10f,0.10f, 0.10f);	
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	//rotation = glm::vec3(1.0f, 0.0f, 0.0f); //rotation X
	//model->setNextRotX(45); // 45� rotation
	ourModel.emplace_back(model);

	ModelAttributes m;

	// seal
	model = new Model("models/seal/sealion.obj", main->cameraDetails, false, true);
	translate = glm::vec3(25.0f, terreno->GetWorldHeight(25.0f, 30.0f) + 2, 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	model->name = "Seal";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(270);
	model->setNextRotX(270);
	ourModel.emplace_back(model);

	// tire
	model = new Model("models/tire/Tire.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(15.0f, terreno->GetWorldHeight(15.0f, 30.0f) + 6 , 60.0f);
	scale = glm::vec3(0.20f, 0.20f, 0.20f);	// it's a bit too big for our scene, so scale it down
	model->name = "Tire";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(270);
	model->setNextRotX(270);
	ourModel.emplace_back(model);

	// seagull
	model = new Model("models/seagull/seagull/seagull.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(36.0f, terreno->GetWorldHeight(36.0f, 30.0f) + 7 , 60.0f);
	scale = glm::vec3(0.002f, 0.002f, 0.002f);	// it's a bit too big for our scene, so scale it down
	model->name = "Seagull";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);
	try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/seagull/seagull/seagull.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}

	// Golden Fish
	model = new Model("models/goldenFish/Golden_Fish_FBX.FBX", main->cameraDetails, true, true);
	translate = glm::vec3(33.0f, terreno->GetWorldHeight(33.0f, 30.0f) , 60.0f);
	scale = glm::vec3(0.07f, 0.07f, 0.07);	// it's a bit too big for our scene, so scale it down
	model->name = "Golden Fish";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);

	// Squid
	model = new Model("models/squid/squid.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(12.0f, terreno->GetWorldHeight(12.0f, 30.0f) + 3, 60.0f);
	scale = glm::vec3(0.50f, 0.20f, 0.50f);	// it's a bit too big for our scene, so scale it down
	model->name = "Squid";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	//model->setRotX(-90);
	//model->setNextRotX(-90);
	ourModel.emplace_back(model);

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
	ourModel.emplace_back(model);

	// Blue Whale
	//model = new Model("models/blueWhale/uploads_files_5014572_Whale_Quad(1)", main->cameraDetails, true, true);
//	model = new Model("models/blueWhale/BlueWhale.fbx", main->cameraDetails, true, true);
/*	translate = glm::vec3(60.0f, terreno->Superficie(60.0f, 30.0f) + 2, 60.0f);
	scale = glm::vec3(0.20f, 0.20f, 0.20f);	// it's a bit too big for our scene, so scale it down
	model->name = "KillerWhale";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	ourModel.emplace_back(model);*/


	// Cave
/*	model = new Model("models/cave/source/Cave.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(82.0f, terreno->Superficie(82.0f, 30.0f) + 2, 60.0f);
	scale = glm::vec3(2.0f, 2.0f, 2.0f);	// it's a bit too big for our scene, so scale it down
	model->name = "Cave";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);*/

	// Ice cube
	model = new Model("models/ice_cube/Ice.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(86.0f, terreno->GetWorldHeight(86.0f, 30.0f) + 1, 60.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	model->name = "IceCube";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	ourModel.emplace_back(model);

	// Stop
/*	model = new Model("models/stop/model.dae", main->cameraDetails, true, true);
	translate = glm::vec3(44.0f, terreno->Superficie(44.0f, 30.0f), 60.0f);
	scale = glm::vec3(6.0f, 6.0f, 6.0f);	// it's a bit too big for our scene, so scale it down
	model->name = "Stop";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	ourModel.emplace_back(model);*/

	// monster
/*	model = new Model("models/monster/icewithwalking.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(50.0f, terreno->Superficie(50.0f, 30.0f) + 2, 60.0f);
	scale = glm::vec3(4.0f, 4.0f, 4.0f);	// it's a bit too big for our scene, so scale it down
	model->name = "Monster";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	ourModel.emplace_back(model);
	try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/monster/icewithwalking.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(0);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}*/

	// monster
	/*model = new Model("models/snowman/snowman.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(90.0f, terreno->Superficie(90.0f, 30.0f) + 2, 60.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	model->name = "Snowman";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	ourModel.emplace_back(model);*/


	// monster
	model = new Model("models/ship/source/alligatorclass.glb", main->cameraDetails, true, true);
	translate = glm::vec3(45.0f, terreno->GetWorldHeight(45.0f, 30.0f) + 2, 20.0f);
	scale = glm::vec3(30.0f, 30.0f, 30.0f);	// it's a bit too big for our scene, so scale it down
	model->name = "Barco";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(90);
	model->setNextRotX(90);
	ourModel.emplace_back(model);


	// monster
/*	model = new Model("models/rock/roca.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(-5.0f, terreno->Superficie(-5.0f, 30.0f) + 2, 60.0f);
	scale = glm::vec3(0.10f, 0.10f, 0.10f);	// it's a bit too big for our scene, so scale it down
	model->name = "Roca";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);*/


/*	model = new Model("models/carro/carrito.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(20.0f, terreno->Superficie(20.0f, 30.0f) + 2, 60.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	model->name = "Carro";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);*/

	// galleta
/*	model = new Model("models/obstaculo/obstacle_hill.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(10.0f, terreno->Superficie(20.0f, 30.0f) + 2, 50.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	model->name = "Cookie";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);*/

	// monster
/*	model = new Model("models/polarBear/source/PB_IDLE_WALK_POUND.fbx", main->cameraDetails, true, true);
	translate = glm::vec3(70.0f, terreno->Superficie(90.0f, 30.0f) + 2, 50.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	model->name = "Polar Bear";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	ourModel.emplace_back(model);*/


	// monster
	model = new Model("models/ala/Wing.stl", main->cameraDetails, true, true);
	translate = glm::vec3(60.0f, terreno->GetWorldHeight(90.0f, 30.0f) + 2, 30.0f);
	scale = glm::vec3(0.10f, 0.10f, 0.10f);	// it's a bit too big for our scene, so scale it down
	model->name = "Ala";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setRotX(-90);
	model->setNextRotX(-90);
	ourModel.emplace_back(model);


	model = new Model("models/penguin/Jogging.fbx", main->cameraDetails);
	translate = glm::vec3(0.0f, terreno->GetWorldHeight(0.0f, 60.0f), 60.0f);
	scale = glm::vec3(0.002f, 0.002f, 0.002f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotY(90);
	ourModel.emplace_back(model);
/*	try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/penguin/Jogging.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::vector<Animation> animation = Animation::loadAllAnimations("models/penguin/Jumping.fbx", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::move(animation.begin(), animation.end(), std::back_inserter(animations));
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(1);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}*/


/*	Model *pez = new Model("models/pez/pez.obj", main->cameraDetails);
	translate = glm::vec3(0.0f, terreno->Superficie(0.0f, 50.0f), 50.0f);
	pez->setNextTranslate(&translate);
	pez->setTranslate(&translate);
	ourModel.emplace_back(pez);
	model = CollitionBox::GenerateAABB(m.translate, pez->AABBsize, main->cameraDetails);
	m.hitbox = model;
	pez->getModelAttributes()->push_back(m);
	translate.x = 5;
	pez->setTranslate(&translate, pez->getModelAttributes()->size()-1);
	pez->setNextTranslate(&translate, pez->getModelAttributes()->size()-1);
	model = CollitionBox::GenerateAABB(m.translate, pez->AABBsize, main->cameraDetails);
	m.hitbox = model; // Le decimos al ultimo ModelAttribute que tiene un hitbox asignado
	pez->getModelAttributes()->push_back(m);
	translate.x = 10;
	pez->setTranslate(&translate, pez->getModelAttributes()->size()-1);
	pez->setNextTranslate(&translate, pez->getModelAttributes()->size()-1);*/
/*
	model = new Model("models/dancing_vampire/dancing_vampire.dae", main->cameraDetails);
	translate = glm::vec3(0.0f, terreno->Superficie(0.0f, 60.0f), 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotY(90);
	ourModel.emplace_back(model);
	try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/dancing_vampire/dancing_vampire.dae", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::vector<Animation> animation = Animation::loadAllAnimations("models/dancing_vampire/dancing_vampire.dae", model->GetBoneInfoMap(), model->getBonesInfo(), model->GetBoneCount());
		std::move(animation.begin(), animation.end(), std::back_inserter(animations));
		for (Animation animation : animations)
			model->setAnimator(Animator(animation));
		model->setAnimation(1);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}

	Model* silly = new Model("models/Silly_Dancing/Silly_Dancing.fbx", main->cameraDetails);
	translate = glm::vec3(10.0f, terreno->Superficie(10.0f, 60.0f) , 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	silly->setTranslate(&translate);
	silly->setNextTranslate(&translate);
	silly->setScale(&scale);
	silly->setNextRotY(180);
	ourModel.emplace_back(silly);
	try{
		std::vector<Animation> animations = Animation::loadAllAnimations("models/Silly_Dancing/Silly_Dancing.fbx", silly->GetBoneInfoMap(), silly->getBonesInfo(), silly->GetBoneCount());
		for (Animation animation : animations)
			silly->setAnimator(Animator(animation));
		silly->setAnimation(0);
	}catch(...){
		ERRORL("Could not load animation!", "ANIMACION");
	}
	model = CollitionBox::GenerateAABB(translate, silly->AABBsize, main->cameraDetails);
	m.hitbox = model; // Le decimos al ultimo ModelAttribute que tiene un hitbox asignado
	silly->getModelAttributes()->push_back(m);
	translate.x += 10;
	silly->setTranslate(&translate, silly->getModelAttributes()->size()-1);
	silly->setNextTranslate(&translate, silly->getModelAttributes()->size()-1);
	silly->setScale(&scale, silly->getModelAttributes()->size()-1);
	silly->setNextRotY(180, silly->getModelAttributes()->size()-1);
	silly->setRotY(180, silly->getModelAttributes()->size()-1);
	// Import model and clone with bones and animations
	model = new Model("models/Silly_Dancing/Silly_Dancing.fbx", main->cameraDetails);
	translate = glm::vec3(30.0f, terreno->Superficie(30.0f, 60.0f) , 60.0f);
	scale = glm::vec3(0.02f, 0.02f, 0.02f);	// it's a bit too big for our scene, so scale it down
	model->name = "Silly_Dancing1";
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	model->setNextRotY(180);
	ourModel.emplace_back(model);
	// Para clonar la animacion se eliminan los huesos del modelo actual y se copian los modelos y animators
	model->GetBoneInfoMap()->clear();
	model->getBonesInfo()->clear();
	*model->GetBoneInfoMap() = *silly->GetBoneInfoMap();
	*model->getBonesInfo() = *silly->getBonesInfo();
	model->setAnimator(silly->getAnimator());*/



	//	model = new Model("models/IronMan.obj", main->cameraDetails);
//	translate = glm::vec3(0.0f, 20.0f, 30.0f);
//	scale = glm::vec3(0.025f, 0.025f, 0.025f);	// it's a bit too big for our scene, so scale it down
//	model->setScale(&scale);
//	model->setTranslate(&translate);
//	ourModel.emplace_back(model);
/*	model = new Model("models/backpack/backpack.obj", main->cameraDetails, false, false);
	translate = glm::vec3(20.0f, terreno->Superficie(20.0f, 0.0f) + 2, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	model->setTranslate(&translate);
	model->setNextTranslate(&translate);
	model->setScale(&scale);
	ourModel.emplace_back(model);
	model->lightColor = glm::vec3(10,0,0);
	model = new CollitionBox(60.0f, 15.0f, 10.0f, 10, 10, 10, main->cameraDetails);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);	// it's a bit too big for our scene, so scale it down
	model->setNextTranslate(model->getTranslate());
	model->setScale(&scale);
	ourModel.emplace_back(model);*/
	

	inicializaBillboards();
//	std::wstring prueba(L"Esta es una prueba");
//	ourText.emplace_back(new Texto(prueba, 20, 0, 0, SCR_HEIGHT, 0, camara));
//	billBoard2D.emplace_back(new Billboard2D((WCHAR*)L"billboards/awesomeface.png", 6, 6, 100, 200, 0, camara->cameraDetails));
	scale = glm::vec3(100.0f, 100.0f, 0.0f);	// it's a bit too big for our scene, so scale it down
	//billBoard2D.back()->setScale(&scale);
	}

void Scenario::inicializaBillboards() {
	float ye = terreno->GetWorldHeight(0, 0);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol.png", 6, 6, 0, ye - 1, 0, camara->cameraDetails));

	ye = terreno->GetWorldHeight(-9, -15);
	billBoard.emplace_back(new Billboard((WCHAR*)L"billboards/Arbol3.png", 8, 8, -9, ye - 1, -15, camara->cameraDetails));

	BillboardAnimation *billBoardAnimated = new BillboardAnimation();
	ye = terreno->GetWorldHeight(5, -5);
	for (int frameArbol = 1; frameArbol < 4; frameArbol++){
		wchar_t textura[50] = {L"billboards/Arbol"};
		if (frameArbol != 1){
			wchar_t convert[25];
			swprintf(convert, 25, L"%d", frameArbol);
			wcscat_s(textura, 50, convert);
		}
		wcscat_s(textura, 50, L".png");
		billBoardAnimated->pushFrame(new Billboard((WCHAR*)textura, 6, 6, 5, ye - 1, -5, camara->cameraDetails));		
	}
	billBoardAnim.emplace_back(billBoardAnimated);
}

	//el metodo render toma el dispositivo sobre el cual va a dibujar
	//y hace su tarea ya conocida
Scene* Scenario::Render() {
	//borramos el biffer de color y el z para el control de profundidad a la 
	//hora del render a nivel pixel.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
//	glClearColor(255.0f, 255.0f, 255.0f, 255.0f);

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
	//water->Draw();
	// Dibujamos cada billboard que este cargado en el arreglo de billboards.
	for (int i = 0; i < billBoard.size(); i++)
		billBoard[i]->Draw();
	for (int i = 0; i < billBoardAnim.size(); i++)
		billBoardAnim[i]->Draw();
	for (int i = 0; i < billBoard2D.size(); i++)
		billBoard2D[i]->Draw();
	// Dibujamos cada modelo que este cargado en nuestro arreglo de modelos
	for (int i = 0; i < ourModel.size(); i++) {
			ourModel[i]->Draw();
	}
	for (int i = 0; i < ourText.size(); i++) {
		ourText[i]->Draw();
	}
		// Le decimos a winapi que haga el update en la ventana
	return this;
}
	
std::vector<Model*> *Scenario::getLoadedModels() {
	return &ourModel;
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
}
