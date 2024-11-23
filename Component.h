#pragma once
#include "Mouse.h"
#include "Keyboard.h"

class GameObject; // 前向声明

class Component {
public:
    GameObject* owner;
    Component() :owner(nullptr) {}
    virtual ~Component() {}
    virtual void Update(UINT tick) = 0;
};