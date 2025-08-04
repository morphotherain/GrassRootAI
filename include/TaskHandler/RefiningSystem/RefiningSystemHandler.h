#pragma once
#include "TaskHandler.h"
#include "RefiningSystem.h"

class RefiningObjectHandler : public TaskHandler {
public:
    // 构造函数中自动注册到SolarSystemMgr
    RefiningObjectHandler() {
        // 获取单例并注册自己
        RefiningSystem::getInstance().registerHandler(std::unique_ptr<RefiningObjectHandler>(this));
    }

    bool handleTask(Task& task) override;

    std::string getTaskType() const override {
        return "refiningObject";
    }
};

