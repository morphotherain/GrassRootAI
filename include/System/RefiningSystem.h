#pragma once
#include "GameObject.h"
#include "TaskMgr.h"
#include "TaskHandler.h"

class RefiningSystem {
public:
	static RefiningSystem& getInstance() {
		static RefiningSystem instance;
		return instance;
	}
	RefiningSystem() = default;
	~RefiningSystem() = default;

	void Init();
	void Update(UINT tick);
	void handleTask(Task& task);

private:
	std::unordered_map<std::string, std::unique_ptr<TaskHandler>> taskHandlers;

public:
	// 注册Handler
	void registerHandler(std::unique_ptr<TaskHandler> handler) {
		auto type = handler->getTaskType();
		taskHandlers[type] = std::move(handler);
	}
};