#include "DockScene.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIWindowStorage.h"
#include "RenderComponent.h"
#include "DatabaseManager.h"
#include "SolarSystemMgr.h"

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC DockScene::VertexPosColor::inputLayout[3] = {
	// 位置字段
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理坐标字段
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// 纹理索引字段
	{ "TEXINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};



std::vector<PosTexIndex>  DockScene::GenerateVertices(int n) {
	std::vector<PosTexIndex> vertices;

	vertices.reserve(6); // 每个格子两个三角形，每个三角形3个顶点

	float x = 0.0f;
	float y = 0.0f;
	float deltaX = 192.0f;
	float deltaY = 108.0f;

	// 第一个三角形
	vertices.push_back({ XMFLOAT3(x, y, 0.0f),             XMFLOAT2(0.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(x, (y + deltaY), 0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((x + deltaX), y, 0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });

	// 第二个三角形
	vertices.push_back({ XMFLOAT3((x + deltaX), y, 0.0f),       XMFLOAT2(1.0f, 1.0f), 0 });
	vertices.push_back({ XMFLOAT3(x, (y + deltaY), 0.0f),       XMFLOAT2(0.0f, 0.0f), 0 });
	vertices.push_back({ XMFLOAT3((x + deltaX), (y + deltaY), 0.0f), XMFLOAT2(1.0f, 0.0f), 0 });

	return vertices;
}

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

	fs.open(L"C:\\Users\\DottogNoggle\\Desktop\\output.txt", std::fstream::in | std::fstream::out | std::fstream::app);

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

	if (m_CameraMode == CameraMode::Free)
	{
		// ******************
		// 自由摄像机的操作
		//

		// 方向移动
		if (keyState.IsKeyDown(Keyboard::W))
		{
			cam1st->MoveY(dt * 6.0f);
		}
		if (keyState.IsKeyDown(Keyboard::S))
		{
			cam1st->MoveY(dt * -6.0f);
		}
		if (keyState.IsKeyDown(Keyboard::A))
		{
			cam1st->MoveX(dt * -6.0f);
		}
		if (keyState.IsKeyDown(Keyboard::D))
		{
			cam1st->MoveX(dt * 6.0f);
		}
		// 在鼠标没进入窗口前仍为ABSOLUTE模式
		if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
		{
			auto pos = cam1st->GetPosition();
			auto delta_Y = lastMouseState.y - mouseState.y;
			auto delta_X = lastMouseState.x - mouseState.x;

			cam1st->MoveY(-delta_Y * dt * factor);
			cam1st->MoveX(delta_X * dt * factor);
		}
		auto delta_scroll = mouseState.scrollWheelValue - lastMouseState.scrollWheelValue;
		if (delta_scroll > 1.0f)
			cam1st->MoveZ(1.0f);
		if (delta_scroll < -1.0f)
			cam1st->MoveZ(-1.0f);
	}

	for (auto& component : uiComponents) {
		component->UpdateUI(dt, mouse, keyboard, tick);
	}

	if (*m_button->getClickFlag()) {
		UINT currentPilotID = SolarSystemMgr::getInstance().currentPilotID;
		UINT currentPilotObjectID = dynGameObjectsManager::getInstance()->getPilotObjectIDByPilotID(currentPilotID);
		UINT currentPilotContainerID = dynGameObjectsManager::getInstance()->getContainerIdByObjectID(currentPilotObjectID);
		dynGameObjectsManager::getInstance()->updateContainerIDByObjectID(currentPilotContainerID, 40000001);
		*(m_button->getClickFlag()) = false;
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

	/*if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wstring textStr = L"$~:hello_你好 ";
		m_pd2dRenderTarget->DrawTextW(textStr.c_str(), textStr.size(), m_pTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, m_pColorBrush.Get());
		HR(m_pd2dRenderTarget->EndDraw());
	}*/

	HR(m_pSwapChain->Present(1, 0));
}

void DockScene::cleanup()
{
	fs.close();
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
	m_effect->getVertexBuffer<PosTexIndex>()->setVertices(GenerateVertices(4));
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