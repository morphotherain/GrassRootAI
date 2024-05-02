#pragma once
#include <fstream>
#include "UIBase.h"
class UIShaderTest : public UIBase
{
public:
	UIShaderTest() = default;
	UIShaderTest(HINSTANCE _hInstance) :UIBase(_hInstance) {}
	~UIShaderTest() = default;

	struct VertexPosColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 TexCoor;
		float index;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
	};


	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { x = _x, y = _y, deltaX = _deltaX, deltaY = _deltaY; }
	void setTex(std::string _Tex) { TexPath = _Tex; };
private:
	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	ComPtr<ID3D11ShaderResourceView> button_textureArraySRV;
	ComPtr<ID3D11InputLayout> button_m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> button_m_pVertexBuffer;		// 顶点缓冲区
	std::fstream fs;


	// 创建常量缓冲区
	ComPtr<ID3D11Buffer> m_pTimeConstantBuffer;


	ComPtr<ID3D11PixelShader> m_pTestPixelShader;	// 像素着色器

	float x = 10.0f;
	float y = 30.0f;
	float deltaX = 20.0f;
	float deltaY = 4.0f;
	std::string TexPath;

	float currentTime = 0.0f;
};

