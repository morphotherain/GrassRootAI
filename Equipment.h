#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "AttributesComponent.h"	
#include "EquipmentComponent.h"
#include "MinerComponent.h"



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

class Miner : public Equipment
{
public:
	Miner() = default;
	Miner(UINT _objectID) :Equipment(_objectID) {};
	~Miner() = default;

	virtual void Init();
	virtual void handleTask(const Task& task);
	virtual void Update(UINT tick) override;
	std::shared_ptr<MinerComponent> m_pMiner;
};


class ThrustEquipments : public Equipment
{
public:
	ThrustEquipments() = default;
	ThrustEquipments(UINT _objectID) :Equipment(_objectID) {};
	~ThrustEquipments() = default;

	virtual void handleTask(const Task& task);
};

class MiningImprovementEquipments : public Equipment
{
public:
	MiningImprovementEquipments() = default;
	MiningImprovementEquipments(UINT _objectID) :Equipment(_objectID) {};
	~MiningImprovementEquipments() = default;
};