#include "Ship.h"
#include "InvTypesManager.h"
using namespace DirectX;

// 实现 ConvertBasedOnGroupID 方法
std::shared_ptr<GameObject> Ship::ConvertBasedOnGroupID(UINT groupID, UINT objectID) {
	switch (groupID) {
	case 25: {
		return std::make_shared<Frigate>(objectID);
	}
	case 26: {
		return std::make_shared<Cruiser>(objectID);
	}
	case 27: {
		return std::make_shared<Battleship>(objectID);
	}
	case 29: {
		return std::make_shared<Capsule>(objectID);
	}
	}
	return nullptr;
}

void Ship::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pAttributes = std::make_shared<AttributesComponent>(objectID);
	AddComponent<Component>(m_pAttributes);
	m_pEquipments = std::make_shared<EquipmentsComponent>(objectID);
	AddComponent<Component>(m_pEquipments);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	AddComponent<Component>(m_pSpaceTran);
	m_pPhysics = std::make_shared<PhysicsComponent>(objectID);
	AddComponent<Component>(m_pPhysics);
	m_pPhysics->SpaceTran = m_pSpaceTran;

	m_pPilotStorage = std::make_shared<PilotStorageComponent>(objectID);
	AddComponent<Component>(m_pPilotStorage);
	
	m_pCargoStorage = std::make_shared<CargoContainerComponent>(objectID);
	AddComponent<Component>(m_pCargoStorage);
	
	m_pHighSlotStorage = std::make_shared<HighSlotComponent>(objectID);
	AddComponent<Component>(m_pHighSlotStorage);
	m_pMediumSlotStorage = std::make_shared<MediumSlotComponent>(objectID);
	AddComponent<Component>(m_pMediumSlotStorage);
	m_pLowSlotStorage = std::make_shared<LowSlotComponent>(objectID);
	AddComponent<Component>(m_pLowSlotStorage);
	m_pRigSlotStorage = std::make_shared<RigSlotComponent>(objectID);
	AddComponent<Component>(m_pRigSlotStorage);

	m_pLocking = std::make_shared<LockingComponent>(objectID);
	AddComponent<Component>(m_pLocking);

	ResolveDependencies();
	fillObjectName();
}

void Ship::Update(UINT tick)
{
	processTasks();
	if (tick % 30 == 0) {
		handleApproach(approachTarget.lock());
		handleWarp(warpTarget.lock());
	}
	m_pSpaceTran->needStore = true;
	//m_pBase->needStore = true;
	if (tick % 60 == 0) {
		if (m_pSpaceTran->needStore) {
			m_pSpaceTran->store();
		}
		/*if (m_pBase->needStore) {
			m_pBase->store();
		}*/
	}
	m_pPhysics->Update(tick);
	m_pBase->Update(tick);
	m_pLocking->Update(tick);
	m_pEquipments->Update(tick);
	updateEquipments(tick);

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
		if (warpTarget.lock() == nullptr)
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
		break;
	}
	case 3:
	{
		m_pLocking->AddLocked(task.target->objectID);
		DEBUG_("发布锁定任务");
		break;
	}
	case 4:
	{
		m_pLocking->EraseLocked(task.target->objectID);
		DEBUG_("发布取消锁定任务");
		break;
	}
	case 5:
	{
		DEBUG_("装备按键输入");
		m_pEquipments->handleTask(task);
		break;
	}
	case 6:
	{
		DEBUG_("切换锁定目标");
		m_pLocking->handleTask(task);
		break;
	}
	case 7:
	{
		DEBUG_("");
		m_pCargoStorage->handleTask(task);
		break;
	}
	default: {
		//nothing to do
		break;
	}
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
		float angleInDegrees = angleInRadians * (180.0f / DirectX::XM_PI);
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

void Ship::updateEquipments(int tick)
{
	std::vector<std::vector<int>> ItemsIDs = {};
	ItemsIDs.push_back(m_pHighSlotStorage->itemIDs);
	ItemsIDs.push_back(m_pMediumSlotStorage->itemIDs);
	ItemsIDs.push_back(m_pLowSlotStorage->itemIDs);
	for (auto& ids : ItemsIDs) {
		for (auto id : ids) {
			auto equipment = GameObjectMgr::getInstance().getObject(id);
			if (equipment) {
				equipment->Update(tick);
			}
		}
	}
}
