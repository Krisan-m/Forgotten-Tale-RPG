#pragma once

#include "../Game.h"
#include "ECS.h"
#include "Components.h"


class KeyboardController : public Component
{
public:
	TransformComponent *transform;
	SpriteComponent *sprite;
	DialogueComponent *dialogue;
	bool receiveInput = true;

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		sprite = &entity->getComponent<SpriteComponent>();
		dialogue = &entity->getComponent<DialogueComponent>();
	}

	void update() override
	{
		if (!receiveInput || (entity->hasGroup(Game::groupDialogues) && entity->isVisible())) {
			if (Game::event.type == SDL_KEYDOWN)
				switch (Game::event.key.keysym.sym)
				{
				case SDLK_z:
					// go to next dialog
					if (entity->hasGroup(Game::groupDialogues)) {
						dialogue->nextScreen();
					}
					break;
				default:
					break;
				}
			return;
		}
		if (Game::event.type == SDL_KEYDOWN)
		{
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_UP:
				transform->velocity.y = -1;
				transform->velocity.x = 0;
				sprite->Play("Walk Up");
				break;
			case SDLK_LEFT:
				transform->velocity.x = -1;
				transform->velocity.y = 0;
				sprite->Play("Walk Left");
				sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
				break;
			case SDLK_RIGHT:
				transform->velocity.x = 1;
				transform->velocity.y = 0;
				sprite->Play("Walk Left");
				sprite->spriteFlip = SDL_FLIP_NONE;
				break;
			case SDLK_DOWN:
				transform->velocity.y = 1;
				transform->velocity.x = 0;
				sprite->Play("Walk Down");
				break;
			case SDLK_z:
				// check if player entity is near another collider with interactive 
				auto& objects(entity->getManager().getGroup(Game::groupInteractiveObjects));
				break;
			default:
				break;
			}
		}

		if (Game::event.type == SDL_KEYUP)
		{
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_UP:
				transform->velocity.y = 0;
				if(transform->velocity.x == 0) sprite->Play("Idle Back");
				break;
			case SDLK_LEFT:
				transform->velocity.x = 0;
				if (transform->velocity.y == 0) {
					sprite->Play("Idle Side");
					sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
				}
				break;
			case SDLK_RIGHT:
				transform->velocity.x = 0;
				if (transform->velocity.y == 0) sprite->Play("Idle Side");
				break;
			case SDLK_DOWN:
				transform->velocity.y = 0;
				if (transform->velocity.x == 0) sprite->Play("Idle Front");
				break;
			case SDLK_F4:
				Game::isRunning = false;
			default:
				break;
			}

		}
	}

};
