#pragma once
#include "ECS.h"
#include "Components.h"

class DialogueComponent : public Component
{
private:
	TransformComponent* transform;
	SpriteComponent* sprite;

public:
	DialogueComponent()
	{
	}
	~DialogueComponent() {}
	
	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		sprite = &entity->getComponent<SpriteComponent>();
	}

	void nextScreen() 
	{
		std::cout << "trigger logic " << std::endl;
		entity->hide();
		entity->destroy();
		//entity->delGroup(Game::groupDialogues);
	}
};