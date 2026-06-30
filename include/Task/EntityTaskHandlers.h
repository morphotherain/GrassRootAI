#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "Task.h"

using EntityTaskHandlerMap = std::unordered_map<std::string, std::function<void(const Task&)>>;

inline std::unique_ptr<EntityTaskHandlerMap> CreateEntityTaskHandlerMap()
{
	return std::make_unique<EntityTaskHandlerMap>();
}

inline void DispatchEntityTask(const EntityTaskHandlerMap* handlers, const Task& task)
{
	if (!handlers) {
		return;
	}

	if (!task.publisher.lock() || !task.target.lock()) {
		return;
	}

	const auto taskType = task.getParamOrDefault<std::string>("taskType", "");
	const auto it = handlers->find(taskType);
	if (it != handlers->end()) {
		it->second(task);
		return;
	}
	if (!taskType.empty()) {
		WARN_("[Task] Entity 未注册 taskType=\"{}\" publisher={} target={}",
			taskType, task.publisherId, task.targetId);
	}
}
