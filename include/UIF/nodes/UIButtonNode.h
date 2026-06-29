#pragma once

#include "UISpriteNode.h"
#include <string>

class UIButtonNode : public UISpriteNode
{
public:
	const std::wstring& GetLabel() const { return m_label; }
	void SetLabel(const std::wstring& label) { m_label = label; }
	void SetLabelUtf8(const std::string& label);

	const std::string& GetActionId() const { return m_actionId; }
	void SetActionId(const std::string& actionId) { m_actionId = actionId; }

	bool Update(UIInputState& input, UIRenderContext& context, float absX, float absY) override;
	void Draw(UIRenderContext& context, float absX, float absY) override;

private:
	std::wstring m_label;
	std::string m_actionId;
	bool m_hovered = false;
	bool m_pressed = false;
};
