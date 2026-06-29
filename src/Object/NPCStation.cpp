#include "NPCStation.h"

void NPCStation::Init()
{
	m_pBase = GetComponentShared<BaseComponent>();
	m_pSpaceTran = GetComponentShared<SpaceTransformComponent>();
	m_pStation = GetComponentShared<StationComponent>();
	if (m_pSpaceTran)
	{
		m_pSpaceTran->radius = 10000.0f;
	}
	initTaskHandlers();
}

void NPCStation::Update(UINT tick)
{
	processTasks();
}

void NPCStation::fillObjectName()
{
	m_pBase->name = InvTypesManager::getInstance()->getNameByTypeId(m_pBase->typeID);
}
void NPCStation::initTaskHandlers() {
	taskHandlers = {
		{"", [this](const Task& task) {}},
		{"dock", [this](const Task& task) {
			auto publisherPtr = task.publisher.lock();
			auto base = publisherPtr->GetComponent<BaseComponent>();
			auto tran = publisherPtr->GetComponent<SpaceTransformComponent>();
			auto distance = m_pSpaceTran->calculateDistance(*tran);
			if (distance < (2500)) {
				base->containerID = m_pBase->objectID;
			}
		}},
		{"undock", [this](const Task& task) {
			auto publisherPtr = task.publisher.lock();
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
		}}
	};
}
void NPCStation::handleTask(const Task& task)
{
	auto publisherPtr = task.publisher.lock();
	auto targetPtr = task.target.lock();
	auto taskType = task.getParamOrDefault<std::string>("taskType", "");

	if (!publisherPtr || !targetPtr) {
		return;
	}

	auto it = taskHandlers.find(taskType);
	if (it != taskHandlers.end()) {
		it->second(task);  // 传递任务对象
	}
}
