#pragma once
#include "ECS.h"
#include "Components.h"

class DialogueComponent : public Component
{
private:
	TransformComponent* transform;
	SpriteComponent* sprite;
	UILabel* label;
	bool dialogueBeingShown = false;

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
			dialogueBeingShown = false;
			return true;
		}
		else {
			label->fullyDraw();
			return false;
		}
	}

	bool isBeingShown() {
		return dialogueBeingShown;
	}

	void showDialogue(std::string text)
	{
		dialogueBeingShown = true;
		entity->show();
		sprite->setTex(sprite->getTexID());
		sprite->draw();
		label->SetNewText(text);
	}
};