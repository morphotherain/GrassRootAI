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

public:
    TaskMgr() : innerID(0) {}
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

};
