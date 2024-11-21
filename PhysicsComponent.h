#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include "Component.h"
#include "SpaceTransformComponent.h"

class PhysicsComponent : public Component {
public:
	PhysicsComponent() = default;
	PhysicsComponent(UINT _object_id) { object_id = _object_id; };
	~PhysicsComponent() = default;

	UINT object_id;
	UINT m_tick;

	// 速度向量
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 begin_velocity;
	DirectX::XMFLOAT3 target_velocity;

	std::shared_ptr<SpaceTransformComponent> SpaceTran;

	//惯性系数
	float inertialCoefficient = 1.0;

	// 用于记录机动开始的时间（tick数）
	UINT maneuverStartTime;

	// 用于记录机动预计完成的时间（tick数）
	UINT maneuverEndTime;

	// 假设最大速度是一个固定值，你可根据实际情况调整
	const float maxSpeed = 1000.0f;

	virtual void Update(UINT tick);

	// 判断是否需要开始新的机动
	void StartManeuver() {
		// 记录机动开始时间
		maneuverStartTime = m_tick;

		// 根据当前速度和目标速度计算机动预计完成时间
		maneuverEndTime = CalculateManeuverEndTime();

		begin_velocity = velocity;
	}

	// 根据惯性系数计算从0加速到满速的时间
	float CalculateAccelerationTime() {
		// 这里简单假设公式为：加速时间 = 4 / 惯性系数
		return 4.0f / inertialCoefficient;
	}

	// 根据当前速度向量与目标速度向量的差值计算完成机动的时间（tick数）
	UINT CalculateManeuverEndTime();

	// 根据插值参数t对速度进行插值更新
	void InterpolateVelocity(float t);

	void UpdateTran() {
		SpaceTran->x += velocity.x / 60.0f;
		SpaceTran->y += velocity.y / 60.0f;
		SpaceTran->z += velocity.z / 60.0f;
	}

	// 判断机动是否正在进行
	bool IsManeuverInProgress(UINT tick) {
		return tick >= maneuverStartTime && tick < maneuverEndTime;
	}

	// 计算速度绝对值的方法
	float CalculateSpeedMagnitude() const {
		return sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
	}

	void reset();
	void setTargetVelocity(DirectX::XMFLOAT3 v) {
		target_velocity = v;

	}
};