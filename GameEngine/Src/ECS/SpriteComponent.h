#pragma once

#include "Components.h"
#include "SDL.h"
#include "../TextureManager.h"
#include "Animation.h"
#include <map>
#include "../AssetManager.h"
#include <vector>

class SpriteComponent : public Component
{
private:
	TransformComponent *transform;
	SDL_Texture *texture;
	std::string texID;
	SDL_Rect srcRect, destRect;
	std::vector<int> animationInfo;
	bool overlay = false;
	bool hideSprite = false;

	bool animated = false;
	int frames = 0;
	int speed = 100; //delay between frames in ms

public:

	int animIndex = 0;
	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

	std::map<std::string, Animation> animations;

	SpriteComponent() = default;
	SpriteComponent(std::string id)
	{
		texID = id;
		setTex(id);
	}

	SpriteComponent(std::string id, bool isAnimated, bool isOverlay)
	{
		animated = isAnimated;
		overlay = isOverlay;
		texID = id;
		setTex(id);
	}

	SpriteComponent(std::string id, bool isAnimated)
	{
		animated = isAnimated;
		texID = id;
		setTex(id);
	}

	SpriteComponent(std::string id, bool isAnimated, std::vector<int> animInfo)
	{
		animated = isAnimated;
		animationInfo = animInfo;
		texID = id;
		setTex(id);
	}

	~SpriteComponent()
	{
		SDL_DestroyTexture(texture);
	}

	std::string getTexID()
	{
		return texID;
	}

	void setTex(std::string id)
	{
		texID = id;
		texture = Game::assets->GetTexture(id);
		
	}

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();

		// General way of adding spritesheet info to SpriteComponent animation
		if (texID != "player" && animated) {
			for (int i = 0; i < animationInfo.size(); i++) {
				animations.emplace(std::to_string(i), Animation(i, animationInfo[i], 100));
			}
			Play("0");
		}
		else if(animated) {
			Animation idle_front = Animation(0, 1, 100);
			Animation idle_side = Animation(1, 1, 100);
			Animation idle_back = Animation(2, 1, 100);

			Animation walk_down = Animation(0, 6, 100);
			Animation walk_left = Animation(1, 6, 100);
			Animation walk_up = Animation(2, 6, 100);

			animations.emplace("Idle Front", idle_front);
			animations.emplace("Idle Side", idle_side);
			animations.emplace("Idle Back", idle_back);
			animations.emplace("Walk Down", walk_down);
			animations.emplace("Walk Left", walk_left);
			animations.emplace("Walk Up", walk_up);

			Play("Idle Front");
		}
		
		srcRect.x = srcRect.y = 0;
		srcRect.w = transform->width;
		srcRect.h = transform->height;
	}

	void update() override
	{
		if (animated)
		{
			srcRect.x = srcRect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
		}

		if (!overlay)
		{
			srcRect.y = animIndex * transform->height;

			destRect.x = static_cast<int>(transform->position.x) - Game::camera.x;
			destRect.y = static_cast<int>(transform->position.y) - Game::camera.y;
			destRect.w = transform->width * transform->scale;
			destRect.h = transform->height * transform->scale;
		}
		else {
			srcRect.y = animIndex * transform->height;

			destRect.x = static_cast<int>(transform->position.x);
			destRect.y = static_cast<int>(transform->position.y);
			destRect.w = transform->width * transform->scale;
			destRect.h = transform->height * transform->scale;
		}
	}

	void draw() override
	{
		if (!hideSprite) {
			TextureManager::Draw(texture, srcRect, destRect, spriteFlip);
		}
	}

	void setHideSprite(bool x) {
		hideSprite = x;
	}

	void Play(const char* animName)
	{
		frames = animations[animName].frames;
		animIndex = animations[animName].index;
		speed = animations[animName].speed;
	}
};
