#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>
// 包含所有组件类型的头文件
#include "StationComponent.h"
#include "TransformComponent.h"
#include "WarpGateComponent.h"
#include "TaskMgr.h"

class GameObject {
public:
    UINT objectID = 0;
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
        tasks.push_back(task);
    }

    // 获取该GameObject的所有任务的方法
    std::vector<std::shared_ptr<Task>> getTasks() const {
        return tasks;
    }

    // 处理任务队列中任务的方法（这里简单示例为按顺序逐个处理，实际可根据具体逻辑调整）
    void processTasks() {
        size_t size = tasks.size();
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


class GameObjectMgr {
private:
    // 使用智能指针管理对象映射表，初始化为nullptr，后续由外部传入进行初始化
    static std::shared_ptr<std::unordered_map<UINT, std::shared_ptr<GameObject>>> p_mapObject;

    // 私有构造函数，保证单例模式，外部不能随意创建实例
    GameObjectMgr() {}

    // 静态单例对象，定义并初始化（C++11及之后可以保证线程安全的初始化）
    static GameObjectMgr instance;

public:
    // 设置映射表的静态方法，用于接收外部初始化好的映射表
    static void setObjectMap(std::shared_ptr<std::unordered_map<UINT, std::shared_ptr<GameObject>>> map);

    // 根据id获取对象的静态成员函数，可直接调用
    static std::shared_ptr<GameObject> getObject(UINT id);

    // 添加对象到映射表的方法（前提是映射表已通过setObjectMap设置好）
    static void addObject(UINT id, std::shared_ptr<GameObject> obj);
};

