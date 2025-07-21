#pragma once
#include "Task.h"

// 1. TaskHandler基类
class TaskHandler {
public:
    virtual ~TaskHandler() = default;
    virtual bool handleTask(Task& task) = 0;
    virtual std::string getTaskType() const = 0;  // 返回处理的任务类型
};