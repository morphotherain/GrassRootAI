
#pragma once
#include <fstream>
#include "UIBase.h"
class UISkyBox : public UIBase
{
public:
	UISkyBox() = default;
	UISkyBox(HINSTANCE _hInstance) :UIBase(_hInstance) {}
	~UISkyBox() = default;



	// 定义常量缓冲区结构
	/*struct MatrixBufferType
	{
		DirectX::XMMATRIX g_WorldViewProj;
	};*/

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { x = _x, y = _y, deltaX = _deltaX, deltaY = _deltaY; }
	void setTex(std::string _Tex) { TexPath = _Tex; };
	void setCamera(std::shared_ptr<Camera> _camera) { m_pCamera = (_camera); }
	std::shared_ptr<bool> getClickFlag() { return clickFlag; }

private:
	std::shared_ptr<Effect> m_skyboxEffect;

	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	ComPtr<ID3D11ShaderResourceView> button_textureArraySRV;
	ComPtr<ID3D11InputLayout> button_m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> button_m_pVertexBuffer;		// 顶点缓冲区
	std::fstream fs;


	ComPtr<ID3D11PixelShader> m_pPixelShaderSky;	// 像素着色器
	ComPtr<ID3D11VertexShader> m_pVertexShaderSky;	// 顶点着色器


	std::shared_ptr<bool> clickFlag;

	float x = 10.0f;
	float y = 30.0f;
	float deltaX = 20.0f;
	float deltaY = 4.0f;
	std::string TexPath;

	std::vector<VertexPos> GenerateButtonVertices();
};

