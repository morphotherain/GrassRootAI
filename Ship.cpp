#include "Ship.h"
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
}

void Ship::Update(UINT tick)
{
	processTasks();
	m_pPhysics->Update(tick);
}

void Ship::handleTask(const Task& task)
{
	switch (task.taskTypeId) {
	case 0:
	{
		handleApproach(task);
		break;
	}
	case 1:
	{

		break;
	}
	default:;
	}
}

void Ship::handleApproach(const Task& task)
{
	auto Tran = task.target->GetComponent<SpaceTransformComponent>();
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

	float maxSpeed = m_pPhysics->maxSpeed*10;  // 这里假设最大速度为10，你可以根据实际情况修改
	// 乘以最大速度
	m_pPhysics->target_velocity.x = direction.x * maxSpeed;
	m_pPhysics->target_velocity.y = direction.y * maxSpeed;
	m_pPhysics->target_velocity.z = direction.z * maxSpeed;
	m_pPhysics->StartManeuver();
}
