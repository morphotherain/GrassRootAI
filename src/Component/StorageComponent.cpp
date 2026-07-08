#include "StorageComponent.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"
#include "GameObject.h"
#include "BaseComponent.h"
#include "Task/TaskParams.h"
#include "Sim/IAssetLocationService.h"

StorageComponent::StorageComponent(UINT _objectID, UINT _containerTypeID)
{
	initBaseTaskHandlers();
	initTaskHandlers();
	objectID = _objectID;
	bagId = GetAssetLocationService().GetOrCreateBag(_objectID, _containerTypeID);
	itemIDs = dynGameObjectsManager::getInstance()->getItemsByBagId(bagId);
}

void StorageComponent::handleTask(const Task& task)
{
	taskRegistry.Handle(task);
}

void StorageComponent::OnDestroy()
{
	dynContainersManager::getInstance()->removeContainerByID(bagId);
}

void StorageComponent::Refresh()
{
	itemIDs = dynGameObjectsManager::getInstance()->getItemsByBagId(bagId);
}

void StorageComponent::initBaseTaskHandlers()
{
	taskRegistry.setTypeKey("storageTaskType");
	taskRegistry.Register("AddObject", TASK_HANDLER{
	}
	).Register("WithDrawObject", TASK_HANDLER{
	}
	).Register("Refresh", TASK_HANDLER{
		itemIDs = dynGameObjectsManager::getInstance()->getItemsByBagId(bagId);
		}
	).Register("RequestObject", TASK_HANDLER{
		const auto requestParams = TryReadCargoStorageRequestParams(task);
		if (!requestParams.has_value())
			return;
		const auto RequesTargetId = requestParams->requestTarget;
		const auto volume = requestParams->volume;

		auto batch = TaskMgr::getInstance().createBatch();
		auto target = GameObjectMgr::getInstance().getObject(RequesTargetId);
		if (target) {
			auto base = target->GetComponent<BaseComponent>();
			for (auto id : itemIDs) {
				auto item = GameObjectMgr::getInstance().getObject(id);
				if (!item)continue;
				auto itemBase = item->GetComponent<BaseComponent>();
				if (itemBase->typeID == base->typeID)
				{
					batch.add(TaskFactory::MakeAddObjectTask(
						objectID,
						RequesTargetId,
						AddObjectParams{ "add", static_cast<int>(id), static_cast<int>(bagId), volume }));
					return;
				}
			}
		}

		batch.add(TaskFactory::MakeAddObjectTask(
			objectID,
			RequesTargetId,
			AddObjectParams{ "create", static_cast<int>(objectID), static_cast<int>(bagId), volume }));
		batch.add(TaskFactory::MakeCargoStorageRefreshTask(objectID));
	}
	);

}
