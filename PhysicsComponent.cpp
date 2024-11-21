#include "PhysicsComponent.h"

// 根据当前速度向量与目标速度向量的差值计算完成机动的时间（tick数）
UINT PhysicsComponent::CalculateManeuverEndTime() {
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
void PhysicsComponent::InterpolateVelocity(float t) {
	DirectX::XMVECTOR beginVelocityVec = DirectX::XMLoadFloat3(&begin_velocity);
	DirectX::XMVECTOR targetVelocityVec = DirectX::XMLoadFloat3(&target_velocity);

	// 使用线性插值（你也可以根据需要选择其他插值方法，如球面线性插值等）
	DirectX::XMVECTOR interpolatedVelocityVec = DirectX::XMVectorLerp(beginVelocityVec, targetVelocityVec, t);

	DirectX::XMStoreFloat3(&velocity, interpolatedVelocityVec);
}

void PhysicsComponent::reset()
{
	velocity = { 0.0f,0.0f, 0.0f };
	begin_velocity = { 0.0f,0.0f, 0.0f };
	target_velocity = { 0.0f,0.0f, 0.0f };
	maneuverStartTime = 0;
	maneuverEndTime = 0;
}

void PhysicsComponent::Update(UINT tick) {
	m_tick = tick;
	if (tick <= 10)
	{
		velocity = { 0,0,0 };
		target_velocity = { 0,100,0 };
	}
	UpdateTran();

	// 如果正在进行机动，进行速度插值更新
	if (IsManeuverInProgress(tick)) {
		float t = static_cast<float>(tick - maneuverStartTime) / static_cast<float>(maneuverEndTime - maneuverStartTime);
		InterpolateVelocity(t);
	}
}