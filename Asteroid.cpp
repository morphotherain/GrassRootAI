#include "Asteroid.h"
#include "mapJumpsManager.h"
#include "mapDenormalizeManager.h"
#include "mapSolarSystemsManager.h"

void Asteroid::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pAttributes = std::make_shared<AttributesComponent>(objectID);
	AddComponent<Component>(m_pAttributes);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	if (m_pSpaceTran->x == 0.0f && m_pSpaceTran->y == 0.0f && m_pSpaceTran->z == 0.0f){
		m_pSpaceTran.reset();
	}
	else {
		AddComponent<Component>(m_pSpaceTran);
	}
	m_pBase->name = InvTypesManager::getInstance()->getNameByTypeId(m_pBase->typeID);
}

std::shared_ptr<GameObject> Asteroid::ConvertBasedOnGroupID(UINT groupID)
{
	switch (groupID) {
	case 462: {
		return std::make_shared<Veldspar>(objectID);
	}
	}
	return nullptr;
}

void Asteroid::handleTask(const Task& task)
{
	switch (task.taskTypeId)
	{
	case 0:    //Add Object
	{
		try {
			auto addTargetId = std::any_cast<int>((*task.paramsPtr)["addTargetId"]);
			auto containerID = std::any_cast<int>((*task.paramsPtr)["containerID"]);
			auto volume = std::any_cast<double>((*task.paramsPtr)["volume"]);
			auto volumePerUnit = (*m_pAttributes->objectAttributes)[ATTR_ID_VOLUME].value;
			std::shared_ptr<Task> pTask = std::make_shared<Task>();
			pTask->isInnerTask = true;
			pTask->publisherId = objectID;
			(*pTask->paramsPtr)["ToSolarSystemMgr"] = 0;
			(*pTask->paramsPtr)["createObject"] = 0;
			(*pTask->paramsPtr)["typeID"] = static_cast<int>(m_pBase->typeID);
			(*pTask->paramsPtr)["OwnerID"] = 0;
			(*pTask->paramsPtr)["ContainerID"] = static_cast<int>(containerID);
			std::vector<Attribute> attributes = {
				{ATTR_ID_QUANTITY, std::floor(volume / volumePerUnit) },
			};
			(*pTask->paramsPtr)["attributes"] = attributes;

			TaskMgr::getInstance().addTask(pTask);
		}
		catch (const std::bad_any_cast& e) {
			// 记录日志或进行错误处理
			DEBUG_("类型转换错误: {}", e.what());
		}
		break;
	}
	
	default:
		break;
	}
}
