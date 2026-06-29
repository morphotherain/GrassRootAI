#include "Asteroid.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "InvTypesManager.h"
#include "TaskMgr.h"

Asteroid::~Asteroid()
{
}

void Asteroid::Init()
{
	auto spaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	if (spaceTran->x == 0.0f && spaceTran->y == 0.0f && spaceTran->z == 0.0f)
	{
		spaceTran.reset();
	}
	else
	{
		AddComponent<Component>(spaceTran);
	}

	if (auto* base = GetComponent<BaseComponent>())
	{
		base->name = InvTypesManager::getInstance()->getNameByTypeId(base->typeID);
	}
}

std::shared_ptr<GameObject> Asteroid::ConvertBasedOnGroupID(UINT groupID)
{
	switch (groupID) {
	case 462: return std::make_shared<Veldspar>(objectID);
	}
	return nullptr;
}

void Asteroid::handleTask(const Task& task)
{
	auto* attributes = GetComponent<AttributesComponent>();
	auto* base = GetComponent<BaseComponent>();
	if (!attributes || !base)
		return;

	auto type = task.getParamOrDefault<std::string>("taskType", "");
	if (type == "addObject") {
		auto addType = task.getParamOrDefault<std::string>("addType", "");
		auto addTargetId = task.getParamOrDefault<int>("addTargetId", -1);
		auto containerID = task.getParamOrDefault<int>("containerID", -1);
		auto volume = task.getParamOrDefault<double>("volume", 0);
		auto volumePerUnit = (*attributes->objectAttributes)[ATTR_ID_VOLUME].value;

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
			(*pTask->paramsPtr)["typeID"] = static_cast<int>(base->typeID);
			(*pTask->paramsPtr)["OwnerID"] = 0;
			(*pTask->paramsPtr)["ContainerID"] = static_cast<int>(containerID);
			std::vector<Attribute> attributesVec = {
				{ATTR_ID_QUANTITY, std::floor(volume / volumePerUnit) },
			};
			(*pTask->paramsPtr)["attributes"] = attributesVec;

			TaskMgr::getInstance().addTask(pTask);
		}
	}
}
