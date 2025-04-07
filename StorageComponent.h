#pragma once

#include "Component.h"
#include "StorageMacro.h"
#include "d3dUtil.h"
#include <vector>

class StorageComponent : public Component {
public:
	StorageComponent() = default;
	StorageComponent(UINT _objectID, UINT _containerTypeID);
	~StorageComponent() = default;

	virtual void Update(UINT tick) {};

	UINT objectID;
	UINT containerID;
	std::vector<int> itemIDs;
};
// 货柜仓
class CargoContainerComponent : public StorageComponent {
public:
	CargoContainerComponent() = default;

	CargoContainerComponent(UINT _objectID) : StorageComponent(_objectID, CONTAINER_TYPE_CARGO) {}
};

// 无人机挂舱
class DronePodComponent : public StorageComponent {
public:
	DronePodComponent() = default;
	DronePodComponent(UINT _objectID) : StorageComponent(_objectID, CONTAINER_TYPE_DRONE_POD) {}
};

// 装备槽
class EquipmentSlotComponent : public StorageComponent {
public:
	EquipmentSlotComponent() = default;
	EquipmentSlotComponent(UINT _objectID) : StorageComponent(_objectID, CONTAINER_TYPE_EQUIPMENT_SLOT) {}
};

// 弹药
class AmmunitionStorageComponent : public StorageComponent {
public:
	AmmunitionStorageComponent() = default;
	AmmunitionStorageComponent(UINT _objectID) : StorageComponent(_objectID, CONTAINER_TYPE_AMMUNITION_STORAGE) {}
};

// 护卫舰逃生舱
class FrigateEscapePodComponent : public StorageComponent {
public:
	FrigateEscapePodComponent() = default;
	FrigateEscapePodComponent(UINT _objectID) : StorageComponent(_objectID, CONTAINER_TYPE_FRIGATE_ESCAPE_POD) {}
};

// 燃料仓
class FuelTankComponent : public StorageComponent {
public:
	FuelTankComponent() = default;
	FuelTankComponent(UINT _objectID) : StorageComponent(_objectID, CONTAINER_TYPE_FUEL_TANK) {}
};

// 矿石仓
class OreStorageComponent : public StorageComponent {
public:
	OreStorageComponent() = default;
	OreStorageComponent(UINT _objectID) : StorageComponent(_objectID, CONTAINER_TYPE_ORE_STORAGE) {}
};

// 矿物仓
class MineralStorageComponent : public StorageComponent {
public:
	MineralStorageComponent() = default;
	MineralStorageComponent(UINT _objectID) : StorageComponent(_objectID, CONTAINER_TYPE_MINERAL_STORAGE) {}
};

//飞行员仓
class PilotStorageComponent : public StorageComponent {
public:
	PilotStorageComponent() = default;
	PilotStorageComponent(UINT _objectID) : StorageComponent(_objectID, CONTAINER_TYPE_PILOT_STORAGE) {}
};