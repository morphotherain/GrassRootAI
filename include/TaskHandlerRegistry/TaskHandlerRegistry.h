#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <Task.h>

#define TASK_HANDLER [this](const Task& task)

class TaskHandlerRegistry {
public:
    std::unordered_map<std::string, std::function<void(const Task&)>> handlers;
    std::string typeKey;

public:
    // 注册任务处理器（支持方法链）
    template<typename Func>
    TaskHandlerRegistry& Register(const std::string& taskType, Func&& handler) {
        handlers[taskType] = std::forward<Func>(handler);
        return *this;
    };

    void setTypeKey(std::string key) { typeKey = key; }
    // 处理任务
    bool Handle(const Task& task) const;
};