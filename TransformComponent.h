#pragma once
#include "Component.h"
#include <DirectXMath.h>

class TransformComponent : public Component {
public:
    DirectX::XMFLOAT3 position;

    TransformComponent() : position(0, 0, 0) {}
    TransformComponent(float _x, float _y, float _z) : position(_x, _y, _z) {}
    virtual void Update(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard) {}
};