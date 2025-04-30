#include "LockingComponent.h"
#include "dynGameObjectsManager.h"
#include "GameObject.h"

LockingComponent::LockingComponent(UINT _objectID)
{
	objectID = _objectID;
}

void LockingComponent::InjectDependency(const std::shared_ptr<Component>& dep)
{
	if (auto pComponent = std::dynamic_pointer_cast<AttributesComponent>(dep)) {
		m_pAttributes = pComponent;
	}
	else if (auto pComponent = std::dynamic_pointer_cast<SpaceTransformComponent>(dep)) {
		m_pSpaceTran = pComponent;
	}
}

void LockingComponent::Update(UINT tick)
{
	auto it = m_mapLockedTarget.find(currentLockedTargetId);
	if (m_mapLockedTarget.size()!= 0 && it == m_mapLockedTarget.end()) {
		currentLockedTargetId = m_mapLockedTarget.begin()->first;
	}

	if (tick % 3 != 0)
		return;

	std::vector<int> IdsToErase = {};
	for (auto& pair : m_mapLockedTarget) {
		pair.second->Update();
		auto target = GameObjectMgr::getInstance().getObject(pair.second->m_TargetObjectID);
		if (!target) {
			IdsToErase.push_back(pair.first);
			continue;
		}
		auto tran = target->GetComponent<SpaceTransformComponent>();
		if (!tran) {
			IdsToErase.push_back(pair.first);
			continue;
		}
		if (tran->calculateDistance(*m_pSpaceTran) > 10000) {
			IdsToErase.push_back(pair.first);
			continue;
		}
	}
	for (auto &id : IdsToErase) {
		EraseLocked(id);
	}
}

void LockingComponent::handleTask(const Task& task)
{
	try {
		auto direction = std::any_cast<int>((*task.paramsPtr)["direction"]);
		if (direction == 0)
			SwitchToPreviousLockedTarget();
		else
			SwitchToNextLockedTarget();
	}
	catch (const std::bad_any_cast& e) {
		// 记录日志或进行错误处理
		DEBUG_("类型转换错误: {}", e.what());
	}
}

bool LockingComponent::IsLocked(int objectID)
{
	auto it = m_mapLockedTarget.find(objectID);
	if (it == m_mapLockedTarget.end())
	{
		return false;
	}
	return true;
}

void LockingComponent::AddLocked(int objectID)
{
	auto it = m_mapLockedTarget.find(objectID);
	if (it == m_mapLockedTarget.end())
	{
		auto newLockedTarget = std::make_shared<LockedTarget>();
		newLockedTarget->m_TargetObjectID = objectID;

		m_mapLockedTarget[objectID] = newLockedTarget;
	}
}

void LockingComponent::EraseLocked(int objectID)
{
	auto it = m_mapLockedTarget.find(objectID);
	if (it != m_mapLockedTarget.end())
	{
		m_mapLockedTarget.erase(objectID);
	}
}

void LockingComponent::SwitchToNextLockedTarget()
{
	if (m_mapLockedTarget.empty()) return;

	auto it = m_mapLockedTarget.find(currentLockedTargetId);
	if (it == m_mapLockedTarget.end()) {
		// 如果当前没有锁定目标，切换到第一个
		currentLockedTargetId = m_mapLockedTarget.begin()->first;
	}
	else {
		++it;
		if (it == m_mapLockedTarget.end()) {
			// 如果已经是最后一个，切换到第一个
			currentLockedTargetId = m_mapLockedTarget.begin()->first;
		}
		else {
			currentLockedTargetId = it->first;
		}
	}
}

void LockingComponent::SwitchToPreviousLockedTarget()
{
	if (m_mapLockedTarget.empty()) return;

	auto it = m_mapLockedTarget.find(currentLockedTargetId);
	if (it == m_mapLockedTarget.end()) {
		// 如果当前没有锁定目标，切换到最后一个
		it = m_mapLockedTarget.end();
		--it;
		currentLockedTargetId = it->first;
	}
	else {
		if (it == m_mapLockedTarget.begin()) {
			// 如果已经是第一个，切换到最后一个
			it = m_mapLockedTarget.end();
			--it;
			currentLockedTargetId = it->first;
		}
		else {
			--it;
			currentLockedTargetId = it->first;
		}
	}
}

void LockingComponent::LockedTarget::Update()
{
	if (m_lockedProcess >= 1.0f)
	{
		m_lockedProcess = 1.0f;
		m_isLocked = true;
	}
	else {
		m_lockedProcess += (1.0f / (20.0f * m_totalLocketTime));
	}
}