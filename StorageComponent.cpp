#include "StorageComponent.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"

StorageComponent::StorageComponent(UINT _objectID, UINT _containerTypeID)
{
	objectID = _objectID;
	containerID = dynContainersManager::getInstance()->getContainerID(_objectID, _containerTypeID);
	if (containerID == -1) {
		containerID = dynContainersManager::getInstance()->createContainerRecord(_objectID, _containerTypeID);
	}
	itemIDs = dynGameObjectsManager::getInstance()->getItemsByContainerID(containerID);
}

void StorageComponent::handleTask(const Task& task)
{
	switch (task.taskTypeId)
	{
	case 0:    //Add Object
	{
		break;
	}
	case 1:    //WithDraw Object
	{
		break;
	}
	case 2:   //Create Object
	{
		

		break;
	}
	case 3:
	{
		break;
	}
	case 7:    //Request Object
	{
		try {
			auto RequesTargetId = std::any_cast<int>((*task.paramsPtr)["RequestTarget"]);
			auto volume = std::any_cast<double>((*task.paramsPtr)["volume"]);

			std::shared_ptr<Task> pTask = std::make_shared<Task>();
			pTask->isInnerTask = true;
			pTask->taskID = 0;
			pTask->publisherId = objectID;
			pTask->targetId = RequesTargetId;
			pTask->taskTypeId = 0;
			(*pTask->paramsPtr)["volume"] = volume;
			(*pTask->paramsPtr)["addTargetId"] = static_cast<int>(objectID);
			(*pTask->paramsPtr)["containerID"] = static_cast<int>(containerID);

			TaskMgr::getInstance().addTask(pTask);
		}
		catch (const std::bad_any_cast& e) {
			// 记录日志或进行错误处理
			DEBUG_("类型转换错误: {}", e.what());
		}
		break;
	}
	default:
		break;
	}
	
}
