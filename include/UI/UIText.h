#pragma once
#include <fstream>
#include "UIBase.h"
class UIText : public UIBase
{
public:
	UIText() = default;
	UIText(HINSTANCE _hInstance) :UIBase(_hInstance) {}
	~UIText() = default;

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _width, const float _height)
	{
		x = _x, y = _y, width = _width, height = _height;
	}
	void setPosition(const float _x, const float _y) { x = _x, y = _y; }
	void setText(std::wstring _Text) { Text = _Text; };
	
	void setClipRect(const D2D1_RECT_F& clipRect) {
		clipRectangle = clipRect;
	}


	void switchTextFormat(std::string index);

	std::fstream fs;
	ComPtr<ID2D1RenderTarget> m_pd2dRenderTarget;
	ComPtr<ID2D1SolidColorBrush> m_pColorBrush;
	ComPtr<IDWriteTextFormat> m_pTextFormat;

	float x = 10.0f;
	float y = 30.0f;
	float width = 20.0f;
	float height = 4.0f;
	std::wstring Text;
	D2D1_RECT_F clipRectangle = { 0, 0, 1920, 1080 }; // 用于存储裁剪区域
};
