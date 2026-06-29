#pragma once

#include "UINode.h"
#include <dwrite.h>
#include <string>
#include <wrl/client.h>

class UITextNode : public UINode
{
public:
	const std::wstring& GetText() const { return m_text; }
	void SetText(const std::wstring& text) { m_text = text; }
	void SetTextUtf8(const std::string& text);

	float GetFontSize() const { return m_fontSize; }
	void SetFontSize(float fontSize);

	void Build(UIRenderContext& context) override;
	void Draw(UIRenderContext& context, float absX, float absY) override;

private:
	void RebuildTextFormat();

	std::wstring m_text;
	float m_fontSize = 24.0f;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;
};
