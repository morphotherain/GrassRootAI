#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include "Component.h"
#include "SpaceTransformComponent.h"

const double AU_TO_METERS = 149597870700.0;

struct Pos {
	double x;
	double y;
	double z;
};

class PhysicsComponent : public Component {
public:
	PhysicsComponent() = default;
	PhysicsComponent(UINT _object_id) { object_id = _object_id; };
	~PhysicsComponent() = default;

	std::vector<std::type_index> GetDependencies() const override {
		return { typeid(SpaceTransformComponent) };
	}
	void InjectDependency(const std::shared_ptr<Component>& dep) override;

	UINT object_id;
	UINT m_tick = 0;

	// 速度向量
	DirectX::XMFLOAT3 velocity = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 begin_velocity = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 target_velocity = { 0.0f,0.0f,0.0f };

	std::shared_ptr<SpaceTransformComponent> SpaceTran;

	//惯性系数
	float inertialCoefficient = 1.0;

	// 用于记录机动开始的时间（tick数）
	UINT maneuverStartTime = 0;

	// 用于记录机动预计完成的时间（tick数）
	UINT maneuverEndTime = 0;

	// 假设最大速度是一个固定值，你可根据实际情况调整
	const float maxSpeed = 500.0f;

	// 最大跃迁速度（以AU/s为单位）
	float maxWarpSpeed = 3.0f;

	// 跃迁加速时间（以tick为单位，可以根据实际游戏帧率等因素调整时间尺度）
	UINT warpAccelerationTime = 180;
	double warpAccelerationDistance = 0.0f;
	double warpAcceleration_t = 0.0f;

	// 跃迁减速时间（以tick为单位）
	UINT warpDecelerationTime = 300;
	double warpDecelerationDistance = 0.0f;
	double warpDeceleration_t = 0.0f;

	UINT warpUniformTime = 0;
	double warpUniform_t = 0.0f;

	// 标记当前是否处于跃迁状态
	bool isWarping = false;

	// 用于记录跃迁开始的时间（tick数）
	UINT warpStartTime = 0;

	// 记录起点坐标（假设用三维向量表示飞船在空间中的位置）
	Pos startPosition = { 0.0f,0.0f,0.0f };

	// 记录终点坐标
	Pos endPosition = { 0.0f,0.0f,0.0f };

	// 当前位置坐标（用于在跃迁过程中实时更新飞船位置）
	Pos currentPosition = { 0.0f,0.0f,0.0f };

	Pos warpDirection = { 0.0f,0.0f,0.0f };

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

	void StartWarp();

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
	double CalculateSpeedMagnitude() const {
		return sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
	}

	void reset();
	void setTargetVelocity(DirectX::XMFLOAT3 v) {
		target_velocity = v;
	}

	void setTargetPos(double x, double y, double z) {
		endPosition.x = x;
		endPosition.y = y;
		endPosition.z = z;
	}
};