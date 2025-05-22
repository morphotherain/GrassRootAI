#include "Equipment.h"
#include "mapJumpsManager.h"
#include "mapDenormalizeManager.h"
#include "mapSolarSystemsManager.h"
#include "dynContainersManager.h"



std::shared_ptr<GameObject> Equipment::ConvertBasedOnGroupID(UINT groupID)
{
	switch (groupID){
	case 46: {
		return std::make_shared<ThrustEquipments>(objectID);        //推进改良设备
		break;
	}
	case 53: {
		return std::make_shared<EnergyWeaponEquipments>(objectID);        //能量武器
		break;
	}
	case 54: {
		return std::make_shared<MinerEquipments>(objectID);        //采矿激光器
		break;
	}
	case 546: {
		return std::make_shared<MiningImprovementEquipments>(objectID);        //采矿改良设备
		break;
	}
	
	}

	return nullptr;
}

void Equipment::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pAttributes = std::make_shared<AttributesComponent>(objectID);
	AddComponent<Component>(m_pAttributes);
	m_pEquipment = std::make_shared<EquipmentComponent>(objectID);
	AddComponent<Component>(m_pEquipment);

	ResolveDependencies();

	m_pBase->name = InvTypesManager::getInstance()->getNameByTypeId(m_pBase->typeID);
}

void Equipment::Update(UINT tick)
{

	if (tick % 3 != 0)
		return;

	m_pEquipment->Update(tick);
}

void EnergyWeaponEquipments::Init()
{
	Equipment::Init();

	m_pWeapon = std::make_shared<WeaponComponent>(objectID);
	AddComponent<Component>(m_pWeapon);

	ResolveDependencies();
}

void EnergyWeaponEquipments::handleTask(const Task& task)
{
	try {
		auto TargetObjectId = std::any_cast<int>((*task.paramsPtr)["TargetObjectId"]);

		auto target = GameObjectMgr::getInstance().getObject(TargetObjectId);
		if (target) {
			m_pEquipment->Switch(TargetObjectId);
		}
	}
	catch (const std::bad_any_cast& e) {
		// 记录日志或进行错误处理
		DEBUG_("类型转换错误: {}", e.what());
	}
	DEBUG_(task.to_string());
}

void EnergyWeaponEquipments::Update(UINT tick)
{
	if (tick % 3 != 0)
		return;

	m_pEquipment->Update(tick);
	if (m_pEquipment->m_beginActive) {
		m_pWeapon->applyDamage();
		return;
	}
}

void MinerEquipments::Init()
{
	Equipment::Init();

	m_pMiner = std::make_shared<MinerComponent>(objectID);
	AddComponent<Component>(m_pMiner);

	ResolveDependencies();
}

void MinerEquipments::handleTask(const Task& task)
{
	m_pMiner->handleTask(task);
}

void MinerEquipments::Update(UINT tick)
{
	if (tick % 3 != 0)
		return;

	m_pEquipment->Update(tick);
	if (m_pEquipment->m_endActive) {
		auto containerId = dynGameObjectsManager::getInstance()->getContainerIdByObjectID(objectID);
		auto ownerId = dynContainersManager::getInstance()->getObjectIDByContainerID(containerId);

		std::shared_ptr<Task> task = std::make_shared<Task>();
		task->isInnerTask = true;
		task->taskID = 0;
		task->publisherId = objectID;
		task->targetId = ownerId;
		task->taskTypeId = 7;
		(*task->paramsPtr)["volume"] = (*m_pAttributes->typeAttributes)[ATTR_ID_MINING_AMOUNT].value;
		(*task->paramsPtr)["RequestTarget"] = m_pEquipment->m_targetObjectId;

		TaskMgr::getInstance().addTask(task);

		DEBUG_("(*m_pAttributes->typeAttributes)[ATTR_ID_MINING_AMOUNT] : {}", (*m_pAttributes->typeAttributes)[ATTR_ID_MINING_AMOUNT].value);
	}
}

void ThrustEquipments::handleTask(const Task& task)
{
	m_pEquipment->keep_active = !m_pEquipment->keep_active;
}
