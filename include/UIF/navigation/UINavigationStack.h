#pragma once

#include "IUIScreenController.h"
#include "UIDocument.h"
#include <memory>
#include <string>
#include <vector>

class UISystem;
class UIRenderContext;
struct UIInputState;

// 栈式 UI 导航：Push/Pop 切换 JSON 屏幕，Esc 返回上一级。
class UINavigationStack
{
public:
	void Initialize(UISystem* system, UIRenderContext* context, int clientWidth, int clientHeight);
	void Shutdown();

	void ReplaceRoot(const std::string& documentPath, std::unique_ptr<IUIScreenController> controller);
	void Push(const std::string& documentPath, std::unique_ptr<IUIScreenController> controller);
	bool Pop();

	bool HandleBack();
	void Update(UIInputState& input);
	bool OnChar(wchar_t ch);

	void SetBusy(bool busy);
	bool IsBusy() const { return m_busy; }

	int Depth() const { return static_cast<int>(m_stack.size()); }
	const std::string& TopDocumentPath() const;

	void OnClientResize(int clientWidth, int clientHeight);

private:
	struct ScreenEntry
	{
		std::string documentPath;
		std::shared_ptr<UIDocument> document;
		std::unique_ptr<IUIScreenController> controller;
	};

	std::shared_ptr<UIDocument> LoadScaledDocument(const std::string& path);
	void ApplyTopScreen();
	void RegisterNavigationActions();
	void SetOverlayPath(const std::string& path);
	void UpdateBusyOverlay();

	UISystem* m_system = nullptr;
	UIRenderContext* m_context = nullptr;
	int m_clientWidth = 0;
	int m_clientHeight = 0;
	bool m_busy = false;
	std::vector<ScreenEntry> m_stack;
};
