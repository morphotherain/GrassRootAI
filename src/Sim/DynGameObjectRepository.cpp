#include "Sim/IGameObjectRepository.h"

#include "dynGameObjectsManager.h"

namespace
{
	class DynGameObjectRepository final : public IGameObjectRepository {
	public:
		dynGameObject getByObjectID(UINT objectID) override
		{
			return dynGameObjectsManager::getInstance()->getGameObjectByObjectID(objectID);
		}

		dynGameObject getByObjectIDOrEmpty(UINT objectID) override
		{
			return getByObjectID(objectID);
		}

		void updatePlacement(
			UINT objectID,
			UINT solarSystemID,
			AssetOwnerKind ownerKind,
			UINT ownerID,
			AssetLocationKind locationKind,
			UINT locationRef) override
		{
			dynGameObjectsManager::getInstance()->updatePlacementByObjectID(
				static_cast<int>(objectID),
				solarSystemID,
				static_cast<unsigned int>(ownerKind),
				ownerID,
				static_cast<unsigned int>(locationKind),
				locationRef);
		}

		void removeByObjectID(UINT objectID) override
		{
			dynGameObjectsManager::getInstance()->removeObjectByObjectID(objectID);
		}

		UINT getSolarSystemIdByObjectID(UINT objectID) override
		{
			return dynGameObjectsManager::getInstance()->getSolarSystemIdByObjectID(objectID);
		}
	};
}

IGameObjectRepository& GetGameObjectRepository()
{
	static DynGameObjectRepository repository;
	return repository;
}
