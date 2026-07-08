#pragma once

#include "Sim/AssetLocation.h"
#include "dynGameObjectsManager.h"

class BaseComponent;

class IAssetLocationService {
public:
	virtual ~IAssetLocationService() = default;

	virtual AssetOwner ReadOwner(std::uint32_t objectId) = 0;
	virtual AssetLocation ReadLocation(std::uint32_t objectId) = 0;
	virtual AssetLocation ReadLocationFromDyn(const dynGameObject& row) = 0;

	virtual std::uint32_t ResolveContextSolarSystemId(std::uint32_t objectId) = 0;

	virtual ContainerBagRef ResolveBag(std::uint32_t bagId) = 0;
	virtual std::uint32_t GetOrCreateBag(std::uint32_t hostObjectId, std::uint32_t containerType) = 0;
	virtual std::uint32_t GetHostObjectIdByBag(std::uint32_t bagId) = 0;

	virtual bool MoveToBag(std::uint32_t objectId, std::uint32_t bagId) = 0;
	virtual bool DockAtStructure(std::uint32_t movableObjectId, std::uint32_t structureObjectId) = 0;
	virtual bool UndockToSpace(std::uint32_t movableObjectId) = 0;

	virtual void PersistFromBaseComponent(BaseComponent& base) = 0;
	virtual void ApplyLocationToBase(BaseComponent& base, const AssetLocation& location) = 0;
	virtual void ApplyOwnerToBase(BaseComponent& base, const AssetOwner& owner) = 0;
	virtual void SyncBaseFromDb(BaseComponent& base) = 0;
};

IAssetLocationService& GetAssetLocationService();
