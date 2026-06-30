#pragma once

#include "dynGameObjectsManager.h"

struct dynGameObject;

class IGameObjectRepository {
public:
	virtual ~IGameObjectRepository() = default;

	virtual dynGameObject getByObjectID(UINT objectID) = 0;
	virtual void updateRelatedIds(UINT objectID, UINT solarSystemID, UINT ownerID, UINT containerID) = 0;
	virtual void removeByObjectID(UINT objectID) = 0;
	virtual UINT getSolarSystemIdByObjectID(UINT objectID) = 0;
	virtual UINT getContainerIdByObjectID(UINT objectID) = 0;
};

IGameObjectRepository& GetGameObjectRepository();
