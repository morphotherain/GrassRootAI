#include "SpaceTransformComponent.h"
#include "dynGameObjectsManager.h"
#include "mapDenormalizeManager.h"

SpaceTransformComponent::SpaceTransformComponent(UINT _objectID)
{
	if(_objectID<40000000)
	{
		auto pos = dynGameObjectsManager::getInstance()->getPosByObjectID(_objectID);
		auto q = dynGameObjectsManager::getInstance()->getQuaternionByObjectID(_objectID);
		x = pos[0];
		y = pos[1];
		z = pos[2];
		orientation.w = q[0];
		orientation.x = q[1];
		orientation.y = q[2];
		orientation.z = q[3];
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
