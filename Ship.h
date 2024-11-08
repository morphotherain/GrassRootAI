#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "PhysicsComponent.h"

class Ship : public GameObject{
public:
	Ship() = default;
	Ship(UINT _objectID) { objectID = (_objectID); }
	~Ship() {};
	
	virtual void Init();
	virtual void Update(UINT tick);

	UINT objectID;
	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<PhysicsComponent> m_pPhysics;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;

};