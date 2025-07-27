#include "StorageComponent.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"
#include "GameObject.h"
#include "BaseComponent.h"


StorageComponent::StorageComponent(UINT _objectID, UINT _containerTypeID)
{
	initBaseTaskHandlers();
	initTaskHandlers();
	objectID = _objectID;
	containerID = dynContainersManager::getInstance()->getContainerID(_objectID, _containerTypeID);
	if (containerID == -1) {
		containerID = dynContainersManager::getInstance()->createContainerRecord(_objectID, _containerTypeID);
	}
	itemIDs = dynGameObjectsManager::getInstance()->getItemsByContainerID(containerID);
}

void StorageComponent::handleTask(const Task& task)
{
	taskRegistry.Handle(task);
}

void StorageComponent::OnDestroy()
{
	dynContainersManager::getInstance()->removeContainerByID(containerID);
}

void StorageComponent::Refresh()
{
	itemIDs = dynGameObjectsManager::getInstance()->getItemsByContainerID(containerID);
}

void StorageComponent::initBaseTaskHandlers()
{
	taskRegistry.setTypeKey("storageTaskType");
	taskRegistry.Register("AddObject", TASK_HANDLER{
	}
	).Register("WithDrawObject", TASK_HANDLER{
	}
	).Register("Refresh", TASK_HANDLER{
		itemIDs = dynGameObjectsManager::getInstance()->getItemsByContainerID(containerID);
		}
	).Register("RequestObject", TASK_HANDLER{
		auto batch = TaskMgr::getInstance().createBatch();
		auto RequesTargetId = std::any_cast<int>((*task.paramsPtr)["RequestTarget"]);
		auto volume = std::any_cast<double>((*task.paramsPtr)["volume"]);

		auto target = GameObjectMgr::getInstance().getObject(RequesTargetId);
		if (target) {
			auto base = target->GetComponent<BaseComponent>();
			for (auto id : itemIDs) {
				auto item = GameObjectMgr::getInstance().getObject(id);
				if (!item)continue;
				auto itemBase = item->GetComponent<BaseComponent>();
				if (itemBase->typeID == base->typeID)
				{
					std::shared_ptr<Task> pTask = std::make_shared<Task>();
					pTask->publisherId = objectID;
					pTask->targetId = RequesTargetId;
					(*pTask->paramsPtr)["volume"] = volume;
					(*pTask->paramsPtr)["taskType"] = std::string("addObject");
					(*pTask->paramsPtr)["addType"] = std::string("add");
					(*pTask->paramsPtr)["addTargetId"] = static_cast<int>(id);
					(*pTask->paramsPtr)["containerID"] = static_cast<int>(containerID);

					batch.add(pTask);
					return;
				}
			}
		}

		std::shared_ptr<Task> pTask = std::make_shared<Task>();
		pTask->publisherId = objectID;
		pTask->targetId = RequesTargetId;
		(*pTask->paramsPtr)["volume"] = volume;
		(*pTask->paramsPtr)["taskType"] = std::string("addObject");
		(*pTask->paramsPtr)["addType"] = std::string("create");
		(*pTask->paramsPtr)["addTargetId"] = static_cast<int>(objectID);
		(*pTask->paramsPtr)["containerID"] = static_cast<int>(containerID);
		batch.add(pTask);

		std::shared_ptr<Task> refreshTask = std::make_shared<Task>();
		refreshTask->publisherId = objectID;
		refreshTask->targetId = objectID;
		(*refreshTask->paramsPtr)["taskType"] = std::string("cargoStorage");
		(*refreshTask->paramsPtr)["storageTaskType"] = std::string("Refresh");
		batch.add(refreshTask);
	}
	);

}
