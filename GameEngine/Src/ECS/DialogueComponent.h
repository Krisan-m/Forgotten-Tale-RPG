#pragma once
#include "ECS.h"
#include "Components.h"

class DialogueComponent : public Component
{
private:
	TransformComponent* transform;
	SpriteComponent* sprite;
	UILabel* label;

public:
	DialogueComponent()
	{
	}
	~DialogueComponent() {}
	
	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		label = &entity->getComponent<UILabel>();
		sprite = &entity->getComponent<SpriteComponent>();
	}

	void nextScreen() 
	{
		entity->hide();
		sprite->destroy();
		label->SetLabelText("", "Determination");
	}
};