#include "UIWindow.h"
#include "UIButton.h"
#include "UIText.h"
using namespace DirectX;


UIWindow::UIWindow() : UIBase()
{

}

bool UIWindow::Init()
{
	windowTitle = L"窗口";

	InitWindowComponent();



	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	for (auto& component : childComponents) {
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}


	return false;
}

void UIWindow::OnResize()
{
}

void UIWindow::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = mouse.GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);




	// 在鼠标没进入窗口前仍为ABSOLUTE模式
	if (mouseState.positionMode == Mouse::MODE_ABSOLUTE && mouseState.leftButton == true)
	{
		if ((x * 10) < mouseState.x && ((x + deltaX) * 10) > mouseState.x && (1080 - y * 10 - deltaY * 10) < mouseState.y && (1080 - y * 10) > mouseState.y)
			;
	}


}

void UIWindow::DrawUI()
{

	for (auto& component : childComponents) {
		component->DrawUI();
	}
	m_windowEffect->apply();

}

void UIWindow::cleanup()
{
}



bool UIWindow::InitResource()
{

	return true;
}

bool UIWindow::InitEffect()
{

	return true;
}

void UIWindow::InitWindowComponent()
{
	textureWindowFileNames = {
		"demoTex\\UI\\Window\\window_body.dds",
		"demoTex\\UI\\Window\\window_title.dds",
		"demoTex\\UI\\Window\\window_min.dds",
		"demoTex\\UI\\Window\\window_max.dds",
		"demoTex\\UI\\Window\\window_close.dds"
	};

	GenerateRectVertex(vertices, x, y, width, height, 0.0f);
	GenerateRectVertex(vertices, x, y, width, TitleHeight, 1.0f);
	GenerateRectVertex(vertices, x + width - 3 * TitleHeight, y, TitleHeight, TitleHeight, 2.0f);
	GenerateRectVertex(vertices, x + width - 2 * TitleHeight, y, TitleHeight, TitleHeight, 3.0f);
	GenerateRectVertex(vertices, x + width - 1 * TitleHeight, y, TitleHeight, TitleHeight, 4.0f);

	m_windowEffect = std::make_shared<Effect>();
	m_windowEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	m_windowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertices);
	m_windowEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_windowEffect->addTextures(textureWindowFileNames);
	m_windowEffect->addConstantBuffer<ConstantMVPIndex>();

	if (windowTitle != L"") {
		auto text = std::make_shared<UIText>();
		text->setSize(x + 3.0f, y + 5.0f, 350.0f, 350.0f);
		text->setText(windowTitle);
		AddUIComponent(text);
	}

	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	m_pWindowCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->SetPosition(XMFLOAT3(100.0f, 100.0f, 10.0f));
	camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
	camera->LookTo(XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, +0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	camera->SetPosition(XMFLOAT3(96.8f, 52.05f, -96.85f));
}
