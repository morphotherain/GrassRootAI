#include "MainScene.h"
#include "UIButton.h"
#include "UIWindow.h"
#include "UIWindowInfo.h"
#include "UIWindowMap.h"
#include "UIText.h"
#include "RenderComponent.h"

using namespace DirectX;


MainScene::MainScene(HINSTANCE _hInstance) : Scene(_hInstance)
{
	/*auto window = std::make_shared<UIWindowInfo>();
	window->setSize(100.0f, 400.0f, 200.0f, 40.0f);
	window->setTypeID(18);
	AddUIComponent(window);*/

	/*auto starmap = std::make_shared<UIWindowMap>();
	starmap->setSize(500.0f, 350.0f, 1120.0f, 657.0f);
	AddUIComponent(starmap);*/
}

bool MainScene::Init()
{
	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	for (auto& component : uiComponents) {
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}

	return true;
}

void MainScene::OnResize()
{
}

void MainScene::UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	// 获取子类
	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);
	auto camPos = cam1st->GetPosition();

	for (auto& component : uiComponents) {
		component->UpdateUI(dt, mouse, keyboard, tick);
	}

	// ******************
	// 更新摄像机
	//

	XMFLOAT3 adjustedPos;
	XMStoreFloat3(&adjustedPos, XMVectorClamp(cam1st->GetPositionXM(),
		XMVectorSet(96.8f, 52.05f, -96.85f, 0.0f), XMVectorSet(96.8f, 52.05f, -96.85f, 0.0f)));
	cam1st->SetPosition(adjustedPos);

	// 退出程序，这里应向窗口发送销毁信息
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(m_hMainWnd, WM_DESTROY, 0, 0);
}

void MainScene::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA = (255,255,255,255)
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), white);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pCamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pCamera->GetProjXM();

	ConstantMVPIndex* dataPtr = m_effect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(XMMatrixIdentity());
	dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	m_effect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_effect->apply();

	for (auto& component : uiComponents) {
		component->DrawUI();
	}

	HR(m_pSwapChain->Present(1, 0));

	return;
}

void MainScene::cleanup()
{
}

bool MainScene::InitResource()
{
	tick = 0;

	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->SetPosition(XMFLOAT3(100.0f, 100.0f, 10.0f));
	camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
	camera->LookTo(XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, +0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	camera->SetPosition(XMFLOAT3(1.0f, 1.0f, -10.0f));

	std::vector<std::string> textureFileNames = {
		"demoTex\\MainScene\\background.dds"
	};
	m_effect = std::make_shared<Effect>();

	m_effect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	m_effect->getVertexBuffer<PosTexIndex>()->setVertices(GenerateVertices(0.0f, 0.0f, 192.0f, 108.0f));
	m_effect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_effect->addConstantBuffer<ConstantMVPIndex>();
	m_effect->addTextures(textureFileNames);
	m_effect->addBlendState();
	m_effect->addSamplerState();
	m_effect->Init();

	return true;
}

bool MainScene::InitEffect()
{
	return true;
}