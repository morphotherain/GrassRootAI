#pragma once
#include <vector>
#include <memory>
// 包含所有组件类型的头文件
#include "StationComponent.h"
#include "TransformComponent.h"
#include "WarpGateComponent.h"
#include "TaskMgr.h"

class GameObject {
public:
    UINT objectID;
    std::shared_ptr<GameObject> parentObject;
    std::vector<std::shared_ptr<GameObject>> childObjects;

private:
    std::vector<std::shared_ptr<Component>> components;
    unsigned int tick = 0;

    // 使用std::list来存储任务，方便在处理任务时进行插入、删除等操作（比如任务完成后移除等）
    std::vector<std::shared_ptr<Task>> tasks;

public:
    template<typename T>
    void AddComponent(std::shared_ptr<T> component) {
        components.push_back(component);
        component->owner = this;
    }

    template<typename T>
    T* GetComponent() {
        for (auto& comp : components) {
            T* casted = dynamic_cast<T*>(comp.get());
            if (casted) {
                return casted;
            }
        }
        return nullptr;
    }

    template<typename T>
    std::vector<T*> GetComponents() {
        std::vector<T*> matchedComponents;
        for (auto& comp : components) {
            T* casted = dynamic_cast<T*>(comp.get());
            if (casted) {
                matchedComponents.push_back(casted);
            }
        }
        return matchedComponents;
    }

    virtual void Update(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard) {
        tick++;
        for (auto& comp : components) {
            comp->Update(tick);
        }
    }
    virtual void Init() {};
    virtual void Update(UINT tick) {}
    virtual std::shared_ptr<GameObject> ConvertBasedOnGroupID(UINT groupID) { return nullptr; }

    // 添加任务的方法
    void addTask(const std::shared_ptr<Task>& task) {
        task.get();
        tasks.push_back(task);
    }

    // 获取该GameObject的所有任务的方法
    std::vector<std::shared_ptr<Task>> getTasks() const {
        return tasks;
    }

    // 处理任务队列中任务的方法（这里简单示例为按顺序逐个处理，实际可根据具体逻辑调整）
    void processTasks() {
        int size = tasks.size();
        for (int i = 0; i < size; ++i) {
            auto task = tasks[i];
            handleTask(*task);
           
        }
        tasks.clear();
    }

    // 处理单个任务的虚函数，具体的子类可重写该函数实现各自不同的任务处理逻辑
    virtual void handleTask(const Task& task) {
        // 默认的任务处理逻辑，可以是空实现或者打印提示信息等，具体由子类按需重写
    }



};