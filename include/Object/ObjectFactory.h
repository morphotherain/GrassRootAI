#pragma once

#include "dynGameObjectsManager.h"
#include "GameObject.h"
#include <memory>

class ObjectFactory
{
public:
	static std::shared_ptr<GameObject> CreateFromDynObject(const dynGameObject& objectData);
};
