#include "TaskMgr.h"

TaskBatch::~TaskBatch() {
    if (!tasks.empty()) {
        TaskMgr::getInstance().addTasks(tasks); // 调用批量添加接口（按顺序执行）
    }
}

void TaskMgr::registerSystemHandler(int systemType, const std::function<void(const std::shared_ptr<Task>&)>& handler)
{
	systemHandlers[systemType] = handler;
}

void TaskMgr::ResetRuntime()
{
	while (!tasks.empty())
	{
		tasks.pop();
	}
	innerID = 0;
	for (auto& handler : systemHandlers)
	{
		handler = nullptr;
	}
}
