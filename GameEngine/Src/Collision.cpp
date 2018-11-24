#include "Collision.h"
#include "SDL.h"
#include "ECS\ColliderComponent.h"

bool Collision::AABB(const SDL_Rect& recA, const SDL_Rect& recB)
{
	if (
		recA.x + recA.w >= recB.x &&
		recB.x + recB.w >= recA.x &&
		recA.y + recA.h >= recB.y &&
		recB.y + recB.h >= recA.y
		)
	{
		std::cout << "collision" << std::endl;
		return true;
	}

	return false;
}

bool Collision::AABB(const ColliderComponent& colA, const ColliderComponent& colB)
{
	if (AABB(colA.collider, colB.collider))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool Collision::InteractiveRangeCollision(const SDL_Rect& recA, const SDL_Rect& recB)
{
	if (
		recA.x + recA.w + 5 >= recB.x &&
		recB.x + recB.w + 5 >= recA.x &&
		recA.y + recA.h + 5 >= recB.y &&
		recB.y + recB.h + 5 >= recA.y
		)
	{
		//std::cout << "within interaction range" << std::endl;
		return true;
	}

	return false;
}

bool Collision::InteractiveRangeCollision(const ColliderComponent& colA, const ColliderComponent& colB)
{
	if (InteractiveRangeCollision(colA.collider, colB.collider))
	{
		return true;
	}
	else
	{
		return false;
	}
}
