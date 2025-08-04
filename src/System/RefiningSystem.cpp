#include "RefiningSystem.h"

void RefiningSystem::Init() {

}

void RefiningSystem::Update(UINT tick) {

}

void RefiningSystem::handleTask(Task& task) {
	if (task.targetSystem == REFINING)
	{
		auto handleType = task.getParamOrDefault<std::string>("handlerType", "");
		auto it = taskHandlers.find(handleType);
		if (it != taskHandlers.end()) {
			it->second->handleTask(task);
			return;
		}
	}
}