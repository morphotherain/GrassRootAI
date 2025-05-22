#include "DockScene.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIWindowStorage.h"
#include "RenderComponent.h"
#include "DatabaseManager.h"
#include "SolarSystemMgr.h"

using namespace DirectX;

DockScene::DockScene(HINSTANCE _hInstance) : Scene(_hInstance)
{
}

bool DockScene::Init()
{
	m_button = std::make_shared<UIButton>();
	m_button->setSize(1500.0f, 400.0f, 100.0f, 40.0f);
	m_button->setTex("demoTex\\UI\\Window\\window_line.dds");
	AddUIComponent(m_button);

	/*auto storage = std::make_shared<UIWindowStorage>();
	storage->setSize(10.0f, 40.0f, 20.0f, 4.0f);
	AddUIComponent(storage);*/

	auto text = std::make_shared<UIText>();
	text->setSize(1530.0f, 410.0f, 500.0f, 500.0f);
	text->setText(L"出站");
	AddUIComponent(text);

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

void DockScene::OnResize()
{
}

void DockScene::UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
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
	float factor = 1.0f / sqrt(abs(camPos.z) + 2.0f); //控制鼠标拖曳速度

	for (auto& component : uiComponents) {
		component->UpdateUI(dt, mouse, keyboard, tick);
	}

	static int cooldown = 0;
	if (cooldown > 0) cooldown--;
	if (cooldown > 0)*(m_button->getClickFlag()) = false;
	if (*m_button->getClickFlag() && cooldown == 0) {
		DEBUG_("click");
		auto currentShip = SolarSystemMgr::getInstance().currentPilot->currentShip;
		auto currentStationID = currentShip->GetComponent<BaseComponent>()->containerID;
		auto it = SolarSystemMgr::getInstance().p_mapObject->find(currentStationID);
		std::shared_ptr<GameObject> currentStation;
		if (it != SolarSystemMgr::getInstance().p_mapObject->end()) {
			currentStation = it->second;
		}
		else {
			DEBUG_("Object with ID 30000083 not found in p_mapObject");
		}
		std::shared_ptr<Task> task = std::make_shared<Task>();
		task->isInnerTask = true;
		task->taskID = -1;
		task->publisher = currentShip;
		task->target = currentStation;
		task->taskTypeId = 1;
		DEBUG_("currentStationID :{}", currentStationID);
		DEBUG_("currentStation :{}", currentStation == nullptr);
		//输出所有p_mapObject
		for (auto& pair : *SolarSystemMgr::getInstance().p_mapObject) {
			DEBUG_("Object ID: {}", pair.first);
		}
		if (currentStation != nullptr)
			currentStation->addTask(task);

		*(m_button->getClickFlag()) = false;
		cooldown = 60;
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

void DockScene::DrawScene()
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
}

void DockScene::cleanup()
{
}

bool DockScene::InitResource()
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
		"demoTex\\DockScene\\background.dds"
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

bool DockScene::InitEffect()
{
	return true;
}