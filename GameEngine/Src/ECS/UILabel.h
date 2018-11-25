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
	UILabel(int xpos, int ypos, std::string text, std::string font, SDL_Color& colour) :
		labelText(text), labelFont(font), textColour(colour)
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

	void draw() override
	{
		if (progress <= labelText.length()*5 + 5) SetLabelText(labelText, labelFont);
		SDL_RenderCopy(Game::renderer, labelTexture, nullptr, &position); //nullptr for source rect since we want to draw all of it
	}

	bool fullyDrawn() {
		return completeDrawing;
	}

	void fullyDraw() {
		progress = labelText.length() * 5;
		SetLabelText(labelText, labelFont);
	}

private:
	SDL_Rect position;
	int progress = 5;
	bool completeDrawing = false;
	std::string labelText;
	std::string labelFont;
	SDL_Color textColour;
	SDL_Texture* labelTexture;

};
