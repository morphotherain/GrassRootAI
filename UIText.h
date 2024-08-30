
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
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { x = _x, y = _y, deltaX = _deltaX, deltaY = _deltaY; }
	void setText(std::wstring _Text) { Text = _Text; };
	std::shared_ptr<bool> getClickFlag() { return clickFlag; }

private:
	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	ComPtr<ID3D11ShaderResourceView> Text_textureArraySRV;
	ComPtr<ID3D11InputLayout> Text_m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> Text_m_pVertexBuffer;		// 顶点缓冲区
	std::fstream fs;

	ComPtr<ID3D11PixelShader> m_pTestPixelShader;	// 像素着色器

	std::shared_ptr<bool> clickFlag;

	float x = 10.0f;
	float y = 30.0f;
	float deltaX = 20.0f;
	float deltaY = 4.0f;
	std::wstring Text;

};

