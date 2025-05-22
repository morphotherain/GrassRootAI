#include "EquipmentsComponent.h"

EquipmentsComponent::EquipmentsComponent(UINT _objectID)
{
	objectID = _objectID;
}

void EquipmentsComponent::InjectDependency(const std::shared_ptr<Component>& dep)
{
	if (auto pComponent = std::dynamic_pointer_cast<AttributesComponent>(dep)) {
		m_pAttributes = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<LockingComponent>(dep)) {
		m_pLocking = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<HighSlotComponent>(dep)) {
		m_pHighSlot = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<MediumSlotComponent>(dep)) {
		m_pMediumSlot = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<LowSlotComponent>(dep)) {
		m_pLowSlot = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<RigSlotComponent>(dep)) {
		m_pRigSlot = pComponent;
	}
}

void EquipmentsComponent::Update(UINT tick)
{
}

void EquipmentsComponent::handleTask(const Task& task)
{
	try {
		auto slotType = std::any_cast<std::string>((*task.paramsPtr)["slotType"]);
		auto taskType = std::any_cast<std::string>((*task.paramsPtr)["taskType"]);
		auto slotIndex = std::any_cast<int>((*task.paramsPtr)["slotIndex"]);

		DEBUG_("slotType, taskType, slotIndex : {}{}{}", slotType, taskType, slotIndex);
		if (taskType == "switch") {
			int targetObjectID = -1;
			if (slotType == "high") {
				if (slotIndex >= 0 && slotIndex < static_cast<int>(m_pHighSlot->itemIDs.size())) {
					targetObjectID = m_pHighSlot->itemIDs[slotIndex];
				}
			}
			if (slotType == "medium") {
				if (slotIndex >= 0 && slotIndex < static_cast<int>(m_pMediumSlot->itemIDs.size())) {
					targetObjectID = m_pMediumSlot->itemIDs[slotIndex];
				}
			}
			if (slotType == "low") {
				if (slotIndex >= 0 && slotIndex < static_cast<int>(m_pLowSlot->itemIDs.size())) {
					targetObjectID = m_pLowSlot->itemIDs[slotIndex];
				}
			}
			auto target = GameObjectMgr::getInstance().getObject(targetObjectID);
			std::shared_ptr<Task> task = std::make_shared<Task>();
			task->isInnerTask = true;
			task->taskID = 0;
			task->publisherId = objectID;
			task->target = target;
			(*task->paramsPtr)["TargetObjectId"] = m_pLocking->currentLockedTargetId;
			TaskMgr::getInstance().addTask(task);
		}
	}
	catch (const std::bad_any_cast& e) {
		// 记录日志或进行错误处理
		DEBUG_("类型转换错误: {}", e.what());
	}


}
