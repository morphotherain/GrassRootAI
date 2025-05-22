#include "UIWindowIndusty.h"
#include "UIButton.h"
#include "UIText.h"

#include "InvTypesManager.h"
#include "dynContainersManager.h"
#include "dynGameObjectsManager.h"

using namespace DirectX;

UIWindowIndusty::UIWindowIndusty() : UIWindow()
{
}

bool UIWindowIndusty::Init()
{
	windowTitle = L"demo";

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

void UIWindowIndusty::OnResize()
{
}

void UIWindowIndusty::UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	UIWindow::UpdateUI(dt, mouse, keyboard, tick);
	for (auto& component : childComponents) {
		component->setDelta(x, y);
	}

}

void UIWindowIndusty::DrawUI()
{
	DirectX::XMMATRIX viewMatrix = m_pUICamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pUICamera->GetProjXM();
	XMMATRIX windowModel = XMMatrixTranslation(x, y, 0.0f);
	ConstantMVPIndex* dataPtr = m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(windowModel);
	dataPtr->view = XMMatrixTranspose(viewMatrix);
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	m_windowEffect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_windowEffect->apply();

	for (auto& component : childComponents) {
		component->DrawUI();
	}
}

void UIWindowIndusty::cleanup()
{
}

bool UIWindowIndusty::InitResource()
{
	return true;
}

bool UIWindowIndusty::InitEffect()
{
	return true;
}


void UIWindowIndusty::ParseParameters(std::unordered_map<std::string, std::any> paras)
{
	x = 300;
	y = 400;
	deltaX = 800;
	deltaY = 600;
	UIWindow::setSize(x, y, deltaX, deltaY);
}

void UIWindowIndusty::InitWindowComponent()
{
	UIWindow::InitWindowComponent();

	m_windowEffect->Init();

}

