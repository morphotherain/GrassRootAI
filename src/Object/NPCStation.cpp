﻿#include "NPCStation.h"

void NPCStation::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	m_pSpaceTran->radius = 10000.0f;
	AddComponent<Component>(m_pSpaceTran);
	m_pStation = std::make_shared<StationComponent>(objectID);
	AddComponent<Component>(m_pStation);
}

void NPCStation::Update(UINT tick)
{
	processTasks();
}

void NPCStation::fillObjectName()
{
	m_pBase->name = InvTypesManager::getInstance()->getNameByTypeId(m_pBase->typeID);
}

void NPCStation::handleTask(const Task& task)
{
	auto publisherPtr = task.publisher.lock();
	if (!publisherPtr)return;
	switch (task.taskTypeId) {
	case 0:
	{
		auto base = publisherPtr->GetComponent<BaseComponent>();
		auto tran = publisherPtr->GetComponent<SpaceTransformComponent>();
		auto distance = m_pSpaceTran->calculateDistance(*tran);
		if (distance < (2500)) {
			base->containerID = m_pBase->objectID;
		}
		else {
			break;
		}
		break;
	}
	case 1:
	{
		auto base = publisherPtr->GetComponent<BaseComponent>();
		auto tran = publisherPtr->GetComponent<SpaceTransformComponent>();
		auto physics = publisherPtr->GetComponent<PhysicsComponent>();
		base->containerID = 0;
		tran->x = m_pSpaceTran->x + 10000.0f;
		tran->y = m_pSpaceTran->y;
		tran->z = m_pSpaceTran->z;
		physics->reset();
		physics->velocity = { 100.0f, 0.0f, 0.0f };
		physics->target_velocity = { 100.0f, 0.0f, 0.0f };

		break;
	}
	default:;
	}
}