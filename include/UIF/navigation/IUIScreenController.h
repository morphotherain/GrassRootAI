#pragma once

#include "UIActionScope.h"

class UISystem;
class UIRenderContext;
struct UIInputState;

// 单个 UIF 屏幕的业务逻辑：Show/Hide 生命周期 + 可选输入扩展。
class IUIScreenController
{
public:
	virtual ~IUIScreenController() = default;

	virtual void OnShow(UISystem& system, UIRenderContext& context) = 0;
	virtual void OnHide() { ClearActions(); }
	virtual void OnUpdate(UIInputState& input) {}
	virtual bool OnChar(wchar_t ch) { return false; }

protected:
	void RegisterAction(const std::string& actionId, std::function<void()> handler)
	{
		m_actions.Register(actionId, std::move(handler));
	}

	void ClearActions()
	{
		m_actions.Clear();
	}

private:
	UIActionScope m_actions;
};
