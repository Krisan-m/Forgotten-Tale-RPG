#pragma once

#include "../Game.h"
#include "ECS.h"
#include "Components.h"
#include "Collision.h"

class KeyboardController : public Component
{
public:
	TransformComponent *transform;
	SpriteComponent *sprite;
	DialogueComponent *dialogue;
	InteractiveComponent *interactiveObject;
	bool receiveInput = true;

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		sprite = &entity->getComponent<SpriteComponent>();
		dialogue = &entity->getComponent<DialogueComponent>();
		interactiveObject = &entity->getComponent<InteractiveComponent>();
	}

	void update() override
	{

		if (entity->hasGroup(Game::groupDialogues)) {
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_b:
				// go to nebxt dialog
				if (entity->hasGroup(Game::groupDialogues)) {
					sprite->setHideSprite(false);
				}
				break;
			default:
				break;
			}
		}

		// case when dialogue is opened
		if (!receiveInput || (entity->hasGroup(Game::groupDialogues) && entity->isConnected())) {
			if (entity->hasGroup(Game::groupPlayers)) {
				if(transform->velocity.x == 1) sprite->Play("Idle Side");
				else if (transform->velocity.x == -1) {
					sprite->Play("Idle Side");
					sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
				}
				else if (transform->velocity.y == 1) sprite->Play("Idle Front");
				else if (transform->velocity.y == -1) sprite->Play("Idle Back");
				transform->velocity.x = 0;
				transform->velocity.y = 0;
			}
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

		// deal with interactive objects
		if (entity->hasGroup(Game::groupInteractiveObjects)) {
			if (Game::event.type == SDL_KEYDOWN) {
				switch (Game::event.key.keysym.sym)
				{
				case SDLK_z:
					if (interactiveObject->contactWithPlayer == true) {
						interactiveObject->action();
					}
					break;
				default:
					break;
				}
			}
			return;
		}
		if (Game::event.type == SDL_KEYDOWN && entity->hasGroup(Game::groupPlayers))
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
			default:
				break;
			}
		}

		if (Game::event.type == SDL_KEYUP && entity->hasGroup(Game::groupPlayers))
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
