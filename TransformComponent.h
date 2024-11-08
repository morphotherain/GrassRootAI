#pragma once
#include "Component.h"
#include "Transform.h"
#include <DirectXMath.h>

class TransformComponent : public Component {
public:
    DirectX::XMFLOAT3 position;

    Transform m_Tran = {};

    TransformComponent() : position(0, 0, 0) {}
    TransformComponent(float _x, float _y, float _z) : position(_x, _y, _z) {}
    void MoveX(float d) { m_Tran.Translate(DirectX::XMFLOAT3{ 1.0f, 0.0f, 0.0f }, d); };
    void MoveY(float d) { m_Tran.Translate(DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f }, d); };
    void MoveZ(float d) { m_Tran.Translate(DirectX::XMFLOAT3{ 0.0f, 0.0f, 1.0f }, d); };

    virtual void Update(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick) {}
};