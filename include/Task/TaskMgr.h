#pragma once
#include "Task.h"
#include <stack>

class TaskMgr {
private:
    // 单例模式，构造函数设为私有
    // 禁止拷贝构造和赋值操作
    TaskMgr(const TaskMgr&) = delete;
    TaskMgr& operator=(const TaskMgr&) = delete;

    std::stack<std::shared_ptr<Task>> tasks;
    UINT innerID;

    // 任务分发映射表
    std::vector<std::function<void(const std::shared_ptr<Task>&)>> systemHandlers;

public:
    TaskMgr() : innerID(0), systemHandlers(MAX_SYSTEM_TYPE) {}
    // 获取单例实例的静态方法
    static TaskMgr& getInstance() {
        static TaskMgr instance;
        return instance;
    }

    // 任务入栈方法
    void addTask(const std::shared_ptr<Task>& task) {
        task->innerID = innerID;
        innerID++;
        task->isInnerTask = true;
        tasks.push(task);
    }

    // 任务出栈方法
    std::shared_ptr<Task> popTask() {
        if (tasks.empty()) {
            return nullptr;
        }
        std::shared_ptr<Task> task = tasks.top();
        tasks.pop();
        return task;
    }

    // 检查栈是否为空
    bool isEmpty() const {
        return tasks.empty();
    }

    // 注册系统处理器
    void registerSystemHandler(int systemType, const std::function<void(const std::shared_ptr<Task>&)>& handler);

    // 分发任务到注册的系统
    void distributeTasksFromTaskMgr() {
        while (!tasks.empty()) {
            auto task = popTask();
            if (task) {
                // 检查任务的目标系统
                const auto& handler = systemHandlers[task->targetSystem];
                handler(task);
            }
        }
    }

};
