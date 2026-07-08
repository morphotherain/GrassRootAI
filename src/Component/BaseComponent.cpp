#include "BaseComponent.h"
#include "Sim/IGameObjectRepository.h"
#include "Sim/IAssetLocationService.h"

BaseComponent::BaseComponent(UINT _objectID)
{
	auto& repository = GetGameObjectRepository();
	objectID = _objectID;
	auto data = repository.getByObjectID(objectID);
	typeID = data.typeID;
	solarSystemID = data.SolarSystemID;
	ownerKind = data.OwnerKind;
	ownerID = data.OwnerID;
	locationKind = data.LocationKind;
	locationRef = data.LocationRef;
	groupID = data.groupID;
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
	GetAssetLocationService().PersistFromBaseComponent(*this);
}

void BaseComponent::setSolarSystemID(UINT _solarSystemID)
{
	solarSystemID = _solarSystemID;
	locationKind = static_cast<UINT>(AssetLocationKind::SolarSystem);
	locationRef = 0;
	store();
}

AssetOwner BaseComponent::GetAssetOwner() const
{
	return AssetOwner::FromDyn(ownerKind, ownerID);
}

AssetLocation BaseComponent::GetAssetLocation() const
{
	return AssetLocation::FromDyn(locationKind, locationRef, solarSystemID);
}

void BaseComponent::SetAssetOwner(const AssetOwner& owner)
{
	GetAssetLocationService().ApplyOwnerToBase(*this, owner);
	store();
}

void BaseComponent::SetAssetLocation(const AssetLocation& location)
{
	GetAssetLocationService().ApplyLocationToBase(*this, location);
	store();
}
