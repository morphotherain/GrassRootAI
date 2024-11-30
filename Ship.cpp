#include "Ship.h"
#include "InvTypesManager.h"
using namespace DirectX;

void Ship::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	AddComponent<Component>(m_pSpaceTran);
	m_pPhysics = std::make_shared<PhysicsComponent>(objectID);
	AddComponent<Component>(m_pPhysics);
	m_pPhysics->SpaceTran = m_pSpaceTran;
	fillObjectName();
}

void Ship::Update(UINT tick)
{
	processTasks();
	if (tick % 30 == 0) {
		handleApproach(approachTarget.lock());
		handleWarp(warpTarget.lock());
	}
	m_pPhysics->Update(tick);
}

void Ship::fillObjectName()
{
	m_pBase->name = InvTypesManager::getInstance()->getNameByTypeId(m_pBase->typeID);
}

void Ship::handleTask(const Task& task)
{
	switch (task.taskTypeId) {
	case 0:
	{
		if(warpTarget.lock() == nullptr)
		{
			approachTarget = task.target;
			activeTarget.reset();
		}
		break;
	}
	case 1:
	{
		warpTarget = task.target;
		approachTarget.reset();
		activeTarget.reset();
		break;
	}
	case 2:
	{
		activeTarget = task.target;
	}
	default:;
	}
}

void Ship::handleApproach(std::shared_ptr<GameObject> target)
{
	if (target == nullptr)
		return;
	auto Tran = target->GetComponent<SpaceTransformComponent>();
	// 计算朝向向量
	DirectX::XMFLOAT3 direction;
	direction.x = static_cast<float>(Tran->x - m_pSpaceTran->x);
	direction.y = static_cast<float>(Tran->y - m_pSpaceTran->y);
	direction.z = static_cast<float>(Tran->z - m_pSpaceTran->z);

	// 将XMFLOAT3转换为XMVECTOR（方便后续数学运算）
	XMVECTOR dirVec = XMLoadFloat3(&direction);
	float length = XMVector3Length(dirVec).m128_f32[0];

	// 归一化向量（计算单位向量）
	if (length > 0.0f)  // 避免除以0的情况
	{
		dirVec = XMVector3Normalize(dirVec);
		XMStoreFloat3(&direction, dirVec);
	}

	float maxSpeed = m_pPhysics->maxSpeed;  
	// 乘以最大速度
	m_pPhysics->target_velocity.x = direction.x * maxSpeed;
	m_pPhysics->target_velocity.y = direction.y * maxSpeed;
	m_pPhysics->target_velocity.z = direction.z * maxSpeed;
	m_pPhysics->StartManeuver();
}

void Ship::handleActive(std::shared_ptr<GameObject> target)
{
}

void Ship::handleWarp(std::shared_ptr<GameObject> target)
{
	if (target == nullptr)
		return;

	switch (currentWarpState) {
	case ShipWarpState::None: 
	{
		currentWarpState = ShipWarpState::PreparingWarp;
		break;
	}
	case ShipWarpState::PreparingWarp:
	{
		auto Tran = target->GetComponent<SpaceTransformComponent>();
		// 计算朝向向量
		DirectX::XMFLOAT3 direction;
		direction.x = static_cast<float>(Tran->x - m_pSpaceTran->x);
		direction.y = static_cast<float>(Tran->y - m_pSpaceTran->y);
		direction.z = static_cast<float>(Tran->z - m_pSpaceTran->z);

		// 获取当前飞船速度向量
		XMVECTOR currentVelocityVec = XMLoadFloat3(&m_pPhysics->velocity);
		float currentSpeed = XMVector3Length(currentVelocityVec).m128_f32[0];
		// 获取最大速度
		float maxSpeed = m_pPhysics->maxSpeed;


		// 将XMFLOAT3转换为XMVECTOR（方便后续数学运算）
		XMVECTOR dirVec = XMLoadFloat3(&direction);
		float length = XMVector3Length(dirVec).m128_f32[0];

		// 归一化向量（计算单位向量）
		if (length > 0.0f)  // 避免除以0的情况
		{
			dirVec = XMVector3Normalize(dirVec);
			XMStoreFloat3(&direction, dirVec);
		}

		// 先调整船头朝向目标
		m_pPhysics->target_velocity.x = direction.x * maxSpeed;
		m_pPhysics->target_velocity.y = direction.y * maxSpeed;
		m_pPhysics->target_velocity.z = direction.z * maxSpeed;
		m_pPhysics->StartManeuver();

		// 判断当前速度是否达到最大速度的75%
		bool speedRequirementMet = currentSpeed >= maxSpeed * 0.75f;

		// 计算当前船头朝向向量（假设飞船有个表示船头朝向的向量属性，这里简化为和速度方向一致，如果实际情况不同需要调整）
		XMVECTOR currentForwardVec = XMLoadFloat3(&m_pPhysics->velocity);
		if (XMVector3Length(currentForwardVec).m128_f32[0] > 0.0f)
		{
			currentForwardVec = XMVector3Normalize(currentForwardVec);
		}

		// 计算船头朝向与目标方向的夹角（使用点积来计算夹角的余弦值，再通过反余弦函数得到夹角弧度值，最后转换为角度值）
		float dotProduct = XMVector3Dot(currentForwardVec, dirVec).m128_f32[0];
		dotProduct = (dotProduct < -1.0f) ? -1.0f : ((dotProduct > 1.0f) ? 1.0f : dotProduct);
		float angleInRadians = acosf(dotProduct);
		float angleInDegrees = angleInRadians * (180.0f /	DirectX::XM_PI);
		bool angleRequirementMet = angleInDegrees <= 5.0f;

		if (speedRequirementMet && angleRequirementMet)
		{
			auto Tran = target->GetComponent<SpaceTransformComponent>();
			m_pPhysics->setTargetPos(Tran->x, Tran->y, Tran->z);
			m_pPhysics->StartWarp();
			currentWarpState = ShipWarpState::Warping;
		}
		break;
	}
	case ShipWarpState::Warping: {
		if (!m_pPhysics->isWarping) {
			currentWarpState = ShipWarpState::None;
			warpTarget.reset();
		}
		break;
	}
	}

	
}

