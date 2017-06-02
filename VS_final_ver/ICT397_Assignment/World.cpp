#include "World.h"

int World::screen_width;
int World::screen_height;
int World::game_status;
GraphicsFactory World::graphics_factory;
Graphics* World::graphic_handler;
Camera World::cam;
Control World::control;
GameTime World::game_time;
ParticleEmmitter World::particleSystem;
LuaData World::LD;
terrainData World::terrainInfo;
//terrain World::Terrain;

map<string,terrain> World::Terrains;
map<string, GameObject> World::Trees;
map<string, GameObject> World::Player;

//Current Postion
double World::CurrentX;
double World::CurrentZ;

//SkyBox
SkyBox World::Sky;
water World::WaterObj;

double World::elapsed_time_second;
int World::fps;

//Draw Terrain Model

bool World::RandomGrids[10][10];

bool World::RandomNPCs[10][10];


bool World::RandomTrees[10][10];

void World::LoadLuaFiles()
{
	LuaScript script("terrain.lua");
	terrainInfo.TerrainFile = script.get<std::string>("Terrain.TerrainFile");
	terrainInfo.ImageSize = script.get<int>("Terrain.ImageSize");
	terrainInfo.scalex = script.get<int>("Terrain.ScaleFactor.x");
	terrainInfo.scaley = script.get<int>("Terrain.ScaleFactor.y");
	terrainInfo.scalez = script.get<int>("Terrain.ScaleFactor.z");
	terrainInfo.heightfieldX = script.get<int>("Terrain.HeightField.x");
	terrainInfo.heightfieldY = script.get<float>("Terrain.HeightField.y");
	terrainInfo.heightfieldZ = script.get<int>("Terrain.HeightField.z");
	terrainInfo.ProceduralTexture1 = script.get<std::string>("Terrain.ProceduralTexture1");
	terrainInfo.ProceduralTexture2 = script.get<std::string>("Terrain.ProceduralTexture2");
	terrainInfo.ProceduralTexture3 = script.get<std::string>("Terrain.ProceduralTexture3");
	terrainInfo.ProceduralTexture4 = script.get<std::string>("Terrain.ProceduralTexture4");
	terrainInfo.repeat = script.get<int>("Terrain.repeatnum");
	terrainInfo.textureMapping = script.get<bool>("Terrain.textureMapping");
	terrainInfo.DetailMap = script.get<std::string>("Terrain.DetailMap");
	terrainInfo.mapRepeat = script.get<int>("Terrain.mapRepeat");
	terrainInfo.DetailMapping = script.get<bool>("Terrain.DetailMapping");
	terrainInfo.LightMap = script.get<std::string>("Terrain.LightMap");
	terrainInfo.LightMapSize = script.get<int>("Terrain.LightMapSize");
	terrainInfo.LightMapping = script.get<bool>("Terrain.LightMapping");

	//cout << "this is the terrain file name: " << terrainInfo.TerrainFile << endl;


	lua_State *gameFile = lua_open();

	luaL_openlibs(gameFile);
	luaL_loadfile(gameFile, "GameFiles.lua");
	lua_pcall(gameFile, 0, 0, 0);
	lua_getglobal(gameFile, "terrainFile");
	lua_getglobal(gameFile, "objectFile");
	lua_getglobal(gameFile, "npcFile");

	LD.TerrainFile = lua_tostring(gameFile, -3);
	LD.NPCsFile = lua_tostring(gameFile, -2);
	LD.ObjectsFile = lua_tostring(gameFile, -1);

	cout << LD.TerrainFile << endl;
	cout << LD.ObjectsFile << endl;
	cout << LD.NPCsFile << endl;
	lua_close(gameFile);

}

World::World(){
	game_status = GAME_PLAYING;
	screen_width = 800;
	screen_height = 600;

	if (! SetOpenGLGraphics())	
		std::cout << "error set openGL graphics. " << std::endl;
	
	control.SetCameraPtr(&cam);
	control.SetScreenSize(screen_width, screen_height);
	control.SetGameStatusFunc(SetGameStatus, GetGameStatus);
	graphic_handler->SetWorldInitializeFunc(Initialize);
	graphic_handler->SetWorldUpdateFunc(Update);
	texture2d.SetScreenSize(screen_width, screen_height);
}

void World::Initialize(){
	//Load extra lua files
	LoadLuaFiles();


	texture2d.SetTextureCount(3);
	texture2d.LoadRawTexture2D("pictures/GameMenu.raw", 512, 512, GAME_MAIN_MENU_TEXTURE);
	texture2d.LoadRawTexture2D("pictures/HelpMenu.raw", 512, 512, GAME_HELP_MENU_TEXTURE);
	texture2d.LoadRawTexture2D("pictures/Credit.raw", 512, 512, CREDIT_TEXTURE);

	// init the special effects
	InitSpecialEffects();

	//Initial Terrian Model
	InitialTerrain();

	InitCamera((terrainInfo.scalex * terrainInfo.ImageSize) / 2, (terrainInfo.scalez * terrainInfo.ImageSize) / 2);


	//Initial NPCs Model
	InitialTrees();


	Initplayer();
	//Initial NPCs Model
	//InitialNPCs();

	InitialSkyBox();
	InitialWater();

}

void World::Update(){



	elapsed_time_second = game_time.GetElapsedTimeSecond();
	fps = game_time.GetFps();
	cam.SetCameraSpdWithDT(elapsed_time_second);
	cam.CheckCamera();
	graphic_handler->SetCameraPos(cam.GetCameraPos());
	graphic_handler->SetCameraLookAt(cam.GetCameraLookAt());

	DrawSkyBox();
	DrawWater();
	//Draw Terrain Model
	DrawTerrain();
	DrawSpecialEffects();

	SetTerrainBoundray();

	//DrawNPCs();
	DrawPlayer();
	DrawTrees();


	//Draw NPCs Models




	if (GetGameStatus() != GAME_PLAYING){
		if (GetGameStatus() == GAME_DONE){
			GameDestruction();
			exit(0);
		}

		if (GetGameStatus() == GAME_MAIN_MENU){
			texture2d.Display2DTexture(512, 512, 0, 0, GAME_MAIN_MENU_TEXTURE);
		}

		if (GetGameStatus() == GAME_HELP_MENU){
			texture2d.Display2DTexture(512, 512, 0, 0, GAME_HELP_MENU_TEXTURE);
		}

		if (GetGameStatus() == GAME_CREDIT_PAGE){
			texture2d.Display2DTexture(512, 512, 0, 0, CREDIT_TEXTURE);
		}

	}

	CurrentX = cam.GetCameraPos().x;
	CurrentZ = cam.GetCameraPos().z;
}

void World::InitCamera(double x, double z )
{

	float y = Terrains["T1"].getAverageHight(x, z);

	cam.SetCameraPosX(x);
	cam.SetCameraPosY(y);
	cam.SetCameraPosZ(z);
}

void World::SetTerrainBoundray()
{
	int IndenSize = 50;
	if ((cam.GetCameraPos().x<IndenSize) ||
		(cam.GetCameraPos().x>terrainInfo.scalex * terrainInfo.ImageSize - IndenSize)||
		(cam.GetCameraPos().z<IndenSize) ||
		(cam.GetCameraPos().z>terrainInfo.scalex * terrainInfo.ImageSize - IndenSize) )
	{
		//cout << "Current22   " << cam.GetCameraPos().x << endl;
		cam.SetCameraPosX(CurrentX);
		cam.SetCameraPosZ(CurrentZ);
		//cout << "Collsion!!" << endl;
	}

	//cout << "Current22   " << cam.GetCameraPos().x << endl;
}

void World::InitSpecialEffects(){

	particleSystem.createParticles(50, 400, 50, fountain);
}

void World::DrawSpecialEffects(){
	particleSystem.updateParticles();
	particleSystem.DrawParticles(&particleSystem);
}

void World::RunGame(int* argc, char* argv[]){
	graphic_handler->SetKeyboardDownFunc(Control::OpenGLKeyboardDownFunc);
	graphic_handler->SetKeyboardUpFunc(Control::OpenGLKeyboardUpFunc);
	graphic_handler->SetPassiveMouseFunc(Control::OpenGLMouseMovementFunc);
	graphic_handler->CreateGameWindow(screen_width, screen_height, "Grid Grunt", argc, argv);
}

void World::GameDestruction(){
	delete graphic_handler;
	texture2d.Clear();
}

bool World::SetOpenGLGraphics(){
	graphic_handler = graphics_factory.Create("OpenGL");
	if (graphic_handler == 0){
		return false;
	}
	return true;
}

void World::InitialTerrain()
{
	terrain *Terrain=new terrain();

	Terrain->loadHeightfield(terrainInfo.TerrainFile.c_str(), terrainInfo.ImageSize);
	Terrain->setScalingFactor(terrainInfo.scalex, terrainInfo.scaley, terrainInfo.scalez);
	//Terrain->generateHeightfield(terrainInfo.heightfieldX, terrainInfo.heightfieldY, terrainInfo.heightfieldZ);
	Terrain->genFaultFormation(64, 128, 1, 128, 0.1, false);
	Terrain->addProceduralTexture(terrainInfo.ProceduralTexture1.c_str());
	Terrain->addProceduralTexture(terrainInfo.ProceduralTexture2.c_str());
	Terrain->addProceduralTexture(terrainInfo.ProceduralTexture3.c_str());
	Terrain->addProceduralTexture(terrainInfo.ProceduralTexture4.c_str());
	Terrain->createProceduralTexture();
	Terrain->setNumTerrainTexRepeat(terrainInfo.repeat);
	Terrain->DoTextureMapping(terrainInfo.textureMapping);
	Terrain->loadDetailMap(terrainInfo.DetailMap.c_str());
	Terrain->setNumDetailMapRepeat(terrainInfo.mapRepeat);
	Terrain->DoDetailMapping(terrainInfo.DetailMapping);
	Terrain->LoadLightMap(terrainInfo.LightMap.c_str(), terrainInfo.LightMapSize);
	Terrain->DoLightMapping(terrainInfo.LightMapping);
	
	/*
	Terrain->loadHeightfield(LD.TerrainFile.c_str(), 128);
	Terrain->setScalingFactor(7, 1, 7);
	Terrain->genFaultFormation(64, 128, 1, 128, 0.2, false);
	Terrain->addProceduralTexture("pictures/lowestTile.tga");
	Terrain->addProceduralTexture("pictures/lowTile.tga");
	Terrain->addProceduralTexture("pictures/highTile.tga");
	Terrain->addProceduralTexture("pictures/highestTile.tga");
	Terrain->createProceduralTexture();
	Terrain->setNumTerrainTexRepeat(1);
	Terrain->DoTextureMapping(true);
	Terrain->loadDetailMap("pictures/detailMap.tga");
	Terrain->setNumDetailMapRepeat(8);
	Terrain->DoDetailMapping(true);
	Terrain->LoadLightMap("pictures/lightmap.raw", 128);
	Terrain->DoLightMapping(true);
	*/
	Terrains["T1"] = *Terrain;

}

void World::DrawTerrain()
{
	Terrains["T1"].render();
	//terrain t1 = Terrains["T1"];
	float CamX = (float)cam.GetCameraPos().x;
	float CamZ = (float)cam.GetCameraPos().z;
	float CamY=Terrains["T1"].getAverageHight(CamX, CamZ);
	//cout << CamY << endl;
	cam.isTerrainCollision(CamY+20);
}

void World::InitialNPCs()
{

}

void World::DrawNPCs()
{


}

void World::Initplayer()
{
	static GameObject player;
	Vector3 playerPos;
	Vector3 playerSca;

	char* NPCFiles[] = { "3Dmodels/tris.md2", "3Dmodels/twilight.PCX" };
	player.LoadGameObject(NPCFiles, "MD2");

	playerPos.x = 50;
	playerPos.z = 50;
	playerPos.y = (double)Terrains["T1"].getAverageHight(playerPos.x, playerPos.z) + 5;

	playerSca.x = 0.3;
	playerSca.y = 0.3;
	playerSca.z = 0.3;

	player.setPosition(playerPos);
	player.setScale(playerSca);

	Player["NPC"] = player;
}

void World::DrawPlayer()
{
	static Vector3 NewPPos;
	static double NewPRot;
	NewPPos.x = cam.GetCameraPos().x + (cam.GetLookAtOffset().x * 25);
	NewPPos.z = cam.GetCameraPos().z + (cam.GetLookAtOffset().z * 25);
	NewPPos.y = (double)Terrains["T1"].getAverageHight(NewPPos.x, NewPPos.z) + 5;
	NewPRot = MathHelp::ToDegrees(cam.GetRotationBuffer().x) - 90.0;
	Player["NPC"].setPosition(NewPPos);
	Player["NPC"].setRotation(NewPRot);

	if (cam.GetCameraPos().x!=CurrentX)
	{
		Player["NPC"].ShowAnimation(40, 46);
	}

	else if (control.GetKeyPressed() == 'f')
	{
		Player["NPC"].ShowAnimation(47, 60);
	}

	else
	{
		Player["NPC"].ShowGameObject();
	}

	// state machine
	Player["NPC"].UpdateState();
}

void World::InitialTrees()
{
	static GameObject Tree;
	Vector3 TreePos;
	Vector3 TreeSca;

	for (int i = 0; i <20; i++)
	{
		int randNum1 = (rand() % 50) + 0;
		int randNum2 = (rand() % 50) + 0;
		char* TreeFiles[] = { const_cast<char *>(LD.ObjectsFile.c_str()) };
		Tree.LoadGameObject(TreeFiles, "OBJ");
		
		TreePos.x = 500 + (i*randNum1);
		TreePos.z = 500 + (i*randNum2);
		TreePos.y = (double)Terrains["T1"].getAverageHight(TreePos.x, TreePos.z)-5;
		Tree.setPosition(TreePos);

		Tree.SetBoundingBox(15);

		string TreeName = "Tree" + to_string(i);
		//cout << TreePos.x << " " << TreePos.y << " " << TreePos.z << " " << TreeName << endl;

		Trees[TreeName] = Tree;
	}



	for (int i = 0; i < 10; i++)
	{
		int randNum1 = (rand() % 10) + 0;
		int randNum2 = (rand() % 10) + 0;

		for (int j = 0; j < 10; j++)
		{
			std::vector<bool> colVector;

			if ((j == randNum1) || (j == randNum2))
			{
				RandomTrees[i][j] = false;
			}
			else
			{
				RandomTrees[i][j] = true;
			}
		}
	}
}


void World::DrawTrees()
{
	//cout << "Current11   " << cam.GetCameraPos().x<<endl;
	for (int i = 0; i < 20; i++)
	{
		string TreeName = "Tree" + to_string(i);
		Trees[TreeName].ShowGameObject();
		if (Trees[TreeName].processCollision(Player["NPC"]))
		{
			cam.SetCameraPosX(CurrentX);
			cam.SetCameraPosZ(CurrentZ);
			cout << "Collsion!!" << endl;
			cout << "Current11   " << CurrentX << " " << cam.GetCameraPos().x<< endl;
		}
		
	}

}


void World::InitialSkyBox()
{
	Vector3 SkyPos;
	Vector3 SkySca;

	Sky.loadSkyBox();

	SkyPos.x = cam.GetCameraPos().x;
	SkyPos.z = cam.GetCameraPos().z;
	SkyPos.y = (double)Terrains["T1"].getAverageHight(SkyPos.x, SkyPos.z) ;

	SkySca.x = 230;
	SkySca.y = 230;
	SkySca.z = 230;

	Sky.setPosition(SkyPos);
	Sky.setScale(SkySca);
	
}

void World::DrawSkyBox()
{
	static Vector3 NewSkyPos;
	Sky.ShowSkyBox();
	NewSkyPos.x = cam.GetCameraPos().x;
	NewSkyPos.z = cam.GetCameraPos().z;
	NewSkyPos.y = (double)Terrains["T1"].getAverageHight(NewSkyPos.x, NewSkyPos.z) +20;
	Sky.setPosition(NewSkyPos);



}

void World::InitialWater()
{
	Vector3 waters;
	Vector3 waterss;

	WaterObj.LoadWater();

	waters.x = cam.GetCameraPos().x;
	waters.z = cam.GetCameraPos().z;
	waters.y = (double)Terrains["T1"].getAverageHight(waters.x, waters.z);

	waterss.x = 230;
	waterss.y = 230;
	waterss.z = 230;

	WaterObj.setPosition(waters);
	WaterObj.setScale(waterss);
}

void World::DrawWater()
{
	static Vector3 NewWaterPos;
	WaterObj.ShowWater();
	NewWaterPos.x = 0;
	NewWaterPos.z = 0;
	NewWaterPos.y = 100;
	WaterObj.setPosition(NewWaterPos);
}