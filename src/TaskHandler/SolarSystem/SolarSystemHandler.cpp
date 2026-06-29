#include "SolarSystemHandler.h"
#include "SolarSystemMgr.h"
#include "dynGameObjectsManager.h"
#include "AttributesComponent.h"
#include "BaseComponent.h"
#include "GameObject.h"
#include "dynContainersManager.h"

bool CreateObjectHandler::handleTask(Task& task)
{
	try {
		dynGameObject objectData;
		objectData.x = task.getParamOrDefault<double>("x", 0.0);
		objectData.y = task.getParamOrDefault("y", 0.0);
		objectData.z = task.getParamOrDefault("z", 0.0);
		objectData.typeID = task.getParamOrDefault("typeID", 0);
		objectData.SolarSystemID = task.getParamOrDefault("SolarSystemID", 0);
		objectData.OwnerID = task.getParamOrDefault("OwnerID", 0);
		objectData.ContainerID = task.getParamOrDefault("ContainerID", 0);
		objectData.qw = task.getParamOrDefault("qw", 0.0);
		objectData.qx = task.getParamOrDefault("qx", 0.0);
		objectData.qy = task.getParamOrDefault("qy", 0.0);
		objectData.qz = task.getParamOrDefault("qz", 0.0);
		if (objectData.typeID == 0)return false;

		auto id = dynGameObjectsManager::getInstance()->insertGameObject(objectData);
		if (task.paramsPtr->find("attributes") != task.paramsPtr->end()) {
			try {
				auto attributeVec = std::any_cast<std::vector<Attribute>>((*task.paramsPtr)["attributes"]);
				auto attributeComp = SolarSystemMgr::getInstance().getObjectById(id)->GetComponent<AttributesComponent>();
				if (attributeComp != nullptr) {
					attributeComp->UpdateAttributes(attributeVec);
					attributeComp->storeAttributes();
				}
			}
			catch (const std::bad_any_cast& e) {
				DEBUG_("类型转换错误: {}", e.what());
			}
		}
		return true;
	}
	catch (const std::bad_any_cast& e) {
		DEBUG_("类型转换错误: {}", e.what());
	}
	return false;
}

bool TransferObjectHandler::handleTask(Task& task)
{
	try {
		auto targetId = task.targetId;
		if (task.target.lock() != nullptr)
			targetId = task.target.lock()->GetComponent<BaseComponent>()->objectID;
		auto containerId = task.getParamOrDefault("ContainerID", 0);
		dynGameObjectsManager::getInstance()->updateContainerIDByObjectID(targetId, containerId);
		return true;
	}
	catch (const std::bad_any_cast& e) {
		DEBUG_("类型转换错误: {}", e.what());
	}
	return false;
}

bool DestroyObjectHandler::handleTask(Task& task)
{
	try {
		auto target = task.target.lock();
		if (!target)
			return false;
		auto id = target->GetComponent<BaseComponent>()->objectID;
		target->Destroy();
		SolarSystemMgr::getInstance().p_mapObject->erase(id);
		return true;
	}
	catch (const std::bad_any_cast& e) {
		DEBUG_("类型转换错误: {}", e.what());
	}
	return false;
}
