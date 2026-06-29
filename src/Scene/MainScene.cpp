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
	if (!InitEffect())
		return false;

	if (!InitResource())
		return false;

	if (!InitUIF())
	{
		ERROR_("UIF 主菜单加载失败");
		return false;
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
	}
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
	}
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
