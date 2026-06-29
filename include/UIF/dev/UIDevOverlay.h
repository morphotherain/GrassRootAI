#pragma once

#include "UIInputState.h"
#include <Keyboard.h>
#include <memory>
#include <string>

class UISystem;
class UIRenderContext;
class UIEditorSession;

// 游戏内开发 overlay（ImGui 替代）：F11 切换，编辑模式下吞掉 gameplay UI 输入。
class UIDevOverlay
{
public:
	static UIDevOverlay& Instance();

	void BindUISystem(UISystem* system, const std::string& documentPath);
	void Unbind();

	bool IsActive() const;
	bool ConsumesInput() const { return IsActive(); }

	void Toggle();
	void SetStats(float fps, float frameMs);

	void HandleGlobalKeys(DirectX::Keyboard::KeyboardStateTracker& keyboardTracker);
	void HandleEditorKeys(DirectX::Keyboard::KeyboardStateTracker& keyboardTracker,
		const DirectX::Keyboard::State& keyState);
	bool OnChar(wchar_t ch);
	void UpdateInput(UIInputState& input);
	void Draw(UIRenderContext& context);

private:
	UIDevOverlay();
	~UIDevOverlay() = default;

	UIDevOverlay(const UIDevOverlay&) = delete;
	UIDevOverlay& operator=(const UIDevOverlay&) = delete;

	std::unique_ptr<UIEditorSession> m_uiEditor;
	float m_fps = 0.0f;
	float m_frameMs = 0.0f;
};
