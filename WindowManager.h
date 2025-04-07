#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include "UIWindow.h"
#include "WindowMessagingSystem.h"

class Camera;

class WindowManager {
public:
	static WindowManager& GetInstance();

	void Initialize(int width, int height, std::shared_ptr<Camera> camera);
	void Update(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	void Draw();
	void RegisterWindowType(const std::string& typeName,
		std::function<std::shared_ptr<UIWindow>()> factory);

	template<typename T>
	void RegisterWindowTypeT(const std::string& typeName);

	void AddWindow(std::shared_ptr<UIWindow> window);

private:
	WindowManager();
	WindowManager(const WindowManager&) = delete;
	WindowManager& operator=(const WindowManager&) = delete;

	void ProcessMessages();

	int m_clientWidth;
	int m_clientHeight;
	int m_maxWindowZOrder = 0;
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<UIWindow> m_activeWindow;
	std::vector<std::shared_ptr<UIWindow>> m_windows;
	std::unordered_map<std::string, std::function<std::shared_ptr<UIWindow>()>> m_factories;

	DirectX::Mouse::ButtonStateTracker m_MouseTracker;			// 鼠标状态追踪器
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;	// 键盘状态追踪器
};

template<typename T>
void WindowManager::RegisterWindowTypeT(const std::string& typeName) {
	WindowManager::GetInstance().RegisterWindowType(typeName,
		[] { return std::make_shared<T>(); });
}