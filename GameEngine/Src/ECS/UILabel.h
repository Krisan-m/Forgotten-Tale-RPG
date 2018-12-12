#pragma once
#include "ECS.h"
#include "../AssetManager.h"
#include "../Game.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class UILabel : public Component
{
public:
	UILabel(int xpos, int ypos, std::string text, std::string font, SDL_Color& colour, bool isFlashing) :
		labelText(text), labelFont(font), textColour(colour), flashing(isFlashing)
	{
		position.x = xpos;
		position.y = ypos;

		SetLabelText(labelText, labelFont);
	}

	~UILabel()
	{}

	void SetNewText(std::string text) 
	{
		progress = 0;
		SetLabelText(text, "Determination");
	}

	void SetLabelText(std::string text, std::string font)
	{
		labelText = text;
		std::string displayString = text.substr(0, progress/5);
		if(progress != text.length()*5) progress++;
		else completeDrawing = true;

		SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(Game::assets->GetFont(font), displayString.c_str(), textColour, 500);
		labelTexture = SDL_CreateTextureFromSurface(Game::renderer, surf);
		SDL_FreeSurface(surf);

		//sets width and height
		SDL_QueryTexture(labelTexture, nullptr, nullptr, &position.w, &position.h);
	}

	void SetTempText(std::string text)
	{
		SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(Game::assets->GetFont(labelFont), text.c_str(), textColour, 500);
		labelTexture = SDL_CreateTextureFromSurface(Game::renderer, surf);
		SDL_FreeSurface(surf);

		//sets width and height
		SDL_QueryTexture(labelTexture, nullptr, nullptr, &position.w, &position.h);

	}

	void draw() override
	{
		if (!completeDrawing && progress <= labelText.length()*5) SetLabelText(labelText, labelFont);
		if (completeDrawing && flashing) {
			flashCounter++;
			if (flashCounter == 20) {
				SetTempText("");
			}
			else if (flashCounter == 30) {
				SetTempText(labelText);
				flashCounter = 0;
			}
		}
		SDL_RenderCopy(Game::renderer, labelTexture, nullptr, &position); //nullptr for source rect since we want to draw all of it
	}

	bool fullyDrawn() {
		return completeDrawing;
	}

	void setCompleteDrawing(bool x) {
		completeDrawing = x;
		return;
	}

	void fullyDraw() {
		progress = labelText.length() * 5;
		SetLabelText(labelText, labelFont);
	}

private:
	SDL_Rect position;
	int progress = 5;
	bool completeDrawing = false;
	bool flashing = false;
	int flashCounter = 0;
	std::string labelText;
	std::string labelFont;
	SDL_Color textColour;
	SDL_Texture* labelTexture;

};
