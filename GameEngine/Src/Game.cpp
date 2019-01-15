#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "AssetManager.h"
#include <sstream>
#include <vector>
#include <unordered_map>

Map* map;
Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
int screenX = 800;
int screenY = 640;
int scale = 3;
SDL_Rect Game::camera = {0, 0, screenX, screenY}; //width and height of camera

AssetManager* Game::assets = new AssetManager(&manager);

bool Game::isRunning = false;
std::unordered_map<std::string, Entity*> entities;

Game::Game()
{}

Game::~Game()
{}

bool Game::inStartMenu() 
{
	return inStart;
}

void addInteractiveObject(Entity& e, int xpos, int ypos, int height, int width, int scale, std::string id, std::string dialogue) 
{
	e.addComponent<TransformComponent>(xpos, ypos, height, width, scale);
	e.addComponent<ColliderComponent>(id);
	e.addComponent<InteractiveComponent>(dialogue);
	e.addComponent<SpriteComponent>(id);
	e.addComponent<KeyboardController>();
	e.addGroup(Game::groupTerrainColliders);
	e.addGroup(Game::groupInteractiveObjects);
}

void clearMap();
void accessMapPortal();

void addTextures()
{
	// Add textures/fonts
	Game::assets->AddTexture("player", "assets/character_spritesheet.png");
	Game::assets->AddTexture("dialogue", "assets/DialogueBackground.png");
	Game::assets->AddFont("Determination", "assets/Determination.ttf", 32);
	Game::assets->AddTexture("startScreen", "assets/StartScreen.png");
	Game::assets->AddTexture("fireplace", "assets/fireplace_spritesheet.png");
	Game::assets->AddTexture("fireplace1", "assets/fireplace_spritesheet.png");
	Game::assets->AddTexture("bed", "assets/bed.png");
	Game::assets->AddTexture("cabinet", "assets/cabinet.png");

}

void setupPlayer()
{
	entities["player"] = &manager.addEntity();
	entities["player"]->addComponent<TransformComponent>(1250, 250, 31, 19, scale);
	entities["player"]->addComponent<SpriteComponent>("player", true);
	entities["player"]->addComponent<ColliderComponent>("player");
	entities["player"]->addComponent<KeyboardController>();
	entities["player"]->addGroup(Game::groupPlayers);
}

void setupStartScreen()
{
	// Set up start screen
	entities["startScreen"] = &manager.addEntity();
	entities["startScreen"]->addComponent<TransformComponent>(0, 0, 640, 800, 1);
	entities["startScreen"]->addComponent<SpriteComponent>("startScreen", false, true);
	SDL_Color yellow = { 255, 255, 0 };
	entities["startScreen"]->addComponent<UILabel>(255, 280, "Press Z to start", "Determination", yellow, true);
	entities["startScreen"]->addComponent<KeyboardController>();
	entities["startScreen"]->addGroup(Game::groupScreenOverlays);
}
void setupMapOne()
{
	entities["player"]->getComponent<TransformComponent>().position.x = 1250;
	entities["player"]->getComponent<TransformComponent>().position.y = 440;
	Game::assets->AddTexture("terrain", "assets/room_1_tileset.png");
	clearMap();
	map = new Map("terrain", scale, 16);
	map->LoadMap("assets/room_1.map", 50, 40);

	if (entities.find("fire") == entities.end()) {
		entities["player"]->getComponent<TransformComponent>().position.x = 1250;
		entities["player"]->getComponent<TransformComponent>().position.y = 250;
		Game::assets->AddTexture("dialogue", "assets/DialogueBackground.png");
		entities["dialogueEntity"] = &manager.addEntity();
		Game::assets->CreateDialogue(Vector2D(screenX / 2 - 290, 450), "dialogue", *entities["dialogueEntity"], "You have awoken from a deep slumber.");
	}

	Game::assets->AddTexture("fireplace", "assets/fireplace_spritesheet.png");
	Game::assets->AddTexture("bed", "assets/bed.png");
	Game::assets->AddTexture("cabinet", "assets/cabinet.png");

	entities["fire"] = &manager.addEntity();
	entities["bed"] = &manager.addEntity();
	entities["cabinet"] = &manager.addEntity();


	entities["fire"]->addComponent<TransformComponent>(1250, 0, 64, 32, scale);
	entities["fire"]->addComponent<ColliderComponent>("fireplace");
	entities["fire"]->addComponent<InteractiveComponent>("The fire is almost out. It needs some wood.");
	std::vector<int> animationIndexFrame{ 7 };  //animations, frames
	entities["fire"]->addComponent<SpriteComponent>("fireplace", true, animationIndexFrame);
	entities["fire"]->addComponent<KeyboardController>();
	entities["fire"]->addGroup(Game::groupTerrainColliders);
	entities["fire"]->addGroup(Game::groupInteractiveObjects);
	
	addInteractiveObject(*entities["bed"], 1500, 100, 50, 32, scale, "bed", "You are well rested already.");
	addInteractiveObject(*entities["cabinet"], 1020, 10, 52, 31, scale, "cabinet", "It is locked. Don't you remember locking it?");
}

void setupMapTwo() 
{
	entities["player"]->getComponent<TransformComponent>().position.x = 425;
	entities["player"]->getComponent<TransformComponent>().position.y = 300;
	Game::assets->AddTexture("terrain2", "assets/forest_1_tileset.png");

	clearMap();
	map = new Map("terrain2", scale, 16);
	map->LoadMap("assets/forest_1_map.map", 20, 15);
}

void Game::init(const char* title, int width, int height, bool fullscreen)
{
	int flags = 0;
	
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		}

		isRunning = true;
	}

	if (TTF_Init() == -1)
	{
		std::cout << "FAILED: Unable to load SDL_TTF" << std::endl;
	}

	addTextures();
	setupPlayer();
	setupStartScreen();
	setupMapOne();
}

auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayers));
auto& terrainColliders(manager.getGroup(Game::groupTerrainColliders));
auto& portalColliders(manager.getGroup(Game::groupPortalColliders));
auto& interactiveObjects(manager.getGroup(Game::groupInteractiveObjects));
auto& dialogues(manager.getGroup(Game::groupDialogues));
auto& screens(manager.getGroup(Game::groupScreenOverlays));

void Game::handleEvents()
{
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT :
		isRunning = false;
		break;
	default:
		break;
	}
}

Vector2D lastPlayerPos;

void Game::update()
{
	if (screens.size() > 0) {
		screens[0]->update();
		manager.refresh();
		return;
	}

	if (entities["dialogueEntity"]->isConnected()) {
		entities["player"]->getComponent<KeyboardController>().receiveInput = false;
		//player.disconnect();
	}
	else {
		entities["player"]->getComponent<KeyboardController>().receiveInput = true;
		//player.connect();
	}

	manager.refresh();
	manager.update();
		
	SDL_Rect playerCol = entities["player"]->getComponent<ColliderComponent>().collider;
	Vector2D playerPos = entities["player"]->getComponent<TransformComponent>().position;
	Vector2D playerVel = entities["player"]->getComponent<TransformComponent>().velocity;

	//interactive objects 
	for (auto& o : interactiveObjects)
	{
		SDL_Rect oCol = o->getComponent<ColliderComponent>().collider;
		if (Collision::InteractiveRangeCollision(oCol, playerCol)) {
			o->getComponent<InteractiveComponent>().contactWithPlayer = true;
		}
		else {
			o->getComponent<InteractiveComponent>().contactWithPlayer = false;
		}
	}

	//check if player has collision with any of the colliders on the map
	bool wasCollision = false;
	for (auto& c : terrainColliders)
	{
		SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;
		if (Collision::AABB(cCol, playerCol))
		{
			// set player to last position that isn't colliding with anything
			entities["player"]->getComponent<TransformComponent>().position = lastPlayerPos;
			wasCollision = true;
		}
	}
	if (!wasCollision) lastPlayerPos = playerPos;



	// player collision with portal colliders
	for (auto& c : portalColliders)
	{
		SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;
		if (Collision::AABB(cCol, playerCol))
		{
			accessMapPortal();
			//setupMapTwo();
			//player.getComponent<TransformComponent>().position = Vector2D(1250,250);
			break;
		}
	}


	//take away half of screen to keep player in middle
	camera.x = entities["player"]->getComponent<TransformComponent>().position.x - (screenX/2);
	camera.y = entities["player"]->getComponent<TransformComponent>().position.y - (screenY/2);
	
	// check bounds to avoid showing null map
	// TODO: Make it work for fullscreen
	if (camera.x < 0)
		camera.x = 0;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.x > screenX * scale - screenX)
		camera.x = screenX * scale - screenX;
	if (camera.y > screenY * scale - screenY)
		camera.y = screenY * scale - screenY;
}

void Game::render()
{
	SDL_RenderClear(renderer);

	for (auto& s : screens)
	{
		s->draw();
	}
	if (screens.size() > 0) {
		SDL_RenderPresent(renderer);
		return;
	}


	for (auto& t : tiles)
	{
		t->draw();
	}
	for (auto& c : terrainColliders)
	{
		//c->draw();
	}
	for (auto& pc : portalColliders)
	{
		pc->draw();
	}
	for (auto& p : players)
	{
		p->draw();
	}
	for (auto& o : interactiveObjects)
	{
		o->draw();
	}
	for (auto& d : dialogues)
	{
		d->draw();
	}

	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void clearMap()
{
	/*std::unordered_map<std::string, Entity*>::iterator itr = entities.begin();
	while (itr != entities.end()) {
		if (itr->first != "player" && itr->first != "dialogueEntity") {
			itr->second->destroy();
			itr = entities.erase(itr);
		}
		else {
			++itr;
		}
	}
	*/
	for (auto& t : tiles)
	{
		t->destroy();
	}
	for (auto& c : terrainColliders)
	{
		c->destroy();
	}
	for (auto& p : portalColliders)
	{
		p->destroy();
	}
	for (auto& o : interactiveObjects)
	{
		//o->destroy();
	}
}

void accessMapPortal()
{
	std::string mapName = map->mapName();
	if (mapName == "terrain2") 
	{
		setupMapOne();
	}
	else if (mapName == "terrain")
	{
		setupMapTwo();
	}
}