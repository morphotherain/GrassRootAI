#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"

class Miner : public GameObject
{
public:
	Miner() = default;
	Miner(UINT _objectID) :objectID(_objectID) {};
	~Miner() = default;

	virtual void Init();

	UINT objectID;
	std::shared_ptr<BaseComponent> m_pBase;
};

class Equipment : public GameObject
{
public:
	Equipment() = default;
	Equipment(UINT _objectID) :objectID(_objectID) {};
	~Equipment() = default;

	virtual void Init() {};
	virtual std::shared_ptr<GameObject> ConvertBasedOnGroupID(UINT groupID);

	UINT objectID;

	std::shared_ptr<BaseComponent> m_pBase;
};
