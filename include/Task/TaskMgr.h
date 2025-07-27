#pragma once
#include "Task.h"
#include <stack>

class TaskBatch {
private:
    std::vector<std::shared_ptr<Task>> tasks; // 临时收集任务

public:
    // 添加任务到批处理
    void add(const std::shared_ptr<Task>& task) {
        tasks.push_back(task);
    }

    // 析构时自动批量提交到TaskMgr
    ~TaskBatch();
};

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
    // 创建TaskBatch（此时TaskBatch已完整定义，可正常返回）
    TaskBatch createBatch() {
        return TaskBatch(); // 返回一个临时的批处理器
    }

    // 任务入栈方法
    void addTask(const std::shared_ptr<Task>& task) {
        task->innerID = innerID;
        innerID++;
        task->isInnerTask = true;
        tasks.push(task);
    }
    void addTasks(const std::vector<std::shared_ptr<Task>>& taskList) {
        // 反转任务列表，让先添加的任务后入栈（从而先执行）
        for (auto it = taskList.rbegin(); it != taskList.rend(); ++it) {
            addTask(*it); // 复用单个入栈逻辑，确保innerID正确
        }
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
