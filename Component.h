#pragma once
#include "Mouse.h"
#include "Keyboard.h"

class GameObject; // 前向声明

class Component {
public:
    GameObject* owner;
    virtual ~Component() {}
    virtual void Update(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard) = 0;
};