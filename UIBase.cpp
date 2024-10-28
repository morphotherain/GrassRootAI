#include "UIBase.h"

using namespace DirectX;

UIBase::UIBase(HINSTANCE _hInstance) :
    hInstance(_hInstance),
    m_CameraMode(CameraMode::Free)
{
}

void UIBase::setd3dResource(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, IDXGISwapChain* pSwapChain, HWND hMainWnd, ID3D11RenderTargetView* pRenderTargetView, ID3D11DepthStencilView* pDepthStencilView)
{
    m_pd3dDevice = pd3dDevice;
    m_pd3dImmediateContext = pd3dImmediateContext;
    m_pSwapChain = pSwapChain;
    m_hMainWnd = hMainWnd;
    m_pRenderTargetView = pRenderTargetView;
    m_pDepthStencilView = pDepthStencilView;
}

void UIBase::setd2dResource(ID2D1RenderTarget* pd2dRenderTarget, ID2D1SolidColorBrush* pColorBrush, IDWriteTextFormat* pTextFormat)
{
    m_pd2dRenderTarget = pd2dRenderTarget;
    m_pColorBrush = pColorBrush;
    m_pTextFormat = pTextFormat;
}

void UIBase::setcameraResource(int ClientWidth, int ClientHeight, std::shared_ptr<Camera> pCamera)
{
    m_ClientWidth = ClientWidth;
    m_ClientHeight = ClientHeight;
    m_pCamera = pCamera;
}

std::vector<PosTexIndex> UIBase::GenerateRectVertex(std::vector<PosTexIndex>& vertices ,float _x, float _y, float _deltaX, float _deltaY, float TexID)
{
    _x = _x / 10.0f;
    _y = 1080.0f - _y;
    _y = _y / 10.0f;
    _deltaX = _deltaX / 10.0f;
    _deltaY = -_deltaY / 10.0f;

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
