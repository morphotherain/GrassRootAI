#include "TaskMgr.h"

void TaskMgr::registerSystemHandler(int systemType, const std::function<void(const std::shared_ptr<Task>&)>& handler)
{
	systemHandlers[systemType] = handler;
}
