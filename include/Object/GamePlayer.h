#pragma once
#include "GameObject.h"
#include "TransformComponent.h"

class GamePlayer : public GameObject
{
public:
	void moveX(float d) { GetComponent<TransformComponent>()->position.x += d; };
	void moveY(float d) { GetComponent<TransformComponent>()->position.y += d; };
};
