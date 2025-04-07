#include "WindowManager.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <memory>

#include "UIWindowInfo.h"
#include "UIWindowMap.h"
#include "UIWindowStorage.h"

using namespace DirectX;

WindowManager& WindowManager::GetInstance() {
	static WindowManager instance;
	return instance;
}

void WindowManager::Initialize(int width, int height, std::shared_ptr<Camera> camera) {
	DEBUG_("WindowManager初始化");

	DEBUG_("WindowManager 注册显示资源");
	m_clientWidth = width;
	m_clientHeight = height;
	m_camera = camera;

	RegisterWindowTypeT<UIWindowInfo>("info");
	RegisterWindowTypeT<UIWindowMap>("map");
	RegisterWindowTypeT<UIWindowStorage>("cargo");
	DEBUG_("WindowManager初始化完成");
}

void WindowManager::Update(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick) {
	Keyboard::State state = keyboard.GetState();
	m_KeyboardTracker.Update(state);

	// 检测 I 键按下
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::I)) {
		DEBUG_("打开info窗口");
		// 触发打开窗口
		WindowMessage msg;
		msg.windowType = "info";
		static int typeID = 520;
		msg.parameters["typeID"] = typeID++;    // int参数
		static float x = 0.0f;
		msg.parameters["x"] = (x += 100.0f);
		msg.parameters["z_order"] = (m_maxWindowZOrder++);
		WindowMessagingSystem::GetInstance().Enqueue(msg);
	}
	// 检测 M 键按下
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::M)) {
		DEBUG_("打开map窗口");
		// 触发打开窗口
		WindowMessage msg;
		msg.windowType = "map";
		msg.parameters["z_order"] = (m_maxWindowZOrder++);
		WindowMessagingSystem::GetInstance().Enqueue(msg);
	}
	// 检测 C 键按下
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::C)) {
		DEBUG_("打开cargo窗口");
		// 触发打开窗口
		WindowMessage msg;
		msg.windowType = "cargo";
		msg.parameters["z_order"] = (m_maxWindowZOrder++);
		WindowMessagingSystem::GetInstance().Enqueue(msg);
	}

	ProcessMessages();

	auto it = std::find(m_windows.begin(), m_windows.end(), m_activeWindow);
	if (it == m_windows.end() && m_windows.size() != 0)
		m_activeWindow = m_windows[0];

	std::shared_ptr<UIWindow> newActiveWindow = nullptr;
	for (auto& window : m_windows) {
		if (window == m_activeWindow)
			continue;
		if (window->IsActive()) {
			int z_order = window->GetZOrder();
			if (newActiveWindow && z_order < newActiveWindow->GetZOrder()) {
				continue;
			}
			else
			{
				newActiveWindow = window;
			}
		}
	}
	if (newActiveWindow != nullptr) {
		m_activeWindow = newActiveWindow;
		m_activeWindow->SetZOrder(m_maxWindowZOrder++);
	}
	for (auto& window : m_windows) {
		if (window == m_activeWindow)
			continue;
		window->SetActive(false);
	}

	// 使用反向迭代器更新窗口，允许在更新过程中修改窗口列表
	for (auto it = m_windows.rbegin(); it != m_windows.rend(); ++it)
	{
		auto& window = *it;

		window->UpdateUI(dt, mouse, keyboard, tick); // 更新窗口逻辑

		// 处理窗口关闭请求
		if (window->ShouldClose())
		{
			// 将需要关闭的窗口移到列表末尾
			auto forward_it = std::next(it).base();
			std::rotate(forward_it, forward_it + 1, m_windows.end());
		}
	}

	// 移除所有标记关闭的窗口
	m_windows.erase(
		std::remove_if(m_windows.begin(), m_windows.end(),
			[](const std::shared_ptr<UIWindow>& w) { return w->ShouldClose(); }),
		m_windows.end()
	);
}

void WindowManager::Draw()
{
	// 按z-order排序
	std::stable_sort(m_windows.begin(), m_windows.end(),
		[](const std::shared_ptr<UIWindow>& a, const std::shared_ptr<UIWindow>& b) {
			return a->GetZOrder() < b->GetZOrder();
		});

	// 遍历绘制所有可见窗口
	for (auto& window : m_windows)
	{
		if (window->IsVisible())
		{
			window->DrawUI();
		}
	}
}

void WindowManager::RegisterWindowType(const std::string& typeName,
	std::function<std::shared_ptr<UIWindow>()> factory) {
	DEBUG_("WindowManager注册");
	m_factories[typeName] = factory;
}

void WindowManager::AddWindow(std::shared_ptr<UIWindow> window) {
	window->setcameraResource(m_clientWidth, m_clientHeight, m_camera);
	window->Init();
	m_windows.push_back(window);
}

WindowManager::WindowManager()
{
	m_factories = std::unordered_map<std::string, std::function<std::shared_ptr<UIWindow>()>>();
}

void WindowManager::ProcessMessages() {
	WindowMessage msg;
	while (WindowMessagingSystem::GetInstance().Dequeue(msg)) {
		auto it = m_factories.find(msg.windowType);
		if (it != m_factories.end()) {
			DEBUG_("找到窗口类型 {} 对应的工厂函数，开始创建窗口", msg.windowType);
			auto window = it->second();
			window->ParseParameters(msg.parameters);
			m_activeWindow = window;
			AddWindow(window);
		}
	}
}