#include "LockingComponent.h"
#include "dynGameObjectsManager.h"

LockingComponent::LockingComponent(UINT _objectID)
{
	objectID = _objectID;
}

void LockingComponent::Update(UINT tick)
{
	if (tick % 3 != 0)
		return;

	for (auto& pair : m_mapLockedTarget) {
		pair.second->Update();
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