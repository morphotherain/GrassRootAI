#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "AttributesComponent.h"
#include "EquipmentsComponent.h"
#include "PhysicsComponent.h"
#include "StorageComponent.h"
#include "LockingComponent.h"
#include <DirectXMath.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

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
	Ship(UINT _objectID) { objectID = (_objectID); }
	~Ship() {
		objectID = 1;
	};

	virtual void Init();
	virtual void Update(UINT tick);

	UINT objectID;

	void fillObjectName();

	std::weak_ptr<GameObject> approachTarget;
	std::weak_ptr<GameObject> warpTarget;
	std::weak_ptr<GameObject> activeTarget;

	ShipWarpState currentWarpState = ShipWarpState::None;

	virtual void handleTask(const Task& task);
	void handleApproach(std::shared_ptr<GameObject> target);
	void handleActive(std::shared_ptr<GameObject> target);
	void handleWarp(std::shared_ptr<GameObject> target);

	void updateEquipments(int tick);

	static std::shared_ptr<GameObject> ConvertBasedOnGroupID(UINT groupID, UINT objectID);

	void initTaskHandlers();
	std::unordered_map<std::string, std::function<void(const Task&)>> taskHandlers;
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
