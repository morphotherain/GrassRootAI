#pragma once
#include <vector>
#include <memory>
// 包含所有组件类型的头文件
#include "Component.h"
#include "TransformComponent.h"

class GameObject {
private:
    std::vector<std::shared_ptr<Component>> components;

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

    void Update(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard) {
        for (auto& comp : components) {
            comp->Update(dt, mouse, keyboard);
        }
    }
};