#pragma once
#include "Transform.h"
#include "Component.h"
#include <DirectXMath.h>

class SpaceTransformComponent : public Component {
public:

    SpaceTransformComponent(UINT _objectID);
    ~SpaceTransformComponent() = default;


    virtual void Update(UINT tick) {};


    double x;
    double y;
    double z;
    DirectX::XMFLOAT4 orientation; // 空间朝向

    Transform m_Tran = {};
    UINT objectID;

    SpaceTransformComponent() : orientation(1,0,0,0){}
    SpaceTransformComponent(float _x, float _y, float _z) : x(_x),y(_y),z(_z), orientation(1, 0, 0, 0) {}

};