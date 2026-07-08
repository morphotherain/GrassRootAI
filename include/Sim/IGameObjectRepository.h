#pragma once

#include "dynGameObjectsManager.h"
#include "Sim/AssetLocation.h"

class IGameObjectRepository {
public:
	virtual ~IGameObjectRepository() = default;

	virtual dynGameObject getByObjectID(UINT objectID) = 0;
	virtual void updatePlacement(
		UINT objectID,
		UINT solarSystemID,
		AssetOwnerKind ownerKind,
		UINT ownerID,
		AssetLocationKind locationKind,
		UINT locationRef) = 0;
	virtual void removeByObjectID(UINT objectID) = 0;
	virtual UINT getSolarSystemIdByObjectID(UINT objectID) = 0;
	virtual dynGameObject getByObjectIDOrEmpty(UINT objectID) = 0;
};

IGameObjectRepository& GetGameObjectRepository();
