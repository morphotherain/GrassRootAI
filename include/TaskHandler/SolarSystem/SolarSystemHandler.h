#pragma once
#include "TaskHandler.h"

class CreateObjectHandler : public TaskHandler {
public:
    bool handleTask(Task& task) override;

    std::string getTaskType() const override {
        return "createObject";
    }
};

class TransferObjectHandler : public TaskHandler {
public:
    bool handleTask(Task& task) override;

    std::string getTaskType() const override {
        return "transferObject";
    }
};

class DestroyObjectHandler : public TaskHandler {
public:
    bool handleTask(Task& task) override;

    std::string getTaskType() const override {
        return "destroyObject";
    }
};
