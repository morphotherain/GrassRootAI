#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "AttributesComponent.h"


class Mineral : public GameObject
{
public:
	Mineral() = default;
	Mineral(UINT _objectID) :objectID(_objectID) {};
	~Mineral() = default;

	virtual void Init();
	virtual void Update(UINT tick);

	UINT objectID;
	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<AttributesComponent> m_pAttributes;
};

class Material : public GameObject
{
public:
	Material() = default;
	Material(UINT _objectID) :objectID(_objectID) {};
	~Material() = default;

	virtual void Init() {};
	virtual std::shared_ptr<GameObject> ConvertBasedOnGroupID(UINT groupID);

	UINT objectID;

	std::shared_ptr<BaseComponent> m_pBase;
};
