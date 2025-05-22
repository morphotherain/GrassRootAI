#include "BaseComponent.h"
#include "dynGameObjectsManager.h"
#include "dynContainersManager.h"

BaseComponent::BaseComponent(UINT _objectID)
{
	objectID = _objectID;
	auto data = dynGameObjectsManager::getInstance()->getGameObjectByObjectID(objectID);
	typeID = data.typeID;
	solarSystemID = data.SolarSystemID;
	ownerID = data.OwnerID;
	groupID = data.groupID;
	containerID = data.ContainerID;
	categoryID = data.categoryID;
	name = data.name;
}
BaseComponent::~BaseComponent()
{
	store();
}
void BaseComponent::Update(UINT tick)
{
	store();
}
void BaseComponent::OnDestroy()
{
	dynGameObjectsManager::getInstance()->removeObjectByObjectID(objectID);
}
void BaseComponent::store()
{
	if (objectID == 5) {
		objectID = 5;
	}
	// 通过单例模式获取dynGameObjectsManager实例，调用对应的更新函数来更新位置和四元数信息
	int tempContainerID = containerID;
	int result_solarSystemID = solarSystemID;

	int resultObjectID = objectID;
	while (tempContainerID != 0) {
		resultObjectID = dynContainersManager::getInstance()->getObjectIDByContainerID(tempContainerID);
		if (resultObjectID == -1) {
			resultObjectID = objectID;
			break;
		}
		tempContainerID = dynGameObjectsManager::getInstance()->getContainerIdByObjectID(resultObjectID);
		result_solarSystemID = dynGameObjectsManager::getInstance()->getSolarSystemIdByObjectID(resultObjectID);
	}

	dynGameObjectsManager* manager = dynGameObjectsManager::getInstance();
	if (manager != nullptr)
	{
		manager->updateRelatedIdsByObjectID(objectID, result_solarSystemID, ownerID, containerID);
	}
}

void BaseComponent::setSolarSystemID(UINT _solarSystemID)
{
	solarSystemID = _solarSystemID;
	store();
}

void BaseComponent::setContainerID(UINT _containerID)
{
	containerID = _containerID;
	store();
}