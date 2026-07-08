#include "MinerComponent.h"
#include "BaseComponent.h"
#include "GameObject.h"
#include "Task/TaskParams.h"

MinerComponent::MinerComponent(UINT _objectID)
{
	objectID = _objectID;
}

void MinerComponent::InjectDependency(const std::shared_ptr<Component>& dep)
{
	if (auto pComponent = std::dynamic_pointer_cast<AttributesComponent>(dep)) {
		m_pAttributes = pComponent;
	}
	if (auto pComponent = std::dynamic_pointer_cast<EquipmentComponent>(dep)) {
		m_pEquipment = pComponent;
	}

}

void MinerComponent::Update(UINT tick)
{

}

void MinerComponent::handleTask(const Task& task)
{
	const auto params = ReadTargetObjectIdParams(task);
	const auto TargetObjectId = params.targetObjectId;

	auto target = GameObjectMgr::getInstance().getObject(TargetObjectId);
	if (target) {
		auto base = target->GetComponent<BaseComponent>();
		if (base->categoryID == 25) {
			m_pEquipment->Switch(TargetObjectId);
		}
		else {
			DEBUG_("当前装备不能作用于该对象: base->groupID:{}", base->groupID);
		}
	}
	std::shared_ptr<Task> followUpTask = std::make_shared<Task>();
	followUpTask->isInnerTask = true;
	followUpTask->taskID = 0;
	followUpTask->publisherId = objectID;
	followUpTask->target = target;
	TaskMgr::getInstance().addTask(followUpTask);

	DEBUG_(task.to_string());
}
