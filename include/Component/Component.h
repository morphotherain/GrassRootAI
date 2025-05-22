#pragma once
#include "Mouse.h"
#include "Keyboard.h"
#include "TaskMgr.h"
#include <typeindex>
#include <vector>

class GameObject; // 前向声明

class Component {
public:
	GameObject* owner;
	Component() :owner(nullptr) {}
	virtual ~Component() {}
	virtual void Update(UINT tick) = 0;
	virtual void handleTask(const Task& task) {};
	virtual void OnDestroy() {};
	virtual void Destroy() { OnDestroy(); };

	virtual std::vector<std::type_index> GetDependencies() const = 0;
	virtual void InjectDependency(const std::shared_ptr<Component>& dep) = 0;
};


