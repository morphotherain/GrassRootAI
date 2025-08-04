#pragma once
#include "TaskHandler.h"
#include "SolarSystemMgr.h"

class CreateObjectHandler : public TaskHandler {
public:
    // 构造函数中自动注册到SolarSystemMgr
    CreateObjectHandler() {
        // 获取单例并注册自己
        SolarSystemMgr::getInstance().registerHandler(std::unique_ptr<CreateObjectHandler>(this));
    }

    bool handleTask(Task& task) override;

    std::string getTaskType() const override {
        return "createObject";
    }
};

class TransferObjectHandler : public TaskHandler {
public:
    // 构造函数中自动注册到SolarSystemMgr
    TransferObjectHandler() {
        // 获取单例并注册自己
        SolarSystemMgr::getInstance().registerHandler(std::unique_ptr<TransferObjectHandler>(this));
    }

    bool handleTask(Task& task) override;

    std::string getTaskType() const override {
        return "transferObject";
    }
};

class DestroyObjectHandler : public TaskHandler {
public:
    // 构造函数中自动注册到SolarSystemMgr
    DestroyObjectHandler() {
        // 获取单例并注册自己
        SolarSystemMgr::getInstance().registerHandler(std::unique_ptr<DestroyObjectHandler>(this));
    }

    bool handleTask(Task& task) override;

    std::string getTaskType() const override {
        return "destroyObject";
    }
};