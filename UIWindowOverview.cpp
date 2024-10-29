#include "UIWindowOverview.h"
#include "UIButton.h"
#include "UIText.h"
using namespace DirectX;


UIWindowOverview::UIWindowOverview() : UIWindow()
{

}

bool UIWindowOverview::Init()
{
	windowTitle = L"总览";

	InitWindowComponent();



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

void UIWindowOverview::OnResize()
{
}

void UIWindowOverview::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene)
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

void UIWindowOverview::DrawUI()
{
	DirectX::XMMATRIX viewMatrix = m_pWindowCamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pWindowCamera->GetProjXM();
	ConstantMVPIndex* dataPtr = m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(XMMatrixIdentity());
	dataPtr->view = XMMatrixTranspose(viewMatrix); // 转置矩阵以匹配HLSL的期望
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_windowEffect->apply();
	for (auto& component : childComponents) {
		component->DrawUI();
	}

}

void UIWindowOverview::cleanup()
{
}



bool UIWindowOverview::InitResource()
{

	return true;
}

bool UIWindowOverview::InitEffect()
{

	return true;
}

void UIWindowOverview::InitWindowComponent()
{
	UIWindow::InitWindowComponent();
	auto Texture = m_windowEffect->getTextures();
	Texture->addTextureFileName("demoTex\\UI\\Window\\window_line.dds");

	auto vertexs = m_windowEffect->getVertexBuffer<PosTexIndex>()->getVertices();
	GenerateRectVertex(vertices, x + 0.0f, y + TitleHeight, 50.0f, 30.0f, 5.0f);
	m_windowEffect->getVertexBuffer<PosTexIndex>()->setVertices(vertices);
	m_windowEffect->Init();

	auto text = std::make_shared<UIText>();
	text->setSize(x + 11.0f,  y + TitleHeight+5.0f, 350.0f, 350.0f);
	text->setText(L"默认");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 41.0f, y + TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"类型");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 81.0f, y + TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"距离");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 201.0f, y + TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"名称");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 311.0f, y + TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"角速度");
	AddUIComponent(text);


	text = std::make_shared<UIText>();
	text->setSize(x + 41.0f, y + TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"类型");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 81.0f, y + TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"距离");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 201.0f, y + TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"名称");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 311.0f, y + TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"角速度");
	AddUIComponent(text);


	text = std::make_shared<UIText>();
	text->setSize(x + 41.0f, y + 2 * TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"恒星");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 81.0f, y + 2 * TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"1.9AU");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 181.0f, y + 2 * TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"恒星(Tanoo)");
	AddUIComponent(text);

	text = std::make_shared<UIText>();
	text->setSize(x + 311.0f, y + 2 * TitleHeight + 35.0f, 350.0f, 350.0f);
	text->setText(L"0.0");
	AddUIComponent(text);



	if (windowTitle != L"") {
		auto text = std::make_shared<UIText>();
		text->setSize(x + 3.0f, y + 5.0f, 350.0f, 350.0f);
		text->setText(windowTitle);
		AddUIComponent(text);
	}
}
