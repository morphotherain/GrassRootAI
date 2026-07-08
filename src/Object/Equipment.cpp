#include "Equipment.h"
#include "BaseComponent.h"
#include "Sim/AssetLocation.h"
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
#include "Task/TaskParams.h"

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
	const auto params = ReadTargetObjectIdParams(task);
	auto target = GameObjectMgr::getInstance().getObject(params.targetObjectId);
	if (target && GetComponent<EquipmentComponent>())
	{
		GetComponent<EquipmentComponent>()->Switch(params.targetObjectId);
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
		const auto* base = GetComponent<BaseComponent>();
		int shipObjectId = objectID;
		if (base && base->locationKind == static_cast<UINT>(AssetLocationKind::ContainerBag)) {
			shipObjectId = dynContainersManager::getInstance()->getObjectIDByContainerID(base->locationRef);
		}

		std::shared_ptr<Task> task = TaskFactory::MakeCargoStorageRequestTask(
			objectID,
			shipObjectId,
			CargoStorageRequestParams{
				equipment->m_targetObjectId,
				(*attributes->typeAttributes)[ATTR_ID_MINING_AMOUNT].value });

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
