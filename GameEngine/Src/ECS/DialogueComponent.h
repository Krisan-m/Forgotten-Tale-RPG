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

	bool nextScreen() 
	{
		if (label->fullyDrawn()) {
			entity->hide();
			sprite->destroy();
			label->SetLabelText("", "Determination");
			label->setCompleteDrawing(false);
			return true;
		}
		else {
			label->fullyDraw();
			return false;
		}
	}

	void showDialogue(std::string text)
	{
		entity->show();
		sprite->draw();
		label->SetNewText(text);
	}
};