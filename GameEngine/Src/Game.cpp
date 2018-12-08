#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "AssetManager.h"
#include <sstream>
#include <vector>

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

auto& player(manager.addEntity());
auto& label(manager.addEntity());
auto& dialogueEntity(manager.addEntity());
auto& fire(manager.addEntity());
auto& startScreen(manager.addEntity());

Game::Game()
{}

Game::~Game()
{}

bool Game::inStartMenu() 
{
	return inStart;
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

	//run start menu
	assets->AddTexture("startScreen", "assets/StartScreen.png");
	startScreen.addComponent<TransformComponent>(0, 0, 640, 800, 1);
	startScreen.addComponent<SpriteComponent>("startScreen", false, true);
	startScreen.addComponent<KeyboardController>();
	startScreen.addGroup(groupScreenOverlays);


	assets->AddTexture("terrain", "assets/room_1_tileset.png");
	assets->AddTexture("player", "assets/character_spritesheet.png");
	assets->AddTexture("fireplace", "assets/fireplace_spritesheet.png");
	
	assets->AddTexture("dialogue", "assets/DialogueBackground.png");
	assets->AddFont("Determination", "assets/Determination.ttf", 32);

	fire.addComponent<TransformComponent>(1250, 0, 64, 32, scale);
	fire.addComponent<ColliderComponent>("fireplace");
	fire.addComponent<InteractiveComponent>("The fire is almost out. It needs some wood.");
	std::vector<int> animationIndexFrame{7};  //animations, frames
	fire.addComponent<SpriteComponent>("fireplace", true, animationIndexFrame);
	fire.addComponent<KeyboardController>();
	fire.addGroup(groupTerrainColliders);
	fire.addGroup(groupInteractiveObjects);

	map = new Map("terrain", scale, 16);
	map->LoadMap("assets/room_1.map", 50, 40);

	player.addComponent<TransformComponent>(1250, 250, 31, 19, scale);
	player.addComponent<SpriteComponent>("player", true);
	player.addComponent<ColliderComponent>("player");
	player.addComponent<KeyboardController>();
	player.addGroup(groupPlayers);

	assets->CreateDialogue(Vector2D(screenX/2 - 290, 450), "dialogue", dialogueEntity, "You have awoken from a deep slumber.");
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

	if (dialogueEntity.isConnected()) {
		player.getComponent<KeyboardController>().receiveInput = false;
		//player.disconnect();
	}
	else {
		player.getComponent<KeyboardController>().receiveInput = true;
		//player.connect();
	}
		
	SDL_Rect playerCol = player.getComponent<ColliderComponent>().collider;
	Vector2D playerPos = player.getComponent<TransformComponent>().position;
	Vector2D playerVel = player.getComponent<TransformComponent>().velocity;

	manager.refresh();
	manager.update();

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
			player.getComponent<TransformComponent>().position = lastPlayerPos;
			wasCollision = true;
		}
	}
	if (!wasCollision) lastPlayerPos = playerPos;

	//take away half of screen to keep player in middle
	camera.x = player.getComponent<TransformComponent>().position.x - (screenX/2);
	camera.y = player.getComponent<TransformComponent>().position.y - (screenY/2);
	
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

	label.draw();
	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}