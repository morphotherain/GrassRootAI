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

		void updateRelatedIds(UINT objectID, UINT solarSystemID, UINT ownerID, UINT containerID) override
		{
			dynGameObjectsManager::getInstance()->updateRelatedIdsByObjectID(
				objectID, solarSystemID, ownerID, containerID);
		}

		void removeByObjectID(UINT objectID) override
		{
			dynGameObjectsManager::getInstance()->removeObjectByObjectID(objectID);
		}

		UINT getSolarSystemIdByObjectID(UINT objectID) override
		{
			return dynGameObjectsManager::getInstance()->getSolarSystemIdByObjectID(objectID);
		}

		UINT getContainerIdByObjectID(UINT objectID) override
		{
			return dynGameObjectsManager::getInstance()->getContainerIdByObjectID(objectID);
		}
	};
}

IGameObjectRepository& GetGameObjectRepository()
{
	static DynGameObjectRepository repository;
	return repository;
}
