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
    double radius = 100.0f;

    Transform m_Tran = {};
    UINT objectID;

    SpaceTransformComponent() : orientation(1,0,0,0){}
    SpaceTransformComponent(float _x, float _y, float _z) : x(_x),y(_y),z(_z), orientation(1, 0, 0, 0) {}

    double calculateDistance(const SpaceTransformComponent& _tran) {
        double dx = this->x - _tran.x;
        double dy = this->y - _tran.y;
        double dz = this->z - _tran.z;
        double distance = sqrt(dx * dx + dy * dy + dz * dz) - this->radius - _tran.radius;
        distance = distance >= 0.0f ? distance : 0.0f;

        return distance;
    }

};