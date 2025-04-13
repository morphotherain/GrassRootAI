#pragma once

#include "Component.h"
#include "d3dUtil.h"
#include <unordered_map>

class LockingComponent : public Component {
public:
	LockingComponent() = default;
	LockingComponent(UINT objectID);
	~LockingComponent() = default;

	std::vector<std::type_index> GetDependencies() const override {
		return {};
	}
	void InjectDependency(const std::shared_ptr<Component>& dep) override {}

	virtual void Update(UINT tick);
	virtual void handleTask(const Task& task);

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
	int currentLockedTargetId = -1;
	std::wstring name;

    // 正向切换锁定目标
	void SwitchToNextLockedTarget();

    // 反向切换锁定目标
	void SwitchToPreviousLockedTarget();
};