#include "Pilot.h"
#include "EntityComponentAssembler.h"
#include "EntityArchetype.h"
#include "BaseComponent.h"
#include "SkillComponent.h"
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
	EntityComponentAssembler::Assemble(*this, EntityArchetype::Pilot, objectID);
	ResolveDependencies();
	initTaskHandlers();
}

void Pilot::Update(UINT tick)
{
	if (auto* base = GetComponent<BaseComponent>())
	{
		base->needStore = true;
		if (tick % 60 == 0 && base->needStore)
		{
			base->store();
		}
	}
	if (tick % 60 == 0)
	{
		if (auto* skills = GetComponent<SkillComponent>())
		{
			skills->Update(tick);
		}
	}
}

void Pilot::initTaskHandlers() {
	taskHandlers = {
		{"", [this](const Task& task) {}},
		{"skillComponent", [this](const Task& task) {
			if (auto* skills = GetComponent<SkillComponent>())
			{
				skills->handleTask(task);
			}
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
		it->second(task);
	}
}
