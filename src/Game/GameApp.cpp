#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include <vector>
#include <DirectXMath.h>
#include "dynGameObjectsManager.h"
#include "mapDenormalizeManager.h"
#include "InvTypesManager.h"
#include "AttributesComponent.h"
#include "WindowManager.h"
#include "RefiningSystem.h"
#include "HandlerFactory.h"
#include "SaveGameManager.h"

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

	// 如果是主菜单场景，注入“开始游戏”回调
	if (auto mainScene = dynamic_cast<MainScene*>(currentScene.get()))
	{
		mainScene->setStartGameCallback([this]() {
			this->StartNewGame();
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

void GameApp::UpdateScene(float dt)
{
	tick++;
	currentScene->UpdateScene(dt, *m_pMouse, *m_pKeyboard, tick);

	// 仅在进入游戏后才驱动 SolarSystem 等与存档相关的系统
	if (m_gameState == GameState::InGame)
	{
		int switchScene = 0;

		SolarSystemMgr::getInstance().Update(tick);
		TaskMgr::getInstance().distributeTasksFromTaskMgr();

		auto currentPilot = SolarSystemMgr::getInstance().currentPilot;
		if (currentPilot && currentPilot->currentShip)
		{
			UINT ContainerID = currentPilot->currentShip->GetComponent<BaseComponent>()->containerID;
			UINT solarSystemID = currentPilot->currentShip->GetComponent<BaseComponent>()->solarSystemID;
			UINT currentSolarSystemID = SolarSystemMgr::getInstance().currentSolarSystem->getSolarSystemID();
			bool needSwitch = solarSystemID != currentSolarSystemID;

			if (tick % 100 == 0 || needSwitch)
			{
				while (true)
				{
					if (currentSceneID == 4) {
						switchScene = 3;
						break;
					}

					if (solarSystemID != SolarSystemMgr::getInstance().currentSolarSystem->getSolarSystemID()) {
						if (currentSceneID != 4) {
							switchScene = 4;
						}
						auto nextSolarSystem = SolarSystemMgr::getInstance().currentSolarSystem;
						auto currentSolarSystem = SolarSystemMgr::getInstance().currentSolarSystem;
						auto it = SolarSystemMgr::getInstance().SolarSystems.find(solarSystemID);
						if (it != SolarSystemMgr::getInstance().SolarSystems.end()) {
							// 找到了对应的太阳系，获取其值
							nextSolarSystem = it->second;
						}
						else {
							nextSolarSystem.reset();
							nextSolarSystem = SolarSystemMgr::getInstance().loadSolarSystem(solarSystemID);
						}
						SolarSystemMgr::getInstance().currentSolarSystem = nextSolarSystem;
						SolarSystemMgr::getInstance().setCurrentPilot();
						currentSolarSystem->clearCurrentPilots();
						currentSolarSystem->clearCurrentSector();

						break;
					}

					if (ContainerID == 0) {
						if (currentSceneID != 3)
							switchScene = 3;
						break;
					}
					if (ContainerID != 0) {
						if (currentSceneID != 2)
							switchScene = 2;
						break;
					}
					break;
				}
			}

			switch (switchScene)
			{
			case 1:
			{
				SwitchToScene(std::make_unique<MainScene>(AppInst()));
				currentSceneID = 1;
				break;
			}
			case 2:
			{
				SwitchToScene(std::make_unique<DockScene>(AppInst()));
				currentSceneID = 2;
				break;
			}
			case 3:
			{
				SwitchToScene(std::make_unique<SpaceScene>(AppInst()));
				currentSceneID = 3;
				break;
			}
			case 4:
			{
				SwitchToScene(std::make_unique<StargateLoadingScene>(AppInst()));
				currentSceneID = 4;
				break;
			}
			default:;
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
	// 如果已经在游戏中，忽略重复请求
	if (m_gameState == GameState::InGame)
	{
		return;
	}

	INFO_("开始新游戏：从模板存档创建新存档并初始化游戏系统");

	// 1. 通过模板存档创建新存档文件，并在 saveSlots 中登记
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

	// 2. 按 slotID 加载存档（ATTACH + 创建 dyn 视图）
	if (!SaveGameManager::getInstance()->loadSaveBySlotID(slotID))
	{
		ERROR_("加载新存档失败，slotID = {}", slotID);
		return;
	}

	// 3. 现在 dyn* 表已经指向当前存档，可以安全地初始化各种依赖存档数据的管理器
	INFO_("初始化 AttributeMgr");
	AttributeMgr::getInstance().Init();

	INFO_("初始化 SolarSystemMgr");
	SolarSystemMgr::getInstance().Init();
	SolarSystemMgr::getInstance().getCurrentPilot();
	SolarSystemMgr::getInstance().setCurrentPilot();

	// 4. 注册各系统的任务处理器
	auto& taskMgr = TaskMgr::getInstance();
	auto& solarSystemMgr = SolarSystemMgr::getInstance();

	// 捕获局部引用而非静态调用
	taskMgr.registerSystemHandler(
		SystemType::NONE,
		[&solarSystemMgr](const std::shared_ptr<Task>& task) {
			solarSystemMgr.handleTask(*task);
		}
	);

	TaskMgr::getInstance().registerSystemHandler(
		SystemType::SOLAR_SYSTEM,
		[](const std::shared_ptr<Task>& task) {
			SolarSystemMgr::getInstance().handleTask(*task);
		}
	);

	TaskMgr::getInstance().registerSystemHandler(
		SystemType::UIWINDOW,
		[](const std::shared_ptr<Task>& task) {
			WindowManager::GetInstance().handleTask(*task);
		}
	);

	TaskMgr::getInstance().registerSystemHandler(
		SystemType::REFINING,
		[](const std::shared_ptr<Task>& task) {
			RefiningSystem::getInstance().handleTask(*task);
		}
	);

	// 初始化Handler（触发构造函数注册）
	HandlerFactory::initializeHandlers();

	// 5. 切换状态为“游戏中”，并进到 SpaceScene
	m_gameState = GameState::InGame;
	SwitchToScene(std::make_unique<SpaceScene>(AppInst()));
	currentSceneID = 3;
}