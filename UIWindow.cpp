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

	HandleMouseEvent(dt, mouse);

	if (!IsActive()) {
		return;
	}

	for (auto& component : childComponents) {
		component->setDelta(x, y);
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

void UIWindow::HandleMouseEvent(float dt, DirectX::Mouse& mouse)
{
	m_inputHandler.Update(dt, mouse.GetState());
	if (!IsActive())
	{
		m_inputHandler.SetDetectionArea(x, y, width, height);
		bool active = m_inputHandler.IsMousePressedInArea();
		SetActive(active);
	}
	if (!IsActive()) {
		return;
	}

	m_inputHandler.SetDetectionArea(x + width - TitleHeight, y, TitleHeight, TitleHeight);

	bool should_closed = m_inputHandler.IsMousePressedInArea();
	if (should_closed) {
		SetClosed(true);
		return;
	}

	m_inputHandler.SetDetectionArea(x, y, width, TitleHeight);

	if (m_inputHandler.IsMouseDragging()) {
		int dx, dy;
		m_inputHandler.GetDragDelta(dx, dy);
		x += dx;
		y += dy;
	}
}

void UIWindow::InitWindowComponent()
{
	textureWindowFileNames = {
		"demoTex\\UI\\Window\\window_body.dds",
		"demoTex\\UI\\Window\\window_title.dds",
		"demoTex\\UI\\Window\\window_min.dds",
		"demoTex\\UI\\Window\\window_max.dds",
		"demoTex\\UI\\Window\\window_close.dds",
		"demoTex\\UI\\Window\\window_deepblue.dds",
		"demoTex\\UI\\Window\\window_select.dds",
		"demoTex\\UI\\Window\\window_line.dds"
	};

	DEBUG_("width:{} height:{}", width, height);
	GenerateRectVertex(vertices, 0.0f, 0.0f, width, height, 0.0f);
	GenerateRectVertex(vertices, 0.0f, 0.0f, width, TitleHeight, 1.0f);
	GenerateRectVertex(vertices, width - 3 * TitleHeight, 0.0f, TitleHeight, TitleHeight, 2.0f);
	GenerateRectVertex(vertices, width - 2 * TitleHeight, 0.0f, TitleHeight, TitleHeight, 3.0f);
	GenerateRectVertex(vertices, width - 1 * TitleHeight, 0.0f, TitleHeight, TitleHeight, 4.0f);
	GenerateRectVertex(vertices, 0.0f, 0.0f, width, 1.0f, 7.0f);
	GenerateRectVertex(vertices, 0.0f, height - 1.0f, width, 1.0f, 7.0f);
	GenerateRectVertex(vertices, 0.0f, 0.0f, 1.0f, height, 7.0f);
	GenerateRectVertex(vertices, width - 1.0f, 0.0f, 1.0f, height, 7.0f);

	m_windowEffect = std::make_shared<Effect>();
	m_windowEffect->addVertexShaderBuffer<PosTexIndex>(L"HLSL\\Triangle_VS.hlsl", L"HLSL\\Triangle_VS.cso");
	m_windowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertices);
	m_windowEffect->addPixelShader(L"HLSL\\Triangle_PS.hlsl", L"HLSL\\Triangle_PS.cso");
	m_windowEffect->addTextures(textureWindowFileNames);
	m_windowEffect->addConstantBuffer<ConstantMVPIndex>();

	auto UIcamera = std::make_shared<OrthographicCamera>();
	m_pUICamera = UIcamera;

	// 设置正交投影参数
	float left = 0.0f;
	float right = static_cast<float>(m_ClientWidth);
	float bottom = static_cast<float>(m_ClientHeight);
	float top = 0.0f;
	float nearZ = 0.0f;
	float farZ = 1.0f;
	UIcamera->SetOrthographic(left, right, bottom, top, nearZ, farZ);

	if (windowTitle != L"") {
		auto text = std::make_shared<UIText>();
		text->setSize(3.0f, 5.0f, 350.0f, 350.0f);
		text->switchTextFormat("Arial_XS");
		text->setText(windowTitle);
		AddUIComponent(text);
	}
}