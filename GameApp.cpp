#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;


GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance),
	m_CameraMode(CameraMode::Free)
{
}

GameApp::~GameApp()
{
}

void GameApp::SwitchToScene(std::unique_ptr<Scene> newScene) {

	if (currentScene) {
		currentScene->cleanup();
	}

	currentScene = std::move(newScene);
	if (currentScene) {
		currentScene->setd3dResource(
			*m_pd3dDevice.GetAddressOf(), 
			*m_pd3dImmediateContext.GetAddressOf(), 
			*m_pSwapChain.GetAddressOf(), 
			MainWnd(),
			*m_pRenderTargetView.GetAddressOf(),
			*m_pDepthStencilView.GetAddressOf()
		);
		currentScene->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		currentScene->Init();
	}
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	// 初始化鼠标，键盘不需要
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	SwitchToScene(std::make_unique<MainScene>(AppInst()));


	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{
	currentScene->UpdateScene(dt, *m_pMouse, *m_pKeyboard);
	return;
	
}


void GameApp::DrawScene()
{
	currentScene->DrawScene();
	return;
	
}


bool GameApp::InitEffect()
{
	return true;
}


bool GameApp::InitResource()
{
	return true;
}
