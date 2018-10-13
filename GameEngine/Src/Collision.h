#pragma once
#include <SDL.h>

class Collision
{
public:
	// Axis aligned bounding box collision
	static bool AABB(const SDL_Rect& recA, const SDL_Rect& recB);
};