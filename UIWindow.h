
#pragma once
#include <fstream>
#include "UIBase.h"
class UIWindow : public UIBase
{
public:
	UIWindow();
	UIWindow(HINSTANCE _hInstance) :UIBase(_hInstance) {}
	~UIWindow() = default;


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
	ComPtr<ID3D11ShaderResourceView> window_textureArraySRV;
	ComPtr<ID3D11InputLayout> window_m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> window_m_pVertexBuffer;		// 顶点缓冲区
	std::fstream fs;

	ComPtr<ID3D11PixelShader> m_pTestPixelShader;	// 像素着色器

	float x = 0.0f;
	float y = 0.0f;
	float deltaX = 30.0f;
	float deltaY = 20.0f;

	float TitleHeight = 5.0f;

	std::wstring windowTitle;

	std::string TexPath;
	std::string TitleTexPath;
	std::string CloseTexPath;
	std::string MaxTexPath;
	std::string MinTexPath;
	std::string BodyTexPath;



	// 窗口标志变量
	bool isVisible = true;
	bool isResizable = false; //是否可调整大小
	bool isMaximized = false;
	bool isMinimized = false;


};

