#include "PhysicsComponent.h"
#include <cmath>
#include <cstdlib>
#include <ctime>


using namespace DirectX;
#define MIN_ACCELERATION_TIME 300
#define MIN_DECELERATION_TIME 480

// 根据当前速度向量与目标速度向量的差值计算完成机动的时间（tick数）
UINT PhysicsComponent::CalculateManeuverEndTime() {
    // 先计算速度向量差值
    DirectX::XMFLOAT3 velocityDiff;
    DirectX::XMStoreFloat3(&velocityDiff, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target_velocity), DirectX::XMLoadFloat3(&velocity)));

    // 计算速度差值的大小（长度）
    double velocityDiffMagnitude = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&velocityDiff)));

    // 获取当前速度的绝对值
    double currentSpeedMagnitude = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&velocity)));

    double speedAdjustmentFactor = 0.0f;
    const double lowerThreshold = maxSpeed / 3.0f;
    const double upperThreshold = 2 * maxSpeed / 3.0f;

    if (currentSpeedMagnitude < lowerThreshold) {
        // 在较低速度区间，调整系数增长较缓慢，使用线性插值逐渐向中等速度区间的计算方式过渡
        double t = currentSpeedMagnitude / lowerThreshold;
        speedAdjustmentFactor = t * (std::pow((currentSpeedMagnitude - lowerThreshold) / (maxSpeed / 3.0f), 1.5f));
    }
    else if (currentSpeedMagnitude < upperThreshold) {
        // 在中等速度区间，调整系数增长速度加快，使用平滑的过渡函数与低速度区间和高速度区间的计算方式衔接
        double t = (currentSpeedMagnitude - lowerThreshold) / (upperThreshold - lowerThreshold);
        speedAdjustmentFactor = (1 - t) * (currentSpeedMagnitude / maxSpeed) + t * (std::pow((currentSpeedMagnitude - lowerThreshold) / (maxSpeed / 3.0f), 1.5f));
    }
    else {
        // 在接近和达到速度上限区间，调整系数快速增大，呈现明显的非线性变化，使用平滑过渡与中等速度区间衔接
        double t = (currentSpeedMagnitude - upperThreshold) / (maxSpeed - upperThreshold);
        speedAdjustmentFactor = (1 - t) * 
            (std::pow((currentSpeedMagnitude - lowerThreshold) / (maxSpeed / 3.0f), 1.5f)) + 
            t * 
            (std::pow((currentSpeedMagnitude - 2 * maxSpeed / 3.0f) / (maxSpeed / 3.0f), 3));
    }
    speedAdjustmentFactor = min(speedAdjustmentFactor, 1.0f);

    // 根据速度差值、加速时间以及速度调整系数来计算完成机动所需的时间（tick数）
    double accelerationTime = CalculateAccelerationTime();
    return static_cast<UINT>(maneuverStartTime + velocityDiffMagnitude / maxSpeed * accelerationTime * 60 * (1 + speedAdjustmentFactor));
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

    if (isWarping)
    {
        
        UINT elapsedTime = tick - warpStartTime;
        double t = 0.0f;  // 插值参数，范围从0到1，表示在整个跃迁过程中的进度

        if (elapsedTime < warpAccelerationTime)
        {
            // 加速阶段
            double factor = static_cast<double>(elapsedTime) / warpAccelerationTime;
            t = pow(factor, 8);
            t *= warpAcceleration_t;
        }
        else if (elapsedTime < warpAccelerationTime + warpUniformTime)
        {
            // 匀速阶段
            t += warpAcceleration_t;
            t += (static_cast<double>(elapsedTime - warpAccelerationTime) / static_cast<double>(warpUniformTime)) * warpUniform_t;

        }
        else
        {
            // 减速阶段
            double decelerationElapsedTime = elapsedTime - warpAccelerationTime - warpUniformTime;
            double factor = (1.0f - (static_cast<double>(decelerationElapsedTime) / warpDecelerationTime));
            t = pow(factor, 16);
            t = 1.0f - t;
            if (decelerationElapsedTime == 700) {
                t = t;
            }
            t *= warpDeceleration_t;
            t += warpAcceleration_t + warpUniform_t;
        }

        // 根据插值参数t进行位置插值计算
        currentPosition.x = std::fma(t, endPosition.x - startPosition.x, startPosition.x);
        currentPosition.y = std::fma(t, endPosition.y - startPosition.y, startPosition.y);
        currentPosition.z = std::fma(t, endPosition.z - startPosition.z, startPosition.z);

        // 根据当前位置更新飞船的速度（简单计算当前位置与上一位置的差值作为速度，可根据实际情况优化）
        if (tick > warpStartTime)
        {
            // 获取上一tick的位置（这里简单记录当前位置作为上一位置，实际可能需要更准确的记录方式）
            Pos prevPosition = currentPosition;

            // 将double类型的坐标转换为float类型，用于后续加载到XMVECTOR中
            DirectX::XMFLOAT3 currentPositionFloat;
            currentPositionFloat.x = static_cast<float>(currentPosition.x);
            currentPositionFloat.y = static_cast<float>(currentPosition.y);
            currentPositionFloat.z = static_cast<float>(currentPosition.z);

            DirectX::XMFLOAT3 prevPositionFloat;
            prevPositionFloat.x = static_cast<float>(prevPosition.x);
            prevPositionFloat.y = static_cast<float>(prevPosition.y);
            prevPositionFloat.z = static_cast<float>(prevPosition.z);

            XMVECTOR currentPosVec = XMLoadFloat3(&currentPositionFloat);
            XMVECTOR prevPosVec = XMLoadFloat3(&prevPositionFloat);
            XMVECTOR velocityVec = currentPosVec - prevPosVec;

            // 将计算得到的速度向量存储回原velocity变量（其类型为DirectX::XMFLOAT3）
            XMStoreFloat3(&velocity, velocityVec);
        }

        // 更新飞船所在的空间变换组件的位置（假设通过SpaceTran指针能访问到相关变换组件进行位置更新）
        SpaceTran->x = currentPosition.x;
        SpaceTran->y = currentPosition.y;
        SpaceTran->z = currentPosition.z;

        // 跃迁结束，重置相关状态和速度等
        if(SpaceTran->calculateDistance(endPosition.x, endPosition.y, endPosition.z, 10.0f)< 10.0f)
        {
            isWarping = false;
            currentPosition = endPosition;
            warpStartTime = 0;
            velocity = {
                static_cast<float>(warpDirection.x) * 200.0f,
                static_cast<float>(warpDirection.y) * 200.0f,
                static_cast<float>(warpDirection.z) * 200.0f
            };
            target_velocity = { 0.0f,0.0f, 0.0f };
            StartManeuver();
;        }
        
    }

    else
    {
        // 如果正在进行机动，进行速度插值更新
        if (IsManeuverInProgress(tick)) {
            float t = static_cast<float>(tick - maneuverStartTime) / static_cast<float>(maneuverEndTime - maneuverStartTime);
            InterpolateVelocity(t);
        }
    }
}

void PhysicsComponent::StartWarp()
{
    isWarping = true;
    warpStartTime = m_tick;
    startPosition = { SpaceTran->x,SpaceTran->y,SpaceTran->z };
    // 计算方向向量
    double distance = SpaceTran->calculateDistance(endPosition.x, endPosition.y, endPosition.z, 0.0f);

    warpDirection.x = endPosition.x - startPosition.x;
    warpDirection.y = endPosition.y - startPosition.y;
    warpDirection.z = endPosition.z - startPosition.z;

    // 归一化方向向量
    double length = distance;
    warpDirection.x /= length;
    warpDirection.y /= length;
    warpDirection.z /= length;

    // 根据方向向量和要提前的距离计算新的终点位置
    const double distanceToMoveForward = 2000.0;
    endPosition.x = startPosition.x + (length - distanceToMoveForward) * warpDirection.x;
    endPosition.y = startPosition.y + (length - distanceToMoveForward) * warpDirection.y;
    endPosition.z = startPosition.z + (length - distanceToMoveForward) * warpDirection.z;

    currentPosition = startPosition;
    warpAccelerationDistance = warpAccelerationTime / 120.0f * maxWarpSpeed * AU_TO_METERS;
    warpDecelerationDistance = warpDecelerationTime / 120.0f * maxWarpSpeed * AU_TO_METERS;
    double totalWarpDistance = warpAccelerationDistance + warpDecelerationDistance;
    double distanceRatio = distance / totalWarpDistance;
    if (distance > (warpAccelerationDistance + warpDecelerationDistance)) {
        warpUniformTime = static_cast<UINT>((distance - warpAccelerationDistance - warpDecelerationDistance) / (maxWarpSpeed * AU_TO_METERS) * 60);
        warpAcceleration_t = warpAccelerationDistance / distance;
        warpDeceleration_t = warpDecelerationDistance / distance;
        warpUniform_t = 1.0f - warpAcceleration_t - warpDeceleration_t;
        warpAccelerationTime = 600;
        warpDecelerationTime = 960;
    }
    else{
        warpUniformTime = 0;
        warpAccelerationTime = static_cast<UINT>max(600.0f * distanceRatio, MIN_ACCELERATION_TIME);
        warpDecelerationTime = static_cast<UINT>max(960.0f * distanceRatio, MIN_DECELERATION_TIME);
        warpDeceleration_t = static_cast<double>(warpDecelerationTime) / static_cast<double>(warpAccelerationTime + warpDecelerationTime);
        warpAcceleration_t = static_cast<double>(warpAccelerationTime) / static_cast<double>(warpAccelerationTime + warpDecelerationTime);
    }
}