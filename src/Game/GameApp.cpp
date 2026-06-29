#include "GameApp.h"
#include "SceneTransitionService.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include <vector>
#include <DirectXMath.h>
#include "dynGameObjectsManager.h"
#include "mapDenormalizeManager.h"
#include "InvTypesManager.h"
#include "AttributesComponent.h"
#include "BaseComponent.h"
#include "WindowManager.h"
#include "RefiningSystem.h"
#include "HandlerFactory.h"
#include "SaveGameManager.h"
#include "TaskMgr.h"
#include "logger_manager.h"

using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance),
	m_CameraMode(CameraMode::Free)
{
}

GameApp::~GameApp()
{
}

void GameApp::SwitchToScene(std::unique_ptr<Scene> newScene) {
	if (currentScene) {
		currentScene->cleanup();
	}

	currentScene = std::move(newScene);

	// 如果是主菜单场景，注入存档 UI 回调
	if (auto mainScene = dynamic_cast<MainScene*>(currentScene.get()))
	{
		UIGameplayHost host;
		host.onQuickStartNewGame = [this]() {
			this->RequestStartNewGame();
		};
		host.onEnterGameFromSlot = [this](int slotId) {
			this->RequestEnterGameFromSlot(slotId);
		};
		host.onCreateAndEnterGame = [this](const std::string& displayName) {
			this->RequestCreateAndEnterGame(displayName);
		};
		host.onDeleteSaveSlot = [](int slotId) {
			return SaveGameManager::getInstance()->deleteSaveSlot(slotId);
		};
		mainScene->SetGameplayHost(std::move(host));
	}

	// 游戏中场景：注入返回主菜单回调（DEV：F10）
	if (auto spaceScene = dynamic_cast<SpaceScene*>(currentScene.get()))
	{
		spaceScene->setReturnToMainMenuCallback([this]() {
			this->RequestReturnToMainMenu();
		});
	}

	if (currentScene) {
		currentScene->setd3dResource(
			*m_pd3dDevice1.GetAddressOf(),
			*m_pd3dImmediateContext1.GetAddressOf(),
			*m_pSwapChain1.GetAddressOf(),
			MainWnd(),
			*m_pRenderTargetView.GetAddressOf(),
			*m_pDepthStencilView.GetAddressOf()
		);
		currentScene->setcameraResource(m_ClientWidth, m_ClientHeight, m_pCamera);
		currentScene->setd2dResource(*m_pd2dRenderTarget.GetAddressOf(), *m_pColorBrush.GetAddressOf(), *m_pTextFormat.GetAddressOf());
		currentScene->Init();
	}
}

bool GameApp::Init()
{
	INFO_("初始化 GameApp");

	INFO_("初始化 D3DApp");
	if (!D3DApp::Init())
	{
		ERROR_("D3DApp 初始化失败");
		return false;
	}

	INFO_("初始化 InitEffect");
	if (!InitEffect())
	{
		ERROR_("InitEffect 初始化失败");
		return false;
	}

	INFO_("初始化 InitResource");
	if (!InitResource())
	{
		ERROR_("InitResource 初始化失败");
		return false;
	}

	// 初始化鼠标，键盘不需要
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	// 此时还未加载或创建任何存档，仅进入主菜单界面。
	m_gameState = GameState::MainMenu;

	INFO_("切换到主界面");
	SwitchToScene(std::make_unique<MainScene>(AppInst()));
	m_currentSceneId = SceneId::Main;

	INFO_("GameApp 初始化完成");
	return true;
}

void GameApp::OnResize()
{
	assert(m_pd2dFactory);
	assert(m_pdwriteFactory);
	// 释放D2D的相关资源
	m_pColorBrush.Reset();
	m_pd2dRenderTarget.Reset();

	D3DApp::OnResize();

	// 为D2D创建DXGI表面渲染目标
	ComPtr<IDXGISurface> surface;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, m_pd2dRenderTarget.GetAddressOf());
	surface.Reset();

	if (hr == E_NOINTERFACE)
	{
		OutputDebugString(L"\n警告：Direct2D与Direct3D互操作性功能受限，你将无法看到文本信息。现提供下述可选方法：\n"
			"1. 对于Win7系统，需要更新至Win7 SP1，并安装KB2670838补丁以支持Direct2D显示。\n"
			"2. 自行完成Direct3D 10.1与Direct2D的交互。详情参阅："
			"https://docs.microsoft.com/zh-cn/windows/desktop/Direct2D/direct2d-and-direct3d-interoperation-overview""\n"
			"3. 使用别的字体库，比如FreeType。\n\n");
	}
	else if (hr == S_OK)
	{
		// 创建固定颜色刷和文本格式
		HR(m_pd2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			m_pColorBrush.GetAddressOf()));

		HR(m_pdwriteFactory->CreateTextFormat(L"黑体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12, L"zh-cn",
			m_pTextFormat.GetAddressOf()));
		D3DManager::getInstance().addTextFormat(m_pTextFormat.Get(), "Bold_XS");

		HR(m_pdwriteFactory->CreateTextFormat(L"黑体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 30, L"zh-cn",
			m_pTextFormat.GetAddressOf()));
		D3DManager::getInstance().addTextFormat(m_pTextFormat.Get(), "Bold_XL");

		HR(m_pdwriteFactory->CreateTextFormat(L"黑体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 25, L"zh-cn",
			m_pTextFormat.GetAddressOf()));
		D3DManager::getInstance().addTextFormat(m_pTextFormat.Get(), "Bold_L");

		HR(m_pdwriteFactory->CreateTextFormat(L"黑体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20, L"zh-cn",
			m_pTextFormat.GetAddressOf()));
		D3DManager::getInstance().addTextFormat(m_pTextFormat.Get(), "Bold_M");

		HR(m_pdwriteFactory->CreateTextFormat(L"黑体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn",
			m_pTextFormat.GetAddressOf()));
		D3DManager::getInstance().addTextFormat(m_pTextFormat.Get(), "Bold_S");
	}
	else
	{
		// 报告异常问题
		assert(m_pd2dRenderTarget);
	}

	D3DManager::getInstance().setd2dResource(m_pd2dRenderTarget.Get(), m_pColorBrush.Get(), m_pTextFormat.Get());
}

void GameApp::SwitchToSceneId(SceneId sceneId)
{
	switch (sceneId)
	{
	case SceneId::Main:
		SwitchToScene(std::make_unique<MainScene>(AppInst()));
		m_currentSceneId = SceneId::Main;
		break;
	case SceneId::Dock:
		SwitchToScene(std::make_unique<DockScene>(AppInst()));
		m_currentSceneId = SceneId::Dock;
		break;
	case SceneId::Space:
		SwitchToScene(std::make_unique<SpaceScene>(AppInst()));
		m_currentSceneId = SceneId::Space;
		break;
	case SceneId::StargateLoading:
		SwitchToScene(std::make_unique<StargateLoadingScene>(AppInst()));
		m_currentSceneId = SceneId::StargateLoading;
		break;
	default:
		break;
	}
}

void GameApp::UpdateScene(float dt)
{
	tick++;
	currentScene->UpdateScene(dt, *m_pMouse, *m_pKeyboard, tick);
	ProcessDeferredActions();

	// 仅在进入游戏后才驱动 SolarSystem 等与存档相关的系统
	if (m_gameState == GameState::InGame)
	{
		SolarSystemMgr::getInstance().Update(tick);
		TaskMgr::getInstance().distributeTasksFromTaskMgr();

		auto currentPilot = SolarSystemMgr::getInstance().currentPilot;
		if (currentPilot && currentPilot->currentShip)
		{
			auto* base = currentPilot->currentShip->GetComponent<BaseComponent>();
			if (base && SolarSystemMgr::getInstance().currentSolarSystem)
			{
				SceneTransitionContext ctx{
					m_currentSceneId,
					tick,
					base->containerID,
					base->solarSystemID,
					SolarSystemMgr::getInstance().currentSolarSystem->getSolarSystemID()
				};
				if (auto nextScene = SceneTransitionService::EvaluateTransition(ctx))
				{
					SwitchToSceneId(*nextScene);
				}
			}
		}
	}
}

void GameApp::DrawScene()
{
	currentScene->DrawScene();
	return;
}

bool GameApp::InitEffect()
{
	return true;
}

bool GameApp::InitResource()
{
	return true;
}

void GameApp::StartNewGame()
{
	if (m_gameState == GameState::InGame)
	{
		return;
	}

	INFO_("开始新游戏：从模板存档创建新存档并初始化游戏系统");

	int slotID = -1;
	if (!SaveGameManager::getInstance()->createNewSaveFromTemplate(
		"save/initial",
		"新建存档",
		"dev",
		slotID))
	{
		ERROR_("创建新存档失败");
		return;
	}

	if (!EnterGameFromSlot(slotID))
	{
		ERROR_("进入新游戏失败，slotID = {}", slotID);
	}
}

bool GameApp::EnterGameFromSlot(int slotID)
{
	if (m_gameState == GameState::InGame)
	{
		ReturnToMainMenu();
	}

	m_gameState = GameState::Loading;

	if (!SaveGameManager::getInstance()->loadSaveBySlotID(slotID))
	{
		ERROR_("加载存档失败，slotID = {}", slotID);
		m_gameState = GameState::MainMenu;
		return false;
	}

	InitializeGameSystems();

	m_gameState = GameState::InGame;
	SwitchToScene(std::make_unique<SpaceScene>(AppInst()));
	m_currentSceneId = SceneId::Space;
	INFO_("已进入游戏，slotID = {}", slotID);
	return true;
}

void GameApp::ReturnToMainMenu()
{
	if (m_gameState == GameState::MainMenu)
	{
		return;
	}

	INFO_("返回主菜单");

	WindowManager::GetInstance().Reset();
	SolarSystemMgr::getInstance().Shutdown();
	TaskMgr::getInstance().ResetRuntime();

	if (!SaveGameManager::getInstance()->detachCurrentSaveDatabase())
	{
		ERROR_("分离存档数据库失败");
	}

	m_gameState = GameState::MainMenu;
	tick = 0;
	SwitchToScene(std::make_unique<MainScene>(AppInst()));
	m_currentSceneId = SceneId::Main;
}

void GameApp::InitializeGameSystems()
{
	INFO_("初始化 AttributeMgr");
	AttributeMgr::getInstance().Init();

	INFO_("初始化 SolarSystemMgr");
	SolarSystemMgr::getInstance().Init();
	SolarSystemMgr::getInstance().getCurrentPilot();
	SolarSystemMgr::getInstance().setCurrentPilot();

	auto& taskMgr = TaskMgr::getInstance();
	auto& solarSystemMgr = SolarSystemMgr::getInstance();

	taskMgr.registerSystemHandler(
		SystemType::NONE,
		[&solarSystemMgr](const std::shared_ptr<Task>& task) {
			solarSystemMgr.handleTask(*task);
		}
	);

	taskMgr.registerSystemHandler(
		SystemType::SOLAR_SYSTEM,
		[](const std::shared_ptr<Task>& task) {
			SolarSystemMgr::getInstance().handleTask(*task);
		}
	);

	taskMgr.registerSystemHandler(
		SystemType::UIWINDOW,
		[](const std::shared_ptr<Task>& task) {
			WindowManager::GetInstance().handleTask(*task);
		}
	);

	taskMgr.registerSystemHandler(
		SystemType::REFINING,
		[](const std::shared_ptr<Task>& task) {
			RefiningSystem::getInstance().handleTask(*task);
		}
	);

	HandlerFactory::initializeHandlers();
}

void GameApp::RequestStartNewGame()
{
	m_pendingStartNewGame = true;
}

void GameApp::RequestReturnToMainMenu()
{
	m_pendingReturnToMainMenu = true;
}

void GameApp::RequestEnterGameFromSlot(int slotID)
{
	m_pendingSlotID = slotID;
	m_pendingEnterGameFromSlot = true;
}

void GameApp::RequestCreateAndEnterGame(const std::string& displayName)
{
	m_pendingDisplayName = displayName;
	m_pendingCreateAndEnterGame = true;
}

void GameApp::ProcessDeferredActions()
{
	if (m_pendingReturnToMainMenu)
	{
		m_pendingReturnToMainMenu = false;
		ReturnToMainMenu();
		return;
	}

	if (m_pendingEnterGameFromSlot)
	{
		m_pendingEnterGameFromSlot = false;
		const int slotID = m_pendingSlotID;
		if (!EnterGameFromSlot(slotID))
		{
			if (auto* mainScene = dynamic_cast<MainScene*>(currentScene.get()))
			{
				mainScene->SetNavigationBusy(false);
			}
		}
		return;
	}

	if (m_pendingCreateAndEnterGame)
	{
		m_pendingCreateAndEnterGame = false;
		const std::string displayName = m_pendingDisplayName;
		m_pendingDisplayName.clear();

		int slotID = -1;
		if (!SaveGameManager::getInstance()->createNewSaveFromTemplate(
			"save/initial",
			displayName.empty() ? "新建存档" : displayName,
			"dev",
			slotID))
		{
			ERROR_("创建新存档失败");
			if (auto* mainScene = dynamic_cast<MainScene*>(currentScene.get()))
			{
				mainScene->SetNavigationBusy(false);
			}
			return;
		}

		if (!EnterGameFromSlot(slotID))
		{
			ERROR_("进入新游戏失败，slotID = {}", slotID);
			if (auto* mainScene = dynamic_cast<MainScene*>(currentScene.get()))
			{
				mainScene->SetNavigationBusy(false);
			}
		}
		return;
	}

	if (m_pendingStartNewGame)
	{
		m_pendingStartNewGame = false;
		StartNewGame();
	}
}