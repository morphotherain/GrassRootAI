
#pragma once
#include <fstream>
#include "UIWindow.h"
class UIWindowMap : public UIWindow
{
public:
	UIWindowMap();
	UIWindowMap(HINSTANCE _hInstance) :UIWindow(_hInstance) {}
	~UIWindowMap() = default;


	struct PointVertexPosColor
	{
		DirectX::XMFLOAT3 pos;            // 顶点位置
		DirectX::XMFLOAT4 color;          // 颜色 (RGBA)，16字节对齐
		float luminosity;                 // 光度

		// 输入布局描述
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
	};

	struct LineVertexPosColor
	{
		DirectX::XMFLOAT3 pos;            // 顶点位置，12 字节
		DirectX::XMFLOAT4 color;          // 颜色 (RGBA)，16 字节

		// 输入布局描述
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
	};


	// 更新后的 MatrixBufferType 结构
	struct MatrixBufferType
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMFLOAT3 cameraPosition; // 添加摄像机位置
		float padding;                    // 用于填充，使得常量缓冲区对齐
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

	ComPtr<ID3D11InputLayout> m_pVertexLayoutPoints;	// 顶点输入布局
	ComPtr<ID3D11Buffer> m_pVertexBufferPoints;		// 顶点缓冲区

	ComPtr<ID3D11InputLayout> m_pVertexLayoutLines;	// 顶点输入布局
	ComPtr<ID3D11Buffer> m_pVertexBufferLines;		// 顶点缓冲区

	std::fstream fs;

	ComPtr<ID3D11PixelShader> m_pPixelShaderPoints;	// 像素着色器
	ComPtr<ID3D11VertexShader> m_pVertexShaderPoints;	// 顶点着色器
	ComPtr<ID3D11GeometryShader> m_pGeometryShader;	// 几何着色器

	ComPtr<ID3D11PixelShader> m_pPixelShaderLines;	// 像素着色器
	ComPtr<ID3D11VertexShader> m_pVertexShaderLines;	// 顶点着色器


	ComPtr<ID3D11BlendState> m_pBlendState;	// 混合状态对象

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

	UINT numPoint;
	UINT numLinePoint;



};

