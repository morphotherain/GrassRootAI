#include "EquipmentComponent.h"

EquipmentComponent::EquipmentComponent(UINT _objectID)
{
	objectID = _objectID;
}

void EquipmentComponent::InjectDependency(const std::shared_ptr<Component>& dep)
{
	if (auto pComponent = std::dynamic_pointer_cast<AttributesComponent>(dep)) {
		m_pAttributes = pComponent;
	}

}

void EquipmentComponent::Switch(int targetObjectId)
{
	keep_active = !keep_active;
	if (keep_active)
		m_targetObjectId = targetObjectId;
	else
		m_targetObjectId = -1;
}

void EquipmentComponent::Update(UINT tick)
{
	m_beginActive = false;
	m_endActive = false;
	m_isActiving = false;
	if (m_activateProcess >= 1.0f)
	{
		m_endActive = true;
		m_activateProcess = 0.0f;
	}
	if (m_activateProcess != 0.0f) {
		m_isActiving = true;
		m_activateProcess += (1.0f / (20.0f * m_totalActivateTime));
	}
	if (m_activateProcess == 0.0f && keep_active) {
		m_beginActive = true;
		m_activateProcess += (1.0f / (20.0f * m_totalActivateTime));
	}
}

void EquipmentComponent::handleTask(const Task& task)
{
	Switch(-1);
}
