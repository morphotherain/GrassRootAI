#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "AttributesComponent.h"
#include "EquipmentsComponent.h"
#include <DirectXMath.h>
#include "PhysicsComponent.h"
#include "StorageComponent.h"
#include "LockingComponent.h"

enum class ShipWarpState
{
	None,          // 表示没有处于跃迁相关状态，正常飞行等情况
	PreparingWarp, // 表示正在准备跃迁，例如正在调整船头朝向和速度以满足跃迁条件
	Warping        // 表示正在进行跃迁操作
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
	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<AttributesComponent> m_pAttributes;
	std::shared_ptr<EquipmentsComponent> m_pEquipments;
	std::shared_ptr<PhysicsComponent> m_pPhysics;
	std::shared_ptr<SpaceTransformComponent>m_pSpaceTran;

	std::shared_ptr<PilotStorageComponent> m_pPilotStorage;
	std::shared_ptr<CargoContainerComponent> m_pCargoStorage;
	std::shared_ptr<HighSlotComponent> m_pHighSlotStorage;
	std::shared_ptr<MediumSlotComponent> m_pMediumSlotStorage;
	std::shared_ptr<LowSlotComponent> m_pLowSlotStorage;
	std::shared_ptr<RigSlotComponent> m_pRigSlotStorage;

	std::shared_ptr<LockingComponent> m_pLocking;

	void fillObjectName();

	std::weak_ptr<GameObject> approachTarget;
	std::weak_ptr<GameObject> warpTarget;
	std::weak_ptr<GameObject> activeTarget;

	ShipWarpState currentWarpState = ShipWarpState::None; // 记录飞船当前的跃迁状态

	virtual void handleTask(const Task& task);
	void handleApproach(std::shared_ptr<GameObject> target);
	void handleActive(std::shared_ptr<GameObject> target);
	void handleWarp(std::shared_ptr<GameObject> target);

	void updateEquipments(int tick);

	// 实现 ConvertBasedOnGroupID 方法
	static std::shared_ptr<GameObject> ConvertBasedOnGroupID(UINT groupID, UINT objectID);
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