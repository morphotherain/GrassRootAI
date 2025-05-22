#pragma once

#include "Component.h"
#include "d3dUtil.h"
#include "AttributesComponent.h"
#include "SpaceTransformComponent.h"
#include <unordered_map>

class LockingComponent : public Component {
public:
	LockingComponent() = default;
	LockingComponent(UINT objectID);
	~LockingComponent() = default;

	std::vector<std::type_index> GetDependencies() const override {
		return {
			typeid(AttributesComponent),
			typeid(SpaceTransformComponent)
		};
	}
	void InjectDependency(const std::shared_ptr<Component>& dep) override;

	virtual void Update(UINT tick);
	virtual void handleTask(const Task& task);

	struct LockedTarget {
		LockedTarget() = default;
		~LockedTarget() = default;

		void Update();

		float m_lockedProcess = 0.0f;
		float m_totalLocketTime = 3.0f;
		bool m_isLocked = false;

		int m_TargetObjectID;
	};
	bool IsLocked(int objectID);
	void AddLocked(int objectID);
	void EraseLocked(int objectID);

	std::shared_ptr<AttributesComponent> m_pAttributes;
	std::shared_ptr<SpaceTransformComponent> m_pSpaceTran;

	std::unordered_map<int, std::shared_ptr<LockedTarget>> m_mapLockedTarget;
	UINT objectID;
	int currentLockedTargetId = -1;
	std::wstring name;

    // 正向切换锁定目标
	void SwitchToNextLockedTarget();

    // 反向切换锁定目标
	void SwitchToPreviousLockedTarget();
};