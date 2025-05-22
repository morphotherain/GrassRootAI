#include "WindowManager.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <memory>

#include "UIWindowInfo.h"
#include "UIWindowMap.h"
#include "UIWindowStorage.h"
#include "UIWindowEquipment.h"
#include "UIWindowMarket.h"

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
	RegisterWindowTypeT<UIWindowEquipment>("equipment");
	RegisterWindowTypeT<UIWindowMarket>("market");
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
	// 检测 X 键按下
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::X)) {
		DEBUG_("打开equipment窗口");
		// 触发打开窗口
		WindowMessage msg;
		msg.windowType = "equipment";
		msg.parameters["z_order"] = (m_maxWindowZOrder++);
		WindowMessagingSystem::GetInstance().Enqueue(msg);
	}
	// 检测 1 键按下
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::NumPad1)) {
		DEBUG_("打开market窗口");
		// 触发打开窗口
		WindowMessage msg;
		msg.windowType = "market";
		msg.parameters["z_order"] = (m_maxWindowZOrder++);
		WindowMessagingSystem::GetInstance().Enqueue(msg);
	}

	ProcessMessages();

	// 查找活动窗口
	std::shared_ptr<UIWindow> activeWindow = nullptr;
	for (auto& [id, window] : m_windows) {
		if (window == m_activeWindow) {
			activeWindow = window;
			break;
		}
	}

	// 如果没找到活动窗口且map不为空，就将第一个窗口设为活动窗口
	if (!activeWindow && !m_windows.empty()) {
		m_activeWindow = m_windows.begin()->second;
	}

	std::shared_ptr<UIWindow> newActiveWindow = nullptr;
	for (auto& [id, window] : m_windows) {
		if (window == m_activeWindow && !window->IsNowClickOn())
			continue;
		if (window->IsNowClickOn()) {
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
	for (auto& [id, window] : m_windows) {
		window->SetNowClickOn(false);
		window->SetActive(window == m_activeWindow);
	}

	// 使用正向迭代器更新窗口，并收集需要关闭的窗口ID
	std::vector<std::string> windowsToClose; // 假设WindowID是unordered_map的键类型

	for (auto& pair : m_windows)
	{
		auto& window = pair.second;

		window->UpdateUI(dt, mouse, keyboard, tick); // 更新窗口逻辑

		// 收集需要关闭的窗口ID
		if (window->ShouldClose())
		{
			windowsToClose.push_back(pair.first);
		}
	}

	// 移除所有标记关闭的窗口
	for (const auto& id : windowsToClose)
	{
		m_windows.erase(id);
	}
}

void WindowManager::Draw()
{
	// 把窗口指针提取到一个vector中
	std::vector<std::shared_ptr<UIWindow>> windowList;
	windowList.reserve(m_windows.size());
	for (auto& [id, window] : m_windows) {
		windowList.push_back(window);
	}

	// 对窗口指针vector按z-order排序
	std::stable_sort(windowList.begin(), windowList.end(),
		[](const std::shared_ptr<UIWindow>& a, const std::shared_ptr<UIWindow>& b) {
			return a->GetZOrder() < b->GetZOrder();
		});

	// 遍历绘制所有可见窗口
	for (auto& window : windowList)
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

void WindowManager::AddWindow(std::shared_ptr<UIWindow> window, std::string windowType) {
	window->setcameraResource(m_clientWidth, m_clientHeight, m_camera);
	window->Init();
	m_windows[windowType] = (window);
}

void WindowManager::handleTask(Task& task)
{

	// 从任务中获取目标坐标（假设参数为浮点数，若为整数需强制转换）
	int taskX = task.getParamOrDefault<int>("x", 0);
	int taskY = task.getParamOrDefault<int>("y", 0);

	std::shared_ptr<UIWindow> targetWindow = nullptr;
	int maxZOrder = -1; // 初始化为最小值，确保第一个符合条件的窗口会被选中

	// 遍历所有窗口，查找符合条件的窗口
	for (auto& [id, window] : m_windows) {
		// 获取窗口尺寸和位置（假设 size 格式为 [x, y, width, height]）
		auto size = window->GetSize();
		float windowX = size[0];
		float windowY = size[1];
		float windowWidth = size[2];
		float windowHeight = size[3];

		// 计算窗口区域范围（左上角到右下角）
		float minX = windowX;
		float maxX = windowX + windowWidth;
		float minY = windowY;
		float maxY = windowY + windowHeight;

		// 判断点是否在窗口区域内（包含边界）
		bool isInRange = (taskX >= minX && taskX <= maxX &&
			taskY >= minY && taskY <= maxY);

		if (isInRange) {
			int currentZOrder = window->GetZOrder();
			// 比较 ZOrder，保留最大的窗口
			if (currentZOrder > maxZOrder) {
				maxZOrder = currentZOrder;
				targetWindow = window;
			}
		}
	}

	// 若找到符合条件的窗口，转发任务
	if (targetWindow) {
		targetWindow->handleTask(task);
	}
}

WindowManager::WindowManager()
{
	m_factories = std::unordered_map<std::string, std::function<std::shared_ptr<UIWindow>()>>();
}

void WindowManager::ProcessMessages() {
	WindowMessage msg;
	while (WindowMessagingSystem::GetInstance().Dequeue(msg)) {
		if (m_windows[msg.windowType]) {
			m_windows.erase(msg.windowType);
		}
		else {
			auto it = m_factories.find(msg.windowType);
			if (it != m_factories.end()) {
				DEBUG_("找到窗口类型 {} 对应的工厂函数，开始创建窗口", msg.windowType);
				auto window = it->second();
				window->ParseParameters(msg.parameters);
				m_activeWindow = window;
				AddWindow(window, msg.windowType);
			}
		}
	}
}