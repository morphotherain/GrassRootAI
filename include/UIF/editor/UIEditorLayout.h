#pragma once

namespace UIEditorLayout
{
	constexpr float kToolbarH = 72.0f;
	constexpr float kTreePanelW = 300.0f;
	constexpr float kPropertyPanelH = 340.0f;
	constexpr float kFramePaletteH = 250.0f;
	constexpr float kTreeRowH = 20.0f;
	constexpr const char* kDefaultAtlasId = "ui_window";

	inline float GetFramePaletteTop(int clientHeight)
	{
		return static_cast<float>(clientHeight) - kPropertyPanelH - kFramePaletteH;
	}

	inline bool IsInToolbar(float x, float y)
	{
		(void)x;
		return y < kToolbarH;
	}

	inline bool IsInFramePalette(float x, float y, int clientHeight)
	{
		const float top = GetFramePaletteTop(clientHeight);
		return x < kTreePanelW && y >= top && y < top + kFramePaletteH;
	}

	inline bool IsInTreePanel(float x, float y, int clientHeight)
	{
		return x < kTreePanelW
			&& y >= kToolbarH
			&& y < GetFramePaletteTop(clientHeight);
	}

	inline bool IsInPropertyPanel(float y, int clientHeight)
	{
		return y >= static_cast<float>(clientHeight) - kPropertyPanelH;
	}
}
