#include "UIWindow.h"
#include "UIButton.h"
#include "UIText.h"
using namespace DirectX;


UIWindow::UIWindow() : UIBase()
{

}

bool UIWindow::Init()
{
	auto button = std::make_shared<UIButton>();
	button->setSize(x, y, 400.0f, 600.0f);
	button->setTex("demoTex\\UI\\Window\\window_body.dds");
	AddUIComponent(button);

	button = std::make_shared<UIButton>();
	button->setSize(x, y, 400.0f, 32.0f);
	button->setTex("demoTex\\UI\\Window\\window_title.dds");
	AddUIComponent(button);

	button = std::make_shared<UIButton>();
	button->setSize(x + 304.0f, y, 32.0f, 32.0f);
	button->setTex("demoTex\\UI\\Window\\window_min.dds");
	AddUIComponent(button);

	button = std::make_shared<UIButton>();
	button->setSize(x + 336.0f, y, 32.0f, 32.0f);
	button->setTex("demoTex\\UI\\Window\\window_max.dds");
	AddUIComponent(button);

	button = std::make_shared<UIButton>();
	button->setSize(x + 368.0f, y, 32.0f, 32.0f);
	button->setTex("demoTex\\UI\\Window\\window_close.dds");
	AddUIComponent(button);


	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	for (auto& component : childComponents) {
		component->setd3dResource(
			*m_pd3dDevice.GetAddressOf(),
			*m_pd3dImmediateContext.GetAddressOf(),
			*m_pSwapChain.GetAddressOf(),
			m_hMainWnd,
			*m_pRenderTargetView.GetAddressOf(),
			*m_pDepthStencilView.GetAddressOf()
		);
		component->setd2dResource(*m_pd2dRenderTarget.GetAddressOf(), *m_pColorBrush.GetAddressOf(), *m_pTextFormat.GetAddressOf());
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}


	return false;
}

void UIWindow::OnResize()
{
}

void UIWindow::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene)
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
			switchScene = 2;
	}


}

void UIWindow::DrawUI()
{

	for (auto& component : childComponents) {
		component->DrawUI();
	}

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