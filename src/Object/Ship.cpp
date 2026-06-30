#include "Ship.h"
#include "InvTypesManager.h"
#include "BaseComponent.h"
#include "AttributesComponent.h"
#include "EquipmentsComponent.h"
#include "SpaceTransformComponent.h"
#include "PhysicsComponent.h"
#include "StorageComponent.h"
#include "LockingComponent.h"
#include "Task.h"
using namespace DirectX;

void Ship::Init()
{
	initEntityTaskHandlers();
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

	auto* spaceTran = GetComponent<SpaceTransformComponent>();
	auto* physics = GetComponent<PhysicsComponent>();
	auto* locking = GetComponent<LockingComponent>();
	auto* equipments = GetComponent<EquipmentsComponent>();
	auto* attributes = GetComponent<AttributesComponent>();

	if (physics)
	{
		physics->Update(tick);
	}
	if (spaceTran)
	{
		spaceTran->Update(tick);
	}
	if (locking) locking->Update(tick);
	if (equipments) equipments->Update(tick);
	if (attributes) attributes->Update(tick);
	updateEquipments(tick);
}

void Ship::fillObjectName()
{
	if (auto* base = GetComponent<BaseComponent>())
	{
		base->name = InvTypesManager::getInstance()->getNameByTypeId(base->typeID);
	}
}

void Ship::registerEntityTaskHandlers(EntityTaskHandlerMap& handlers)
{
	handlers.emplace("", [this](const Task& task) {});
	handlers.emplace("setApproachTarget", [this](const Task& task) {
		approachTarget = task.target;
		activeTarget.reset();
	});
	handlers.emplace("setWarpTarget", [this](const Task& task) {
		warpTarget = task.target;
		approachTarget.reset();
		activeTarget.reset();
	});
	handlers.emplace("setActiveTarget", [this](const Task& task) {
		activeTarget = task.target;
	});
	handlers.emplace("addLocked", [this](const Task& task) {
		auto targetPtr = task.target.lock();
		if (targetPtr && GetComponent<LockingComponent>())
		{
			GetComponent<LockingComponent>()->AddLocked(targetPtr->objectID);
			DEBUG_("发布锁定任务");
		}
	});
	handlers.emplace("eraseLocked", [this](const Task& task) {
		auto targetPtr = task.target.lock();
		if (targetPtr && GetComponent<LockingComponent>())
		{
			GetComponent<LockingComponent>()->EraseLocked(targetPtr->objectID);
			DEBUG_("发布取消锁定任务");
		}
	});
	handlers.emplace("equipments", [this](const Task& task) {
		DEBUG_("装备按键输入");
		if (auto* equipments = GetComponent<EquipmentsComponent>())
		{
			equipments->handleTask(task);
		}
	});
	handlers.emplace("refreshEquipment", [this](const Task& task) {
		DEBUG_("刷新装备");
		if (auto* equipments = GetComponent<EquipmentsComponent>())
		{
			equipments->Refresh();
		}
	});
	handlers.emplace("locking", [this](const Task& task) {
		DEBUG_("切换锁定目标");
		if (auto* locking = GetComponent<LockingComponent>())
		{
			locking->handleTask(task);
		}
	});
	handlers.emplace("cargoStorage", [this](const Task& task) {
		DEBUG_("");
		if (auto* cargo = GetComponent<CargoContainerComponent>())
		{
			cargo->handleTask(task);
		}
	});
}

void Ship::handleApproach(std::shared_ptr<GameObject> target)
{
	if (target == nullptr)
		return;

	auto* spaceTran = GetComponent<SpaceTransformComponent>();
	auto* physics = GetComponent<PhysicsComponent>();
	if (!spaceTran || !physics)
		return;

	auto Tran = target->GetComponent<SpaceTransformComponent>();
	if (!Tran)
		return;

	DirectX::XMFLOAT3 direction;
	direction.x = static_cast<float>(Tran->x - spaceTran->x);
	direction.y = static_cast<float>(Tran->y - spaceTran->y);
	direction.z = static_cast<float>(Tran->z - spaceTran->z);

	XMVECTOR dirVec = XMLoadFloat3(&direction);
	float length = XMVector3Length(dirVec).m128_f32[0];

	if (length > 0.0f)
	{
		dirVec = XMVector3Normalize(dirVec);
		XMStoreFloat3(&direction, dirVec);
	}

	float maxSpeed = physics->maxSpeed;
	physics->target_velocity.x = direction.x * maxSpeed;
	physics->target_velocity.y = direction.y * maxSpeed;
	physics->target_velocity.z = direction.z * maxSpeed;
	physics->StartManeuver();
}

void Ship::handleActive(std::shared_ptr<GameObject> target)
{
}

void Ship::handleWarp(std::shared_ptr<GameObject> target)
{
	if (target == nullptr)
		return;

	auto* spaceTran = GetComponent<SpaceTransformComponent>();
	auto* physics = GetComponent<PhysicsComponent>();
	if (!spaceTran || !physics)
		return;

	switch (currentWarpState) {
	case ShipWarpState::None:
		currentWarpState = ShipWarpState::PreparingWarp;
		break;

	case ShipWarpState::PreparingWarp:
	{
		auto Tran = target->GetComponent<SpaceTransformComponent>();
		if (!Tran)
			break;

		DirectX::XMFLOAT3 direction;
		direction.x = static_cast<float>(Tran->x - spaceTran->x);
		direction.y = static_cast<float>(Tran->y - spaceTran->y);
		direction.z = static_cast<float>(Tran->z - spaceTran->z);

		XMVECTOR currentVelocityVec = XMLoadFloat3(&physics->velocity);
		float currentSpeed = XMVector3Length(currentVelocityVec).m128_f32[0];
		float maxSpeed = physics->maxSpeed;

		XMVECTOR dirVec = XMLoadFloat3(&direction);
		float length = XMVector3Length(dirVec).m128_f32[0];

		if (length > 0.0f)
		{
			dirVec = XMVector3Normalize(dirVec);
			XMStoreFloat3(&direction, dirVec);
		}

		physics->target_velocity.x = direction.x * maxSpeed;
		physics->target_velocity.y = direction.y * maxSpeed;
		physics->target_velocity.z = direction.z * maxSpeed;
		physics->StartManeuver();

		bool speedRequirementMet = currentSpeed >= maxSpeed * 0.75f;

		XMVECTOR currentForwardVec = XMLoadFloat3(&physics->velocity);
		if (XMVector3Length(currentForwardVec).m128_f32[0] > 0.0f)
		{
			currentForwardVec = XMVector3Normalize(currentForwardVec);
		}

		float dotProduct = XMVector3Dot(currentForwardVec, dirVec).m128_f32[0];
		dotProduct = (dotProduct < -1.0f) ? -1.0f : ((dotProduct > 1.0f) ? 1.0f : dotProduct);
		float angleInRadians = acosf(dotProduct);
		float angleInDegrees = angleInRadians * (180.0f / DirectX::XM_PI);
		bool angleRequirementMet = angleInDegrees <= 5.0f;

		if (speedRequirementMet && angleRequirementMet)
		{
			physics->setTargetPos(Tran->x, Tran->y, Tran->z);
			physics->StartWarp();
			currentWarpState = ShipWarpState::Warping;
		}
		break;
	}
	case ShipWarpState::Warping:
		if (!physics->isWarping) {
			currentWarpState = ShipWarpState::None;
			warpTarget.reset();
		}
		break;
	}
}

void Ship::updateEquipments(int tick)
{
	auto updateSlotItems = [tick](StorageComponent* slot) {
		if (!slot) return;
		for (auto id : slot->itemIDs) {
			if (auto equipment = GameObjectMgr::getInstance().getObject(id))
			{
				equipment->Update(tick);
			}
		}
	};

	updateSlotItems(GetComponent<HighSlotComponent>());
	updateSlotItems(GetComponent<MediumSlotComponent>());
	updateSlotItems(GetComponent<LowSlotComponent>());
}
