#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "AttributesComponent.h"
#include "EquipmentsComponent.h"
#include "PhysicsComponent.h"
#include "StorageComponent.h"
#include "LockingComponent.h"
#include "EntityTaskHandlers.h"
#include <DirectXMath.h>

struct Task;

enum class ShipWarpState
{
	None,
	PreparingWarp,
	Warping
};

class Ship : public GameObject {
public:
	Ship() = default;
	explicit Ship(UINT _objectID) { objectID = _objectID; }
	~Ship() = default;

	virtual void Init();
	virtual void Update(UINT tick);

	void fillObjectName();

	std::weak_ptr<GameObject> approachTarget;
	std::weak_ptr<GameObject> warpTarget;
	std::weak_ptr<GameObject> activeTarget;

	ShipWarpState currentWarpState = ShipWarpState::None;

	void handleApproach(std::shared_ptr<GameObject> target);
	void handleActive(std::shared_ptr<GameObject> target);
	void handleWarp(std::shared_ptr<GameObject> target);

	void updateEquipments(int tick);

protected:
	void registerEntityTaskHandlers(EntityTaskHandlerMap& handlers) override;
};

class Frigate : public Ship {
public:
	Frigate(UINT _objectID) : Ship(_objectID) {}
};

class Cruiser : public Ship {
public:
	Cruiser(UINT _objectID) : Ship(_objectID) {}
};

class Battleship : public Ship {
public:
	Battleship(UINT _objectID) : Ship(_objectID) {}
};

class Capsule : public Ship {
public:
	Capsule(UINT _objectID) : Ship(_objectID) {}
};
