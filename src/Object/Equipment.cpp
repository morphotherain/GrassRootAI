#include "Equipment.h"
#include "BaseComponent.h"
#include "AttributesComponent.h"
#include "EquipmentComponent.h"
#include "WeaponComponent.h"
#include "MinerComponent.h"
#include "InvTypesManager.h"
#include "mapJumpsManager.h"
#include "mapDenormalizeManager.h"
#include "mapSolarSystemsManager.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"
#include "TaskMgr.h"

std::shared_ptr<GameObject> Equipment::ConvertBasedOnGroupID(UINT groupID)
{
	switch (groupID){
	case 46: return std::make_shared<ThrustEquipments>(objectID);
	case 53: return std::make_shared<EnergyWeaponEquipments>(objectID);
	case 54: return std::make_shared<MinerEquipments>(objectID);
	case 546: return std::make_shared<MiningImprovementEquipments>(objectID);
	}
	return nullptr;
}

void Equipment::Init()
{
	ResolveDependencies();

	if (auto* base = GetComponent<BaseComponent>())
	{
		base->name = InvTypesManager::getInstance()->getNameByTypeId(base->typeID);
	}
}

void Equipment::Update(UINT tick)
{
	if (tick % 3 != 0)
		return;

	if (auto* equipment = GetComponent<EquipmentComponent>())
	{
		equipment->Update(tick);
	}
}

void EnergyWeaponEquipments::Init()
{
	Equipment::Init();
	ResolveDependencies();
}

void EnergyWeaponEquipments::handleTask(const Task& task)
{
	try {
		auto TargetObjectId = std::any_cast<int>((*task.paramsPtr)["TargetObjectId"]);
		auto target = GameObjectMgr::getInstance().getObject(TargetObjectId);
		if (target && GetComponent<EquipmentComponent>())
		{
			GetComponent<EquipmentComponent>()->Switch(TargetObjectId);
		}
	}
	catch (const std::bad_any_cast& e) {
		DEBUG_("类型转换错误: {}", e.what());
	}
	DEBUG_(task.to_string());
}

void EnergyWeaponEquipments::Update(UINT tick)
{
	if (tick % 3 != 0)
		return;

	auto* equipment = GetComponent<EquipmentComponent>();
	auto* weapon = GetComponent<WeaponComponent>();
	if (!equipment)
		return;

	equipment->Update(tick);
	if (equipment->m_beginActive && weapon) {
		weapon->applyDamage();
	}
}

void MinerEquipments::Init()
{
	Equipment::Init();
	ResolveDependencies();
}

void MinerEquipments::handleTask(const Task& task)
{
	if (auto* miner = GetComponent<MinerComponent>())
	{
		miner->handleTask(task);
	}
}

void MinerEquipments::Update(UINT tick)
{
	if (tick % 3 != 0)
		return;

	auto* equipment = GetComponent<EquipmentComponent>();
	auto* attributes = GetComponent<AttributesComponent>();
	if (!equipment)
		return;

	equipment->Update(tick);
	if (equipment->m_endActive && attributes) {
		auto containerId = dynGameObjectsManager::getInstance()->getContainerIdByObjectID(objectID);
		auto ownerId = dynContainersManager::getInstance()->getObjectIDByContainerID(containerId);

		std::shared_ptr<Task> task = std::make_shared<Task>();
		task->publisherId = objectID;
		task->targetId = ownerId;
		(*task->paramsPtr)["taskType"] = std::string("cargoStorage");
		(*task->paramsPtr)["storageTaskType"] = std::string("RequestObject");
		(*task->paramsPtr)["volume"] = (*attributes->typeAttributes)[ATTR_ID_MINING_AMOUNT].value;
		(*task->paramsPtr)["RequestTarget"] = equipment->m_targetObjectId;

		TaskMgr::getInstance().addTask(task);

		DEBUG_("(*attributes->typeAttributes)[ATTR_ID_MINING_AMOUNT] : {}", (*attributes->typeAttributes)[ATTR_ID_MINING_AMOUNT].value);
	}
}

void ThrustEquipments::handleTask(const Task& task)
{
	if (auto* equipment = GetComponent<EquipmentComponent>())
	{
		equipment->keep_active = !equipment->keep_active;
	}
}
