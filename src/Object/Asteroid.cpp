#include "Asteroid.h"
#include "mapJumpsManager.h"
#include "mapDenormalizeManager.h"
#include "mapSolarSystemsManager.h"

Asteroid::~Asteroid()
{
}

void Asteroid::Init()
{
	m_pBase = GetComponentShared<BaseComponent>();
	m_pAttributes = GetComponentShared<AttributesComponent>();
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	if (m_pSpaceTran->x == 0.0f && m_pSpaceTran->y == 0.0f && m_pSpaceTran->z == 0.0f)
	{
		m_pSpaceTran.reset();
	}
	else
	{
		AddComponent<Component>(m_pSpaceTran);
	}
	if (m_pBase)
	{
		m_pBase->name = InvTypesManager::getInstance()->getNameByTypeId(m_pBase->typeID);
	}
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
	auto type = task.getParamOrDefault<std::string>("taskType", "");
	if (type == "addObject") {
		auto addType = task.getParamOrDefault<std::string>("addType", "");

		auto addTargetId = task.getParamOrDefault<int>("addTargetId", -1);
		auto containerID = task.getParamOrDefault<int>("containerID", -1);
		auto volume = task.getParamOrDefault<double>("volume", 0);
		auto volumePerUnit = (*m_pAttributes->objectAttributes)[ATTR_ID_VOLUME].value;

		if(addType == "add") {
			auto addTarget = GameObjectMgr::getInstance().getObject(addTargetId);
			if (addTarget) {
				auto attr = addTarget->GetComponent<AttributesComponent>();
				double itemVolume = attr->getAttrValueById(ATTR_ID_QUANTITY, 0.0);
				itemVolume += std::floor(volume / volumePerUnit);
				attr->setAttrValueById(ATTR_ID_QUANTITY, itemVolume);
				attr->storeAttributes();
			}
		}
		if (addType == "create") {
			std::shared_ptr<Task> pTask = std::make_shared<Task>();
			pTask->isInnerTask = true;
			pTask->publisherId = objectID;
			pTask->targetSystem = SOLAR_SYSTEM;
			(*pTask->paramsPtr)["createObject"] = 0;
			(*pTask->paramsPtr)["handlerType"] = std::string("createObject");
			(*pTask->paramsPtr)["typeID"] = static_cast<int>(m_pBase->typeID);
			(*pTask->paramsPtr)["OwnerID"] = 0;
			(*pTask->paramsPtr)["ContainerID"] = static_cast<int>(containerID);
			std::vector<Attribute> attributes = {
				{ATTR_ID_QUANTITY, std::floor(volume / volumePerUnit) },
			};
			(*pTask->paramsPtr)["attributes"] = attributes;

			TaskMgr::getInstance().addTask(pTask);
		}
	}

}
