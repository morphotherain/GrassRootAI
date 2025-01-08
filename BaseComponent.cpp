#include "BaseComponent.h"
#include "dynGameObjectsManager.h"

BaseComponent::BaseComponent(UINT _objectID)
{
	objectID = _objectID;
	typeID = dynGameObjectsManager::getInstance()->getTypeIdByObjectID(_objectID);
	solarSystemID = dynGameObjectsManager::getInstance()->getSolarSystemIdByObjectID(_objectID);
	containerID  = dynGameObjectsManager::getInstance()->getContainerIdByObjectID(_objectID);
	ownerID = dynGameObjectsManager::getInstance()->getOwnerIdByObjectID(_objectID);

}
void BaseComponent::store()
{
	// 通过单例模式获取dynGameObjectsManager实例，调用对应的更新函数来更新位置和四元数信息
	dynGameObjectsManager* manager = dynGameObjectsManager::getInstance();
	if (manager != nullptr)
	{
		manager->updateRelatedIdsByObjectID(objectID, solarSystemID, ownerID, containerID);
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

