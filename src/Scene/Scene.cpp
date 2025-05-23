#include "Scene.h"

Scene::Scene(HINSTANCE _hInstance) :
	hInstance(_hInstance),
	m_CameraMode(CameraMode::Free)
{
}

void Scene::setd3dResource(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, IDXGISwapChain* pSwapChain, HWND hMainWnd, ID3D11RenderTargetView* pRenderTargetView, ID3D11DepthStencilView* pDepthStencilView)
{
	m_pd3dDevice = pd3dDevice;
	m_pd3dImmediateContext = pd3dImmediateContext;
	m_pSwapChain = pSwapChain;
	m_hMainWnd = hMainWnd;
	m_pRenderTargetView = pRenderTargetView;
	m_pDepthStencilView = pDepthStencilView;
}

void Scene::setd2dResource(ID2D1RenderTarget* pd2dRenderTarget, ID2D1SolidColorBrush* pColorBrush, IDWriteTextFormat* pTextFormat)
{
	m_pd2dRenderTarget = pd2dRenderTarget;
	m_pColorBrush = pColorBrush;
	m_pTextFormat = pTextFormat;
}

void Scene::setcameraResource(int ClientWidth, int ClientHeight, std::shared_ptr<Camera> pCamera)
{
	m_ClientWidth = ClientWidth;
	m_ClientHeight = ClientHeight;
	m_pCamera = pCamera;
}