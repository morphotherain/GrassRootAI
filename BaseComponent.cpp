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
