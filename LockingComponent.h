#pragma once

#include "Component.h"
#include "d3dUtil.h"
#include <unordered_map>

class LockingComponent : public Component {
public:
	LockingComponent() = default;
	LockingComponent(UINT objectID);
	~LockingComponent() = default;

	virtual void Update(UINT tick);

	struct LockedTarget {
		LockedTarget() = default;
		~LockedTarget() = default;

		void Update();

		float m_lockedProcess = 0.0f;
		float m_totalLocketTime = 6.0f;
		bool m_isLocked = false;

		int m_TargetObjectID;
	};
	bool IsLocked(int objectID);
	void AddLocked(int objectID);
	void EraseLocked(int objectID);

	std::unordered_map<int, std::shared_ptr<LockedTarget>> m_mapLockedTarget;
	UINT objectID;
	std::wstring name;
};