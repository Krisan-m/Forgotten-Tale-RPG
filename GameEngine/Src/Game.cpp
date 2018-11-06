#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "AssetManager.h"
#include <sstream>

Map* map;
Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
int screenX = 800;
int screenY = 640;
int scale = 3;
SDL_Rect Game::camera = {0, 0, screenX * scale, screenY * scale}; //width and height of map

AssetManager* Game::assets = new AssetManager(&manager);

bool Game::isRunning = false;

auto& player(manager.addEntity());
auto& label(manager.addEntity());


Game::Game()
{}

Game::~Game()
{}

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

	assets->AddTexture("terrain", "assets/cave_1_ss.png");
	assets->AddTexture("player", "assets/character_spritesheet.png");
	assets->AddTexture("dialogue", "assets/DialogueBackground.png");

	assets->AddFont("Determination", "assets/Determination.ttf", 16);

	map = new Map("terrain", scale, 16);
	map->LoadMap("assets/cave_1.map", 50, 40);


	player.addComponent<TransformComponent>(camera.w/2, camera.h/2, 31, 19, scale);
	player.addComponent<SpriteComponent>("player", true);
	player.addComponent<KeyboardController>();
	player.addComponent<ColliderComponent>("player");
	player.addGroup(groupPlayers);

	assets->CreateDialogue(Vector2D(1200, 960), "dialogue");


	SDL_Color white = { 255, 255, 255 };
	label.addComponent<UILabel>(10, 10, "You have awoken.", "Determination", white);

	
}


auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayers));
auto& terrainColliders(manager.getGroup(Game::groupTerrainColliders));
auto& portalColliders(manager.getGroup(Game::groupPortalColliders));
auto& dialogues(manager.getGroup(Game::groupDialogues));

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

	SDL_Rect playerCol = player.getComponent<ColliderComponent>().collider;
	Vector2D playerPos = player.getComponent<TransformComponent>().position;
	Vector2D playerVel = player.getComponent<TransformComponent>().velocity;

	std::stringstream ss;
	ss << "Player position: " << playerPos;
	label.getComponent<UILabel>().SetLabelText(ss.str(), "Determination");

	manager.refresh();
	manager.update();

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

	//check bounds to avoid showing null map
	if (camera.x < 0)
		camera.x = 0;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.x > camera.w - screenX)
		camera.x = camera.w - screenX;
	if (camera.y > camera.h - screenY)
		camera.y = camera.h - screenY;	
}


void Game::render()
{
	SDL_RenderClear(renderer);

	for (auto& t : tiles)
	{
		t->draw();
	}
	for (auto& c : terrainColliders)
	{
		c->draw();
	}
	for (auto& pc : portalColliders)
	{
		pc->draw();
	}
	for (auto& p : players)
	{
		p->draw();
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