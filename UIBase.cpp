#include "UIBase.h"

using namespace DirectX;

UIBase::UIBase(HINSTANCE _hInstance) :
    hInstance(_hInstance),
    m_CameraMode(CameraMode::Free)
{
}


void UIBase::setcameraResource(int ClientWidth, int ClientHeight, std::shared_ptr<Camera> pCamera)
{
    m_ClientWidth = ClientWidth;
    m_ClientHeight = ClientHeight;
    m_pCamera = pCamera;
}

std::vector<PosTexIndex> UIBase::GenerateRectVertex(std::vector<PosTexIndex>& vertices ,float _x, float _y, float _deltaX, float _deltaY, float TexID)
{
    /*_x = _x / 10.0f;
    _y = 1080.0f - _y;
    _y = _y / 10.0f;
    _deltaX = _deltaX / 10.0f;
    _deltaY = -_deltaY / 10.0f;*/

    // 第一个三角形
    vertices.push_back({ XMFLOAT3(_x, (_y + _deltaY), -0.0f),             XMFLOAT2(0.0f, 1.0f), TexID });
    vertices.push_back({ XMFLOAT3(_x, (_y), -0.0f),       XMFLOAT2(0.0f, 0.0f), TexID });
    vertices.push_back({ XMFLOAT3((_x + _deltaX), (_y + _deltaY), -0.0f),       XMFLOAT2(1.0f, 1.0f), TexID });

    // 第二个三角形
    vertices.push_back({ XMFLOAT3((_x + _deltaX), (_y + _deltaY), -0.0f),       XMFLOAT2(1.0f, 1.0f), TexID });
    vertices.push_back({ XMFLOAT3(_x, (_y), -0.0f),       XMFLOAT2(0.0f, 0.0f), TexID });
    vertices.push_back({ XMFLOAT3((_x + _deltaX), (_y), -0.0f), XMFLOAT2(1.0f, 0.0f), TexID });


    return vertices;
}


