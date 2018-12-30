#include "AssetManager.h"
#include "ECS\Components.h"

AssetManager::AssetManager(Manager* man) : manager(man)
{}

AssetManager::~AssetManager()
{}

void AssetManager::CreateDialogue(Vector2D pos, std::string id, Entity& dialogue, std::string text)
{
	//auto& dialogue(manager->addEntity());
	dialogue.addComponent<TransformComponent>(pos.x, pos.y, 180, 620, 1);
	dialogue.addComponent<SpriteComponent>(id, false, true);
	SDL_Color white = { 255, 255, 255 };
	dialogue.addComponent<UILabel>(pos.x + 20, 460, text, "Determination", white, false);
	dialogue.addComponent<DialogueComponent>();
	dialogue.addComponent<KeyboardController>();
	dialogue.addGroup(Game::groupDialogues);
	//return dialogue;
}

void AssetManager::AddTexture(std::string id, const char* path)
{
	textures.emplace(id, TextureManager::LoadTexture(path));
}

SDL_Texture* AssetManager::GetTexture(std::string id)
{
	return textures[id];
}

void AssetManager::RemoveTexture(std::string id)
{
	textures.erase(id);
	return;
}

void AssetManager::AddFont(std::string id, std::string path, int fontSize)
{
	fonts.emplace(id, TTF_OpenFont(path.c_str(), fontSize));
}

TTF_Font* AssetManager::GetFont(std::string id)
{
	return fonts[id];
}