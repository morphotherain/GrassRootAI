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

	std::shared_ptr<BaseComponent> m_pBase;
	std::shared_ptr<AttributesComponent> m_pAttributes;
	std::shared_ptr<EquipmentComponent> m_pEquipment;
};



class ThrustEquipments : public Equipment   //groupID 46 推进改良设备
{
public:
	ThrustEquipments() = default;
	ThrustEquipments(UINT _objectID) :Equipment(_objectID) {};
	~ThrustEquipments() = default;

	virtual void handleTask(const Task& task);
};

class EnergyWeaponEquipments : public Equipment   //groupID 53 能量武器
{
public:
	EnergyWeaponEquipments() = default;
	EnergyWeaponEquipments(UINT _objectID) :Equipment(_objectID) {};
	~EnergyWeaponEquipments() = default;

	virtual void Init();
	virtual void handleTask(const Task& task);
	virtual void Update(UINT tick) override;
	std::shared_ptr<WeaponComponent> m_pWeapon;
};

class MinerEquipments : public Equipment   //groupID 54 采矿激光器
{
public:
	MinerEquipments() = default;
	MinerEquipments(UINT _objectID) :Equipment(_objectID) {};
	~MinerEquipments() = default;

	virtual void Init();
	virtual void handleTask(const Task& task);
	virtual void Update(UINT tick) override;
	std::shared_ptr<MinerComponent> m_pMiner;
};




class MiningImprovementEquipments : public Equipment   //groupID 546 采矿改良设备
{
public:
	MiningImprovementEquipments() = default;
	MiningImprovementEquipments(UINT _objectID) :Equipment(_objectID) {};
	~MiningImprovementEquipments() = default;
};