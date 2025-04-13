
#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "AttributesComponent.h"

class Asteroid : public GameObject
{
public:
	Asteroid() = default;
	Asteroid(UINT _objectID) :objectID(_objectID) {};
	~Asteroid() = default;

	virtual void Init();
	virtual std::shared_ptr<GameObject> ConvertBasedOnGroupID(UINT groupID);
	virtual void handleTask(const Task& task);

	UINT objectID;

	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;
	std::shared_ptr<AttributesComponent> m_pAttributes;

};


class Veldspar : public Asteroid {
public:
	Veldspar(UINT _objectID) : Asteroid(_objectID) {}
};