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

	INFO_("初始化 AttributeMgr");
	AttributeMgr::getInstance().Init();

	INFO_("初始化 SolarSystemMgr");
	SolarSystemMgr::getInstance().Init();

	SolarSystemMgr::getInstance().getCurrentPilot();

	SolarSystemMgr::getInstance().setCurrentPilot();
	
	auto& taskMgr = TaskMgr::getInstance();
	auto& solarSystemMgr = SolarSystemMgr::getInstance();

	// 捕获局部引用而非静态调用
	taskMgr.registerSystemHandler(
		SystemType::NONE,
		[&solarSystemMgr](const std::shared_ptr<Task>& task) {
			solarSystemMgr.handleTask(*task); // 使用捕获的引用
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

	


	INFO_("切换到主界面");
	SwitchToScene(std::make_unique<SpaceScene>(AppInst()));

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
	int switchScene = 0;
	currentScene->UpdateScene(dt, *m_pMouse, *m_pKeyboard, tick);
	SolarSystemMgr::getInstance().Update(tick);
	TaskMgr::getInstance().distributeTasksFromTaskMgr();

	auto currentPilot = SolarSystemMgr::getInstance().currentPilot;
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
	return;
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