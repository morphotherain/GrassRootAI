﻿#include "SpaceScene.h"
#include "UIButton.h"
#include "UIShaderTest.h"
#include "UIWindowInfo.h"
#include "UIWindowMap.h"
#include "RenderComponent.h"
#include "DatabaseManager.h"
#include "SolarSystemMgr.h"
#include "invGroupsManager.h"
#include "eveBracketsManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>


#include "WindowManager.h"

using namespace DirectX;

SpaceScene::SpaceScene(HINSTANCE _hInstance) : Scene(_hInstance)
{
}

bool SpaceScene::Init()
{

	m_pSolarSystem = SolarSystemMgr::getInstance().currentSolarSystem;

	WindowManager::GetInstance().Initialize(m_ClientWidth, m_ClientHeight, m_pCamera);

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	for (auto& vertex : vertices3D) {
		auto text = std::make_shared<UIText>();
		text->setSize(0.0f, 0.0f, 350.0f, 350.0f); // 设置文本位置和尺寸
		text->setText(vertex.name); // 设置星域名称文本
		AddUIComponent(text); //添加 UI 组件

		// 存储文本对象以便后续更新
		vertex.text = (text);
	}

	auto text = std::make_shared<UIText>();
	text->setSize(350.0f, 90.0f, 350.0f, 350.0f); // 设置文本位置和尺寸
	text->setText(m_pSolarSystem->m_solarSystem.regionName); // 设置星域名称文本
	AddUIComponent(text); // 添加 UI 组件

	text = std::make_shared<UIText>();
	text->setSize(230.0f, 90.0f, 350.0f, 350.0f); // 设置文本位置和尺寸
	text->setText(std::wstring(L" < ") + m_pSolarSystem->m_solarSystem.constellationName + L" < "); // 设置星域名称文本
	AddUIComponent(text); // 添加 UI 组件

	// 获取security的值
	double securityValue = m_pSolarSystem->m_solarSystem.security;

	std::wstringstream ss;
	ss << std::fixed << std::setprecision(1) << securityValue;
	auto solarSystemName = m_pSolarSystem->m_solarSystem.solarSystemName;
	solarSystemName += L" ";
	solarSystemName += ss.str();

	text = std::make_shared<UIText>();
	text->setSize(100.0f, 80.0f, 350.0f, 350.0f); // 设置文本位置和尺寸
	text->setText(solarSystemName); // 设置星域名称文本
	text->switchTextFormat("Bold_L");
	AddUIComponent(text); // 添加 UI 组件

	/*auto window = std::make_shared<UIWindowInfo>();
	window->setSize(100.0f, 400.0f, 200.0f, 40.0f);
	window->setTypeID(620);
	AddUIComponent(window);*/

	m_WindowOverview = std::make_shared<UIWindowOverview>();
	m_WindowOverview->setSize(1350.0f, 200.0f, 550.0f, 600.0f);
	AddUIComponent(m_WindowOverview);

	auto shipUI = std::make_shared<UIShip>();
	shipUI->setSize(750.0f, 900.0f, 0.0f, 0.0f);
	shipUI->setLocalCamera(m_pLocalCamera);
	AddUIComponent(shipUI);

	/*m_RButtonMenu = std::make_shared<UIRButtonMenu>(2, 60012526);
	m_RButtonMenu->setSize(100.0f, 100.0f);
	m_RButtonMenu->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
	m_RButtonMenu->Init();*/

	for (auto& component : uiComponents) {
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}

	m_skybox = std::make_shared<UISkyBox>();
	m_skybox->setTex("demoTex\\SpaceScene\\skybox.dds");
	m_skybox->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
	m_skybox->Init();

	DEBUG_("Space Scene 初始化完成");

	return true;
}

void SpaceScene::OnResize()
{
}

void SpaceScene::UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	WindowManager::GetInstance().Update(dt, mouse, keyboard, tick);

	m_pSolarSystem = SolarSystemMgr::getInstance().currentSolarSystem;

	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	// 获取子类
	auto cam3st = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);
	auto cam3stLocal = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pLocalCamera);
	auto camPos = cam3st->GetPosition();
	float factor = 1.0f / sqrt(abs(camPos.z) + 2.0f); //控制鼠标拖曳速度

	if (m_CameraMode == CameraMode::Free)
	{
		// ******************
		// 自由摄像机的操作
		//
		cam3st->Approach(0.0f);

		// 方向移动
		if (keyState.IsKeyDown(Keyboard::W))
		{
			cam3st->RotateX(0.002f);
			cam3stLocal->RotateX(0.002f);
		}
		if (keyState.IsKeyDown(Keyboard::S))
		{
			cam3st->RotateX(-0.002f);
			cam3stLocal->RotateX(-0.002f);
		}
		if (keyState.IsKeyDown(Keyboard::A))
		{
			cam3st->RotateY(-0.005f);
			cam3stLocal->RotateY(-0.005f);
		}
		if (keyState.IsKeyDown(Keyboard::D))
		{
			cam3st->RotateY(0.005f);
			cam3stLocal->RotateY(0.005f);
		}
		if (keyState.IsKeyDown(Keyboard::Q))
		{
			cam3st->Approach(0.2f * (sqrt(cam3st->GetDistance()) / 15.0f));
			cam3stLocal->Approach(0.2f * (sqrt(cam3st->GetDistance()) / 15.0f));
		}
		if (keyState.IsKeyDown(Keyboard::E))
		{
			cam3st->Approach(-0.2f * (sqrt(cam3st->GetDistance()) / 15.0f));
			cam3stLocal->Approach(-0.2f * (sqrt(cam3st->GetDistance()) / 15.0f));
		}
		if (!(*(m_WindowOverview->getCatchingMouse()))) {
			auto delta_scroll = mouseState.scrollWheelValue - lastMouseState.scrollWheelValue;
			if (delta_scroll > 1.0f)
			{
				float distance = cam3stLocal->GetDistance();
				distance *= 0.9f;
				cam3st->SetDistance(distance);
				cam3stLocal->SetDistance(distance);
				cam3st->Approach(0.0f);
				cam3stLocal->Approach(0.0f);
			}
			if (delta_scroll < -1.0f)
			{
				float distance = cam3stLocal->GetDistance();
				distance *= 1.1f;
				cam3st->SetDistance(distance);
				cam3stLocal->SetDistance(distance);
				cam3st->Approach(0.0f);
				cam3stLocal->Approach(0.0f);
			}
		}
	}

	// 在鼠标没进入窗口前仍为ABSOLUTE模式
	if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
	{
		m_RButtonMenu = nullptr;
	}

	//// 在鼠标没进入窗口前仍为ABSOLUTE模式
	//if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.rightButton == true)
	//{
	//	m_RButtonMenu = std::make_shared<UIRButtonMenu>(2, 60012526);
	//	m_RButtonMenu->setSize(static_cast<float>(mouseState.x), static_cast<float>(mouseState.y));
	//	m_RButtonMenu->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
	//	m_RButtonMenu->Init();
	//}

	for (auto& component : uiComponents) {
		component->UpdateUI(dt, mouse, keyboard, tick);
	}

	// 退出程序，这里应向窗口发送销毁信息
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(m_hMainWnd, WM_DESTROY, 0, 0);
}

void SpaceScene::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pd3dImmediateContext->OMSetBlendState(m_pBlendState.Get(), blendFactor, 0xffffffff);

	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA = (255,255,255,255)
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), black);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	auto Pilot = m_pSolarSystem->currentPilot;
	if (Pilot == nullptr)
		return;
	auto currentShip = Pilot->currentShip;
	auto Tran = currentShip->GetComponent<SpaceTransformComponent>();

	objectVertices3D.clear();
	auto sector = m_pSolarSystem->currentSector;
	auto objects = sector->space_objects;
	for (auto obj : objects) {
		auto pobj = obj.lock();
		auto TranObj = pobj->GetComponent<SpaceTransformComponent>();
		auto Base = pobj->GetComponent<BaseComponent>();
		auto texIndex = invGroupsManager::getInstance()->getBracketIDByGroupId(Base->groupID);

		auto distance = Tran->calculateDistance(*TranObj);
		std::wstringstream wss;
		wss << Base->name << L"\n";
		wss.precision(1);  // 设置精度为1位小数
		wss << std::fixed << distance / 1000.0f << L"km";  // 使用定点表示法
		std::wstring str = wss.str();

		Vertex3DPosIndex temp = {
			(TranObj->x - sector->x),
			(TranObj->y - sector->y),
			(TranObj->z - sector->z),
			static_cast<float>(texIndex),str };
		temp.text = std::make_shared<UIText>();
		temp.text->setText(temp.name);
		temp.text->Init();
		objectVertices3D.push_back(temp);
	}

	m_skybox->DrawUI();

	auto ThirdCamera = std::reinterpret_pointer_cast<ThirdPersonCamera>(m_pCamera);
	ThirdCamera->SetTarget(
		{ static_cast<float>(Tran->x),
			static_cast<float>(Tran->y),
			static_cast<float>(Tran->z) });
	ThirdCamera->Approach(-0.00f);

	// 假设 camera 是当前场景中的摄影机对象
	DirectX::XMMATRIX viewMatrix = m_pCamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pCamera->GetProjXM();

	// 映射常量缓冲区
	auto p_constantBufferData = m_pBracketEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	p_constantBufferData->model = XMMatrixTranspose(XMMatrixIdentity());
	p_constantBufferData->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
	p_constantBufferData->projection = XMMatrixTranspose(projMatrix);
	p_constantBufferData->TexIndex = 0;
	m_pBracketEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();

	vertices.clear();

	for (auto& vertex3D : vertices3D)
	{
		double dx = vertex3D.x - Tran->x;
		double dy = vertex3D.y - Tran->y;
		double dz = vertex3D.z - Tran->z;
		auto distance = std::sqrt(dx * dx + dy * dy + dz * dz);
		if (distance < 10000000)
		{
			vertex3D.text->setSize(-100.0f, 0.0f, 0.0f, 0.0f);
			continue;
		}
		std::wstring temp;
		// 将每个三维坐标转换为 NDC 坐标
		XMFLOAT2 ndcCoord = Convert3DToNDC({
			static_cast<float>(vertex3D.x),
			static_cast<float>(vertex3D.y),
			static_cast<float>(vertex3D.z) }
		, viewMatrix, projMatrix);

		// 创建新的顶点，设置其位置为 NDC 坐标，并保持纹理索引
		Pos2Tex vertex;
		vertex.position = XMFLOAT2(ndcCoord.x, ndcCoord.y); // 将 NDC 坐标设置为顶点位置
		vertex.texIndex = vertex3D.texIndex;   // 保留原始的纹理索引

		vertex3D.text->setSize((ndcCoord.x + 1.0f) / 2.0f * 1920.0f + 20.0f, (-ndcCoord.y + 1.0f) / 2.0f * 1080.0f + 3.0f, 350.0f, 350.0f);

		// 将转换后的顶点加入目标顶点数组
		vertices.push_back(vertex);
	}
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->setVertices(vertices);
	auto p_vertexData = m_pBracketEffect->getVertexBuffer<Pos2Tex>()->Map();
	if (p_vertexData != nullptr)
	{
		// 确保数据正确地拷贝到映射缓冲区中
		memcpy(p_vertexData, vertices.data(), sizeof(Pos2Tex) * vertices.size());
	}

	// 取消映射顶点缓冲区
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->Unmap();
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->setVerticesNum(static_cast<UINT>(vertices.size()));
	//

	m_pBracketEffect->apply();
	m_pBracketEffect->clearShader();

	//-------------------------
	// 假设 camera 是当前场景中的摄影机对象

	auto ThirdCameraLocal = std::reinterpret_pointer_cast<ThirdPersonCamera>(m_pLocalCamera);
	ThirdCameraLocal->SetTarget({
			static_cast<float>(Tran->x - sector->x),
			static_cast<float>(Tran->y - sector->y),
			static_cast<float>(Tran->z - sector->z) });
	ThirdCameraLocal->Approach(-0.00f);

	DirectX::XMMATRIX viewMatrixLocal = m_pLocalCamera->GetViewXM();
	DirectX::XMMATRIX projMatrixLocal = m_pLocalCamera->GetProjXM();

	// 映射常量缓冲区
	auto p_constantBufferDataLocal = m_pBracketEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	p_constantBufferDataLocal->model = XMMatrixTranspose(XMMatrixIdentity());
	p_constantBufferDataLocal->view = XMMatrixTranspose(viewMatrixLocal); // 转置矩阵以匹配HLSL的期望
	p_constantBufferDataLocal->projection = XMMatrixTranspose(projMatrixLocal);
	p_constantBufferDataLocal->TexIndex = 0;
	m_pBracketEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();

	objectVertices.clear();

	for (auto& vertex3D : objectVertices3D)
	{
		// 将每个三维坐标转换为 NDC 坐标
		XMFLOAT2 ndcCoord = Convert3DToNDC({
			static_cast<float>(vertex3D.x),
			static_cast<float>(vertex3D.y),
			static_cast<float>(vertex3D.z) }, viewMatrixLocal, projMatrixLocal);

		// 创建新的顶点，设置其位置为 NDC 坐标，并保持纹理索引
		Pos2Tex vertex;
		vertex.position = XMFLOAT2(ndcCoord.x, ndcCoord.y); // 将 NDC 坐标设置为顶点位置
		vertex.texIndex = vertex3D.texIndex;   // 保留原始的纹理索引

		vertex3D.text->setSize((ndcCoord.x + 1.0f) / 2.0f * 1920.0f + 20.0f, (-ndcCoord.y + 1.0f) / 2.0f * 1080.0f + 3.0f, 350.0f, 350.0f);
		vertex3D.text->DrawUI();
		// 将转换后的顶点加入目标顶点数组
		objectVertices.push_back(vertex);
	}
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->setVertices(objectVertices);
	auto p_vertexDataLocal = m_pBracketEffect->getVertexBuffer<Pos2Tex>()->Map();
	if (p_vertexDataLocal != nullptr)
	{
		// 确保数据正确地拷贝到映射缓冲区中
		memcpy(p_vertexDataLocal, objectVertices.data(), sizeof(Pos2Tex) * objectVertices.size());
	}

	// 取消映射顶点缓冲区
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->Unmap();
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->setVerticesNum(static_cast<UINT>(objectVertices.size()));
	//

	m_pBracketEffect->apply();
	m_pBracketEffect->clearShader();

	for (auto& component : uiComponents) {
		component->DrawUI();
	}

	WindowManager::GetInstance().Draw();

	/*if(m_RButtonMenu != nullptr)
		m_RButtonMenu->DrawUI();*/

	HR(m_pSwapChain->Present(1, 0));
}

void SpaceScene::cleanup()
{
}

bool SpaceScene::InitResource()
{
	tick = 0;

	auto camera = std::shared_ptr<ThirdPersonCamera>(new ThirdPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
	camera->SetTarget(XMFLOAT3(331517583.0f, 43610234.0f, -586343669.0f));
	camera->SetDistance(50000.0f);
	camera->SetDistanceMinMax(1.0f, 200000000.0f);
	camera->Approach(-0.00f);

	auto Local_camera = std::shared_ptr<ThirdPersonCamera>(new ThirdPersonCamera);
	m_pLocalCamera = Local_camera;
	Local_camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	Local_camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
	Local_camera->SetTarget(XMFLOAT3(331517583.0f, 43610234.0f, -586343669.0f));
	Local_camera->SetDistance(50000.0f);
	Local_camera->SetDistanceMinMax(1.0f, 20000000.0f);
	Local_camera->Approach(-0.00f);

	std::vector<std::string> textureFileNames = eveBracketsManager::getInstance()->getAllDdsPaths();

	for (auto p_denormalize : m_pSolarSystem->m_denormalizes) {
		auto groupID = p_denormalize->groupID;
		if (groupID == 9 || groupID == 8 || groupID == 7) continue;

		double factor = 1;
		double x = p_denormalize->x / factor;
		double y = p_denormalize->y / factor;
		double z = p_denormalize->z / factor;
		Vertex3DPosIndex temp = { x,y,z,static_cast<float>(p_denormalize->bracketID),p_denormalize->name };
		//if(abs(temp.texIndex - 1.0 ) > 0.5f && abs(temp.texIndex - 2.0) > 0.5f && abs(temp.texIndex - 4.0) > 0.5f)
		vertices3D.push_back(temp);
	}

	vertices.resize(vertices3D.size());

	m_pBracketEffect = std::make_shared<Effect>();

	m_pBracketEffect->addVertexShaderBuffer<Pos2Tex>(L"HLSL\\Space\\Billboard_VS.hlsl", L"HLSL\\Space\\Billboard_VS.cso");
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->setVertices(vertices);
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->setVerticesNumMax(100);
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->setUsage(D3D11_USAGE_DYNAMIC);
	m_pBracketEffect->getVertexBuffer<Pos2Tex>()->setCPUAccessFlags(D3D11_CPU_ACCESS_WRITE);
	m_pBracketEffect->getVertexShader<Pos2Tex>()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pBracketEffect->addConstantBuffer<ConstantMVPIndex>();
	m_pBracketEffect->addPixelShader(L"HLSL\\Space\\Billboard_PS.hlsl", L"HLSL\\Space\\Billboard_PS.cso");
	m_pBracketEffect->addGeometryShader(L"HLSL\\Space\\Billboard_GS.hlsl", L"HLSL\\Space\\Billboard_GS.cso");
	m_pBracketEffect->addTextures(textureFileNames);
	m_pBracketEffect->addBlendState();
	m_pBracketEffect->addSamplerState();
	m_pBracketEffect->Init();

	return true;
}

bool SpaceScene::InitEffect()
{
	return true;
}