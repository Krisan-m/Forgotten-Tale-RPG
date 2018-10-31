#pragma once

#include "Components.h"
#include "SDL.h"
#include "../TextureManager.h"
#include "Animation.h"
#include <map>

class SpriteComponent : public Component
{
private:
	TransformComponent *transform;
	SDL_Texture *texture;
	SDL_Rect srcRect, destRect;

	bool animated = false;
	int frames = 0;
	int speed = 100; //delay between frames in ms

public:

	int animIndex = 0;
	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

	std::map<const char*, Animation> animations
		;
	SpriteComponent() = default;
	SpriteComponent(const char* path)
	{
		setTex(path);
	}

	SpriteComponent(const char* path, bool isAnimated)
	{
		animated = isAnimated;

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
		setTex(path);
	}

	~SpriteComponent()
	{
		SDL_DestroyTexture(texture);
	}

	void setTex(const char* path)
	{
		texture = TextureManager::LoadTexture(path);
	}

	void init() override
	{

		transform = &entity->getComponent<TransformComponent>();

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

		srcRect.y = animIndex * transform->height;

		destRect.x = static_cast<int>(transform->position.x) - Game::camera.x;
		destRect.y = static_cast<int>(transform->position.y) - Game::camera.y;
		destRect.w = transform->width * transform->scale;
		destRect.h = transform->height * transform->scale;
	}

	void draw() override
	{
		TextureManager::Draw(texture, srcRect, destRect, spriteFlip);
	}

	void Play(const char* animName)
	{
		frames = animations[animName].frames;
		animIndex = animations[animName].index;
		speed = animations[animName].speed;
	}
};
