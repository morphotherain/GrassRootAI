#include "NPCStation.h"
#include "BaseComponent.h"
#include "PhysicsComponent.h"
#include "SpaceTransformComponent.h"
#include "InvTypesManager.h"
#include "Task.h"

void NPCStation::Init()
{
	if (auto* spaceTran = GetComponent<SpaceTransformComponent>())
	{
		spaceTran->radius = 10000.0f;
	}
	initEntityTaskHandlers();
}

void NPCStation::Update(UINT tick)
{
	processTasks();
}

void NPCStation::fillObjectName()
{
	if (auto* base = GetComponent<BaseComponent>())
	{
		base->name = InvTypesManager::getInstance()->getNameByTypeId(base->typeID);
	}
}

void NPCStation::registerEntityTaskHandlers(EntityTaskHandlerMap& handlers)
{
	handlers.emplace("", [this](const Task& task) {});
	handlers.emplace("dock", [this](const Task& task) {
		auto publisherPtr = task.publisher.lock();
		if (!publisherPtr)
			return;
		auto* base = publisherPtr->GetComponent<BaseComponent>();
		auto* tran = publisherPtr->GetComponent<SpaceTransformComponent>();
		auto* stationTran = GetComponent<SpaceTransformComponent>();
		auto* stationBase = GetComponent<BaseComponent>();
		if (!base || !tran || !stationTran || !stationBase)
			return;

		auto distance = stationTran->calculateDistance(*tran);
		if (distance < (2500)) {
			base->setContainerID(stationBase->objectID);
		}
	});
	handlers.emplace("undock", [this](const Task& task) {
		auto publisherPtr = task.publisher.lock();
		if (!publisherPtr)
			return;
		auto* base = publisherPtr->GetComponent<BaseComponent>();
		auto* tran = publisherPtr->GetComponent<SpaceTransformComponent>();
		auto* physics = publisherPtr->GetComponent<PhysicsComponent>();
		auto* stationTran = GetComponent<SpaceTransformComponent>();
		if (!base || !tran || !physics || !stationTran)
			return;

		base->setContainerID(0);
		tran->x = stationTran->x + 10000.0f;
		tran->y = stationTran->y;
		tran->z = stationTran->z;
		tran->MarkDirty();
		tran->store();
		physics->reset();
		physics->velocity = { 100.0f, 0.0f, 0.0f };
		physics->target_velocity = { 100.0f, 0.0f, 0.0f };
	});
}
