#include "Sim/IAssetLocationService.h"
#include "Sim/IGameObjectRepository.h"
#include "BaseComponent.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"
#include "SimLog.h"

namespace
{
	class AssetLocationService final : public IAssetLocationService {
		bool IsValidBagId(std::uint32_t bagId) const
		{
			if (bagId == 0)
				return false;
			return dynContainersManager::getInstance()->getObjectIDByContainerID(bagId) != -1;
		}

		void WritePlacement(
			std::uint32_t objectId,
			std::uint32_t solarSystemId,
			const AssetOwner& owner,
			const AssetLocation& location)
		{
			GetGameObjectRepository().updatePlacement(
				objectId,
				solarSystemId,
				owner.kind,
				owner.DynOwnerId(),
				location.kind,
				location.locationRef);
		}

	public:
		AssetOwner ReadOwner(std::uint32_t objectId) override
		{
			return GetGameObjectRepository().getByObjectID(objectId).GetOwner();
		}

		AssetLocation ReadLocation(std::uint32_t objectId) override
		{
			return ReadLocationFromDyn(GetGameObjectRepository().getByObjectID(objectId));
		}

		AssetLocation ReadLocationFromDyn(const dynGameObject& row) override
		{
			return row.GetLocation();
		}

		std::uint32_t ResolveContextSolarSystemId(std::uint32_t objectId) override
		{
			const auto row = GetGameObjectRepository().getByObjectID(objectId);
			if (row.objectID == 0)
				return 0;

			const auto kind = static_cast<AssetLocationKind>(row.LocationKind);
			if (kind == AssetLocationKind::SolarSystem || kind == AssetLocationKind::DockedAtStructure)
				return row.SolarSystemID;

			if (kind == AssetLocationKind::ContainerBag) {
				const int hostObjectId = dynContainersManager::getInstance()->getObjectIDByContainerID(row.LocationRef);
				if (hostObjectId == -1)
					return row.SolarSystemID;
				return ResolveContextSolarSystemId(static_cast<std::uint32_t>(hostObjectId));
			}

			return row.SolarSystemID;
		}

		ContainerBagRef ResolveBag(std::uint32_t bagId) override
		{
			ContainerBagRef ref;
			ref.bagId = bagId;
			ref.hostObjectId = GetHostObjectIdByBag(bagId);
			return ref;
		}

		std::uint32_t GetOrCreateBag(std::uint32_t hostObjectId, std::uint32_t containerType) override
		{
			auto bagId = dynContainersManager::getInstance()->getContainerID(
				static_cast<int>(hostObjectId), static_cast<int>(containerType));
			if (bagId == static_cast<std::uint32_t>(-1)) {
				dynContainersManager::getInstance()->createContainerRecord(
					static_cast<int>(hostObjectId), static_cast<int>(containerType));
				bagId = dynContainersManager::getInstance()->getContainerID(
					static_cast<int>(hostObjectId), static_cast<int>(containerType));
			}
			return bagId;
		}

		std::uint32_t GetHostObjectIdByBag(std::uint32_t bagId) override
		{
			return static_cast<std::uint32_t>(
				dynContainersManager::getInstance()->getObjectIDByContainerID(bagId));
		}

		bool MoveToBag(std::uint32_t objectId, std::uint32_t bagId) override
		{
			if (!IsValidBagId(bagId)) {
				LOG_SIM("MoveToBag 失败 objectId={} bagId={}（无效 bag）", objectId, bagId);
				return false;
			}
			const auto owner = ReadOwner(objectId);
			const auto location = AssetLocation::InBag(bagId, ResolveContextSolarSystemId(objectId));
			WritePlacement(objectId, location.solarSystemId, owner, location);
			return true;
		}

		bool DockAtStructure(std::uint32_t movableObjectId, std::uint32_t structureObjectId) override
		{
			const auto structureRow = GetGameObjectRepository().getByObjectID(structureObjectId);
			if (structureRow.objectID == 0) {
				LOG_SIM("DockAtStructure 失败 structureObjectId={} 不存在", structureObjectId);
				return false;
			}
			const auto owner = ReadOwner(movableObjectId);
			const auto location = AssetLocation::DockedAt(structureObjectId, structureRow.SolarSystemID);
			WritePlacement(movableObjectId, location.solarSystemId, owner, location);
			return true;
		}

		bool UndockToSpace(std::uint32_t movableObjectId) override
		{
			const auto contextSolarSystemId = ResolveContextSolarSystemId(movableObjectId);
			const auto owner = ReadOwner(movableObjectId);
			const auto location = AssetLocation::InSolarSystem(contextSolarSystemId);
			WritePlacement(movableObjectId, contextSolarSystemId, owner, location);
			return true;
		}

		void PersistFromBaseComponent(BaseComponent& base) override
		{
			const auto resolvedSolarSystemId = ResolveContextSolarSystemId(base.objectID);
			const auto owner = base.GetAssetOwner();
			const auto location = base.GetAssetLocation();
			auto persistedLocation = location;
			persistedLocation.solarSystemId = resolvedSolarSystemId;
			WritePlacement(base.objectID, resolvedSolarSystemId, owner, persistedLocation);
			base.solarSystemID = resolvedSolarSystemId;
		}

		void ApplyLocationToBase(BaseComponent& base, const AssetLocation& location) override
		{
			base.locationKind = location.DynLocationKind();
			base.locationRef = location.locationRef;
			base.solarSystemID = location.solarSystemId;
		}

		void ApplyOwnerToBase(BaseComponent& base, const AssetOwner& owner) override
		{
			base.ownerKind = owner.DynOwnerKind();
			base.ownerID = owner.DynOwnerId();
		}

		void SyncBaseFromDb(BaseComponent& base) override
		{
			const auto row = GetGameObjectRepository().getByObjectID(base.objectID);
			ApplyOwnerToBase(base, row.GetOwner());
			ApplyLocationToBase(base, row.GetLocation());
		}
	};
}

IAssetLocationService& GetAssetLocationService()
{
	static AssetLocationService service;
	return service;
}
