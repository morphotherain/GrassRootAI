#include "TaskHandlerRegistry.h"


// 处理任务
bool TaskHandlerRegistry::Handle(const Task& task) const {
    auto type = task.getParamOrDefault<std::string>(typeKey, "");
    auto it = handlers.find(type);
    if (it != handlers.end()) {
        it->second(task);
        return true;
    }
    return false;
}