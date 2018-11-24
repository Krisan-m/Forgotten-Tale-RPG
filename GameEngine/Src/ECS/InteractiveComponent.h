#pragma once

#include "Components.h"
#include "SDL.h"
#include "../TextureManager.h"
#include "../AssetManager.h"

class InteractiveComponent : public Component
{
private:
	TransformComponent *transform;
	DialogueComponent * dialogue;

public:
	bool contactWithPlayer = false;
	//InteractiveComponent() = default;
	InteractiveComponent()
	{
	}

	~InteractiveComponent()
	{
	}

	void action() { std::cout << "INTERACTIVE OBJECT ACTION" << std::endl; }

	void init() override
	{
	}

	void update() override
	{

	}

	void draw() override
	{
		//TextureManager::Draw(texture, srcRect, destRect, spriteFlip);
	}
};
