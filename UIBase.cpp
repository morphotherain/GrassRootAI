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

std::vector<PosTexIndex> UIBase::GenerateRectVertex(std::vector<PosTexIndex>& vertices, float _x, float _y, float _deltaX, float _deltaY, float angle, float TexID)
{
    // 计算矩形的中心点
    float centerX = _x + _deltaX / 2.0f;
    float centerY = _y + _deltaY / 2.0f;

    // 平移矩阵：将矩形移动到原点
    DirectX::XMMATRIX translationToOrigin = DirectX::XMMatrixTranslation(-centerX, -centerY, 0.0f);
    // 旋转矩阵
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationZ(angle);
    // 平移矩阵：将矩形移回原来的位置
    DirectX::XMMATRIX translationBack = DirectX::XMMatrixTranslation(centerX, centerY, 0.0f);

    // 组合矩阵
    DirectX::XMMATRIX combinedMatrix = translationToOrigin * rotationMatrix * translationBack;

    // 矩形的四个角点
    DirectX::XMFLOAT3 points[4] = {
        DirectX::XMFLOAT3(_x, _y, 0.0f),
        DirectX::XMFLOAT3(_x + _deltaX, _y, 0.0f),
        DirectX::XMFLOAT3(_x + _deltaX, _y + _deltaY, 0.0f),
        DirectX::XMFLOAT3(_x, _y + _deltaY, 0.0f)
    };

    // 旋转每个点
    for (int i = 0; i < 4; ++i) {
        DirectX::XMVECTOR point = DirectX::XMLoadFloat3(&points[i]);
        point = DirectX::XMVector3Transform(point, combinedMatrix);
        DirectX::XMStoreFloat3(&points[i], point);
    }

    //DEBUG_(" points[0]({},{}), points[1]({},{}), points[2]({},{}), points[3]({},{})",
    //    points[0].x, points[0].y,
    //    points[1].x, points[1].y,
    //    points[2].x, points[2].y,
    //    points[3].x, points[3].y
    //    );

    // 第一个三角形
    vertices.push_back({ points[0], DirectX::XMFLOAT2(0.0f, 1.0f), TexID }); // 左下
    vertices.push_back({ points[1], DirectX::XMFLOAT2(1.0f, 1.0f), TexID }); // 右下
    vertices.push_back({ points[2], DirectX::XMFLOAT2(1.0f, 0.0f), TexID }); // 右上

    // 第二个三角形 (0, 2, 3)
    vertices.push_back({ points[0], DirectX::XMFLOAT2(0.0f, 1.0f), TexID }); // 左下
    vertices.push_back({ points[2], DirectX::XMFLOAT2(1.0f, 0.0f), TexID }); // 右上
    vertices.push_back({ points[3], DirectX::XMFLOAT2(0.0f, 0.0f), TexID }); // 左上

    return vertices;
}

DirectX::XMMATRIX CreateRotatedWindowModel(float x, float y, float width, float height, float rotationAngle)
{

    // 计算组件的中心
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;

    // 步骤 1: 平移到原点
    DirectX::XMMATRIX translateToOrigin = DirectX::XMMatrixTranslation(-centerX, -centerY, 0.0f);

    // 步骤 2: 进行旋转
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationZ(rotationAngle);

    // 步骤 3: 平移回原位置
    DirectX::XMMATRIX translateBack = DirectX::XMMatrixTranslation(centerX, centerY, 0.0f);

    // 组合变换矩阵
    return  DirectX::XMMatrixTranslation(0, 0, 0.0f) * translateToOrigin * rotationMatrix * translateBack * DirectX::XMMatrixTranslation(x, y, 0.0f);
}
