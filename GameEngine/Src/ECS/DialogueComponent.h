#pragma once

#include "ECS.h"
#include "Components.h"

class DialogueComponent : public Component
{
public:
	DialogueComponent()
	{}
	~DialogueComponent() {};
	
	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
	}

	void update() override
	{

	}

private:
	TransformComponent* transform;
};