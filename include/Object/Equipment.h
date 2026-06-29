#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "AttributesComponent.h"
#include "EquipmentComponent.h"
#include "MinerComponent.h"
#include "WeaponComponent.h"

class Equipment : public GameObject
{
public:
	Equipment() = default;
	Equipment(UINT _objectID) :objectID(_objectID) {};
	~Equipment() = default;

	virtual void Init();
	virtual void Update(UINT tick) override;
	virtual std::shared_ptr<GameObject> ConvertBasedOnGroupID(UINT groupID);
	virtual void handleTask(const Task& task) {};

	UINT objectID;
};

class ThrustEquipments : public Equipment
{
public:
	ThrustEquipments() = default;
	ThrustEquipments(UINT _objectID) :Equipment(_objectID) {};
	~ThrustEquipments() = default;

	virtual void handleTask(const Task& task);
};

class EnergyWeaponEquipments : public Equipment
{
public:
	EnergyWeaponEquipments() = default;
	EnergyWeaponEquipments(UINT _objectID) :Equipment(_objectID) {};
	~EnergyWeaponEquipments() = default;

	virtual void Init();
	virtual void handleTask(const Task& task);
	virtual void Update(UINT tick) override;
};

class MinerEquipments : public Equipment
{
public:
	MinerEquipments() = default;
	MinerEquipments(UINT _objectID) :Equipment(_objectID) {};
	~MinerEquipments() = default;

	virtual void Init();
	virtual void handleTask(const Task& task);
	virtual void Update(UINT tick) override;
};

class MiningImprovementEquipments : public Equipment
{
public:
	MiningImprovementEquipments() = default;
	MiningImprovementEquipments(UINT _objectID) :Equipment(_objectID) {};
	~MiningImprovementEquipments() = default;
};
