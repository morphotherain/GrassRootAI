#include "BaseComponent.h"
#include "Sim/IGameObjectRepository.h"
#include "dynContainersManager.h"

BaseComponent::BaseComponent(UINT _objectID)
{
	auto& repository = GetGameObjectRepository();
	objectID = _objectID;
	auto data = repository.getByObjectID(objectID);
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
	if (!needStore)
	{
		return;
	}
	if (tick % 60 != 0)
	{
		return;
	}
	store();
	needStore = false;
}
void BaseComponent::OnDestroy()
{
	GetGameObjectRepository().removeByObjectID(objectID);
}
void BaseComponent::store()
{
	int tempContainerID = containerID;
	int result_solarSystemID = solarSystemID;

	int resultObjectID = objectID;
	auto& repository = GetGameObjectRepository();
	while (tempContainerID != 0) {
		resultObjectID = dynContainersManager::getInstance()->getObjectIDByContainerID(tempContainerID);
		if (resultObjectID == -1) {
			resultObjectID = objectID;
			break;
		}
		tempContainerID = repository.getContainerIdByObjectID(resultObjectID);
		result_solarSystemID = repository.getSolarSystemIdByObjectID(resultObjectID);
	}

	repository.updateRelatedIds(objectID, result_solarSystemID, ownerID, containerID);
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
