#include "StorageComponent.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"

StorageComponent::StorageComponent(UINT _objectID, UINT _containerTypeID)
{
	objectID = _objectID;
	containerID = dynContainersManager::getInstance()->getContainerID(_objectID, _containerTypeID);
	if (containerID == -1) {
		containerID = dynContainersManager::getInstance()->createContainerRecord(_objectID, _containerTypeID);
	}
	itemIDs = dynGameObjectsManager::getInstance()->getItemsByContainerID(containerID);
}
