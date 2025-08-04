#include "Pilot.h"
#include "dynGameObjectsManager.h"
#include "dynContainersManager.h"

Pilot::Pilot(UINT _objectID, UINT _PilotID) :objectID(_objectID), PilotID(_PilotID)
{
	UINT containerID = dynGameObjectsManager::getInstance()->getContainerIdByObjectID(_objectID);
	UINT object = dynContainersManager::getInstance()->getObjectIDByContainerID(containerID);
	currentSolarSystemID = dynGameObjectsManager::getInstance()->getSolarSystemIdByObjectID(object);
}

void Pilot::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pAttributes = std::make_shared<AttributesComponent>(objectID);
	AddComponent<Component>(m_pAttributes);
	m_pSkills = std::make_shared<SkillComponent>(objectID, m_pBase->ownerID);
	AddComponent<Component>(m_pSkills);

	ResolveDependencies();
	initTaskHandlers();
}

void Pilot::Update(UINT tick)
{
	m_pBase->needStore = true;
	if (tick % 60 == 0) {
		if (m_pBase->needStore) {
			m_pBase->store();
		}
		m_pSkills->Update(tick);
	}
}
void Pilot::initTaskHandlers() {
	taskHandlers = {
		{"", [this](const Task& task) {}},
		{"skillComponent", [this](const Task& task) {
			m_pSkills->handleTask(task);
		}},
	};
}

void Pilot::handleTask(const Task& task)
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
