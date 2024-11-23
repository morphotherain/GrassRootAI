#include "SpaceTransformComponent.h"
#include "dynGameObjectsManager.h"
#include "mapDenormalizeManager.h"

SpaceTransformComponent::SpaceTransformComponent(UINT _objectID)
{
	orientation = { 0.0f, 0.0f, 0.0f, 0.0f };
	if(_objectID<40000000)
	{
		auto pos = dynGameObjectsManager::getInstance()->getPosByObjectID(_objectID);
		auto q = dynGameObjectsManager::getInstance()->getQuaternionByObjectID(_objectID);
		x = pos[0];
		y = pos[1];
		z = pos[2];
		orientation.w = static_cast<float>(q[0]);
		orientation.x = static_cast<float>(q[1]);
		orientation.y = static_cast<float>(q[2]);
		orientation.z = static_cast<float>(q[3]);
		objectID = _objectID;
	}
	else {
		auto pos = mapDenormalizeManager::getInstance()->getPosByObjectID(_objectID);
		x = pos[0];
		y = pos[1];
		z = pos[2];
		objectID = _objectID;
	}

}
