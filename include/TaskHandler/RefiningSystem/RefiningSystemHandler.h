#pragma once
#include "TaskHandler.h"

class RefiningObjectHandler : public TaskHandler {
public:
    bool handleTask(Task& task) override;

    std::string getTaskType() const override {
        return "refiningObject";
    }
};
