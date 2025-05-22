#include "MinerComponent.h"
#include "BaseComponent.h"
#include "GameObject.h"

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
	try {
		auto TargetObjectId = std::any_cast<int>((*task.paramsPtr)["TargetObjectId"]);

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
		else {
		}
		std::shared_ptr<Task> task = std::make_shared<Task>();
		task->isInnerTask = true;
		task->taskID = 0;
		task->publisherId = objectID;
		task->target = target;
		TaskMgr::getInstance().addTask(task);

	}
	catch (const std::bad_any_cast& e) {
		// 记录日志或进行错误处理
		DEBUG_("类型转换错误: {}", e.what());
	}
	DEBUG_(task.to_string());
}
