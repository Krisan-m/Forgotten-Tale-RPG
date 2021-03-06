#pragma once

#include <string>
#include <map>
#include "TextureManager.h"
#include "Vector2D.h"
#include "ECS\ECS.h"
#include "SDL_ttf.h"

class AssetManager
{
public:
	AssetManager(Manager* man);
	~AssetManager();

	//game objects
	void CreateDialogue(Vector2D pos, std::string id, Entity& dialogue, std::string text);

	//texture management
	void AddTexture(std::string id, const char* path);
	SDL_Texture* GetTexture(std::string id);
	void RemoveTexture(std::string id);

	void AddFont(std::string id, std::string path, int fontSize);
	TTF_Font* GetFont(std::string id);

private:

	Manager* manager;
	std::map<std::string, SDL_Texture*> textures;
	std::map<std::string, TTF_Font*> fonts;

};