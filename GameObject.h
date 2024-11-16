#pragma once
#include <vector>
#include <memory>
// 包含所有组件类型的头文件
#include "TransformComponent.h"

class GameObject {
public:
    UINT objectID;
    std::shared_ptr<GameObject> parentObject;
    std::vector<std::shared_ptr<GameObject>> childObjects;

private:
    std::vector<std::shared_ptr<Component>> components;
    unsigned int tick = 0;

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
};