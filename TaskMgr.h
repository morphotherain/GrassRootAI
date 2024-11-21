#pragma once
#include "Task.h"


class TaskMgr {
public:
    static TaskMgr& getInstance() {
        static TaskMgr instance;
        return instance;
    }
    // 根据任务ID查找任务方法
    std::shared_ptr<Task> getTaskById(UINT taskId) {
        for (const auto& t : tasks) {
            if (t->taskID == taskId) {
                return t;
            }
        }
        return nullptr;  // 如果没找到，返回空指针
    }
    // 添加任务方法
    void addTask(const std::shared_ptr<Task>& task) {
        task->innerID = innerID; innerID++;
        task->isInnerTask = true;
        tasks.push_back(task);
    }

    // 删除任务的方法（根据任务ID）
    bool deleteTaskById(UINT taskId) {
        auto it = tasks.begin();
        for (; it != tasks.end(); ++it) {
            if ((*it)->taskID == taskId) {
                tasks.erase(it);
                return true;
            }
        }
        return false;  // 如果没找到对应任务，返回删除失败
    }

    std::vector<std::shared_ptr<Task>> tasks;
    UINT innerID = 0;

private:
    TaskMgr() {}  // 私有构造函数，防止外部直接创建实例
    TaskMgr(const TaskMgr&) = delete;  // 禁止拷贝构造
    TaskMgr& operator=(const TaskMgr&) = delete;  // 禁止赋值操作

};