#include "MainScene.h"
#include "UIText.h"
#include "Effect.h"
#include "VertexData.h"
#include "ConstantBufferData.h"
#include "UIRect.h"
#include "UIRenderContext.h"
#include "UISystem.h"
#include "UIAtlasRegistry.h"
#include "UIDevOverlay.h"
#include "UIInputState.h"
#include "UIInputRouter.h"
#include "UINavigationStack.h"
#include "MainMenuScreenController.h"
#include "D3DManager.h"
#include "logger_manager.h"

#if UIF_USE_LEGACY_MAIN_MENU
#include "UIButton.h"
#endif

using namespace DirectX;

MainScene::MainScene(HINSTANCE _hInstance) : Scene(_hInstance)
{
}

MainScene::~MainScene() = default;

bool MainScene::InitUIF()
{
	UIAtlasRegistry::Instance().LoadAtlasesFromDirectory("resource/ui/atlas");

	m_uifContext = std::make_unique<UIRenderContext>();
	m_uifContext->Initialize(m_ClientWidth, m_ClientHeight);

	m_uifSystem = std::make_unique<UISystem>();
	m_uifSystem->SetRenderContext(m_uifContext.get());

	m_navigation = std::make_unique<UINavigationStack>();
	m_navigation->Initialize(m_uifSystem.get(), m_uifContext.get(), m_ClientWidth, m_ClientHeight);
	m_navigation->ReplaceRoot(
		"resource/ui/main_menu.ui.json",
		std::make_unique<MainMenuScreenController>(*m_navigation, m_gameplayHost));

	UIInputRouter::Instance().SetCharHandler([this](wchar_t ch) {
		return m_navigation && m_navigation->OnChar(ch);
	});

	m_uifEnabled = true;
	INFO_("UIF 主菜单导航已加载");
	return true;
}

bool MainScene::Init()
{
#if UIF_USE_LEGACY_MAIN_MENU
	m_startButton = std::make_shared<UIButton>();
	m_startButton->setSize(800.0f, 500.0f, 320.0f, 48.0f);
	m_startButton->setTex("demoTex\\UI\\Window\\window_line.dds");
	m_startButton->setText(L"开始新游戏");
	AddUIComponent(m_startButton);
#endif

	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

#if UIF_USE_LEGACY_MAIN_MENU
	for (auto& component : uiComponents) {
		component->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		component->Init();
	}
#endif

	if (!InitUIF())
	{
		ERROR_("UIF 主菜单加载失败");
#if UIF_USE_LEGACY_MAIN_MENU
		return true;
#else
		return false;
#endif
	}

	return true;
}

void MainScene::OnResize()
{
	if (m_uifContext)
	{
		m_uifContext->OnResize(m_ClientWidth, m_ClientHeight);
	}
	if (m_navigation)
	{
		m_navigation->OnClientResize(m_ClientWidth, m_ClientHeight);
	}
}

void MainScene::SetNavigationBusy(bool busy)
{
	if (m_navigation)
	{
		m_navigation->SetBusy(busy);
	}
}

void MainScene::UpdateUIFInput(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard)
{
	Mouse::State mouseState = mouse.GetState();
	m_MouseTracker.Update(mouseState);

	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	m_uifInput.mouseX = static_cast<float>(mouseState.x);
	m_uifInput.mouseY = static_cast<float>(mouseState.y);
	m_uifInput.mouseLeftDown = mouseState.leftButton;
	m_uifInput.mouseLeftPressed = m_MouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED;
	m_uifInput.mouseLeftReleased = m_MouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED;

	auto& devOverlay = UIDevOverlay::Instance();
	if (dt > 0.0f)
	{
		devOverlay.SetStats(1.0f / dt, dt * 1000.0f);
	}

	if (m_KeyboardTracker.IsKeyPressed(Keyboard::F11))
	{
		m_uifInput.captureNode.reset();
	}
	devOverlay.HandleGlobalKeys(m_KeyboardTracker);
	devOverlay.HandleEditorKeys(m_KeyboardTracker, keyState);

	if (devOverlay.ConsumesInput())
	{
		devOverlay.UpdateInput(m_uifInput);
		return;
	}

	if (m_navigation && !m_navigation->IsBusy())
	{
		if (m_KeyboardTracker.IsKeyPressed(Keyboard::Escape))
		{
			if (m_navigation->Depth() > 1)
			{
				m_navigation->HandleBack();
			}
			else
			{
				SendMessage(m_hMainWnd, WM_DESTROY, 0, 0);
			}
		}
	}

	if (m_uifSystem && m_navigation && !m_navigation->IsBusy())
	{
		m_navigation->Update(m_uifInput);
		m_uifSystem->Update(m_uifInput);
	}
}

void MainScene::UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick)
{
	if (m_uifEnabled)
	{
		UpdateUIFInput(dt, mouse, keyboard);
		return;
	}

#if UIF_USE_LEGACY_MAIN_MENU
	Mouse::State mouseState = mouse.GetState();
	m_MouseTracker.Update(mouseState);
	Keyboard::State keyState = keyboard.GetState();
	m_KeyboardTracker.Update(keyState);

	for (auto& component : uiComponents) {
		component->UpdateUI(dt, mouse, keyboard, tick);
	}

	if (m_startButton && *m_startButton->getClickFlag() && m_gameplayHost.onQuickStartNewGame)
	{
		m_gameplayHost.onQuickStartNewGame();
	}

	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Enter) && m_gameplayHost.onQuickStartNewGame)
	{
		m_gameplayHost.onQuickStartNewGame();
	}

	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Escape))
		SendMessage(m_hMainWnd, WM_DESTROY, 0, 0);
#endif
	(void)tick;
}

void MainScene::DrawScene()
{
	if (m_uifEnabled && m_uifSystem)
	{
		m_uifSystem->Draw();
		if (m_uifContext)
		{
			UIDevOverlay::Instance().Draw(*m_uifContext);
		}
		D3DManager::getInstance().present(true);
		return;
	}

#if UIF_USE_LEGACY_MAIN_MENU
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), white);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DirectX::XMMATRIX viewMatrix = m_pCamera->GetViewXM();
	DirectX::XMMATRIX projMatrix = m_pCamera->GetProjXM();

	ConstantMVPIndex* dataPtr = m_effect->getConstantBuffer<ConstantMVPIndex>()->Map();
	dataPtr->model = XMMatrixTranspose(XMMatrixIdentity());
	dataPtr->view = XMMatrixTranspose(viewMatrix);
	dataPtr->projection = XMMatrixTranspose(projMatrix);
	dataPtr->TexIndex = 0;
	m_effect->getConstantBuffer<ConstantMVPIndex>()->Unmap();
	m_effect->apply();

	for (auto& component : uiComponents) {
		component->DrawUI();
	}

	HR(m_pSwapChain->Present(1, 0));
#endif
}

void MainScene::cleanup()
{
	UIInputRouter::Instance().SetCharHandler({});
	UIDevOverlay::Instance().Unbind();
	if (m_navigation)
	{
		m_navigation->Shutdown();
		m_navigation.reset();
	}
	m_uifSystem.reset();
	m_uifContext.reset();
	m_uifEnabled = false;
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
