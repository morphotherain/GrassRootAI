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

	virtual void Update(UINT tick) {
		if (tick <= 10)
		{
			velocity = { 0,0,0 };
			target_velocity = { 0,1000,0 };
		}
		UpdateTran();


		// 检查是否需要开始新的机动
		if (ShouldStartManeuver() || tick == 100) {
			// 记录机动开始时间
			maneuverStartTime = tick;

			// 根据当前速度和目标速度计算机动预计完成时间
			maneuverEndTime = CalculateManeuverEndTime();

			begin_velocity = velocity;
		}

		// 如果正在进行机动，进行速度插值更新
		if (IsManeuverInProgress(tick)) {
			float t = static_cast<float>(tick - maneuverStartTime) / static_cast<float>(maneuverEndTime - maneuverStartTime);
			InterpolateVelocity(t);
		}
		else {
			tick++;
		}
	}



	// 判断是否需要开始新的机动
	bool ShouldStartManeuver() {
		return false;
	}

	// 根据惯性系数计算从0加速到满速的时间
	float CalculateAccelerationTime() {
		// 这里简单假设公式为：加速时间 = 4 / 惯性系数
		return 4.0f / inertialCoefficient;
	}

	// 根据当前速度向量与目标速度向量的差值计算完成机动的时间（tick数）
	UINT CalculateManeuverEndTime() {
		// 先计算速度向量差值
		DirectX::XMFLOAT3 velocityDiff;
		DirectX::XMStoreFloat3(&velocityDiff, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target_velocity), DirectX::XMLoadFloat3(&velocity)));

		// 计算速度差值的大小（长度）
		float velocityDiffMagnitude = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&velocityDiff)));

		// 根据速度差值和加速时间来计算完成机动所需的时间（tick数）
		// 这里假设每tick对应的时间是dt，且dt是固定的
		float accelerationTime = CalculateAccelerationTime();
		return static_cast<UINT>(maneuverStartTime + velocityDiffMagnitude / maxSpeed * accelerationTime * 60);
	}

	// 根据插值参数t对速度进行插值更新
	void InterpolateVelocity(float t) {
		DirectX::XMVECTOR beginVelocityVec = DirectX::XMLoadFloat3(&begin_velocity);
		DirectX::XMVECTOR targetVelocityVec = DirectX::XMLoadFloat3(&target_velocity);

		// 使用线性插值（你也可以根据需要选择其他插值方法，如球面线性插值等）
		DirectX::XMVECTOR interpolatedVelocityVec = DirectX::XMVectorLerp(beginVelocityVec, targetVelocityVec, t);

		DirectX::XMStoreFloat3(&velocity, interpolatedVelocityVec);
	}

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
};