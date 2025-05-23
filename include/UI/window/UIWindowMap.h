﻿#pragma once
#include <fstream>
#include "UIWindow.h"
#include "UIButton.h"
#include "UIText.h"
#include "SolarSystem.h"

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
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	virtual void ParseParameters(std::unordered_map<std::string, std::any> paras);

	bool InitResource();
	bool InitEffect();
	bool InitMap();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY)
	{
		UIWindow::setSize(_x, _y, _deltaX, _deltaY);
	}

private:

	std::shared_ptr<Effect> m_mapLineEffect;
	std::shared_ptr<Effect> m_mapPointEffect;

	ComPtr<ID3D11InputLayout> m_pVertexLayoutPoints;	// 顶点输入布局
	ComPtr<ID3D11Buffer> m_pVertexBufferPoints;		// 顶点缓冲区

	ComPtr<ID3D11InputLayout> m_pVertexLayoutLines;	// 顶点输入布局
	ComPtr<ID3D11Buffer> m_pVertexBufferLines;		// 顶点缓冲区

	ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;   // D3D11设备上下文

	std::fstream fs;

	ComPtr<ID3D11PixelShader> m_pPixelShaderPoints;	// 像素着色器
	ComPtr<ID3D11VertexShader> m_pVertexShaderPoints;	// 顶点着色器
	ComPtr<ID3D11GeometryShader> m_pGeometryShader;	// 几何着色器

	ComPtr<ID3D11PixelShader> m_pPixelShaderLines;	// 像素着色器
	ComPtr<ID3D11VertexShader> m_pVertexShaderLines;	// 顶点着色器

	ComPtr<ID3D11BlendState> m_pBlendState;	// 混合状态对象

	std::wstring windowTitle;

	UINT numPoint = 0;
	UINT numLinePoint = 0;

	std::vector<SolarSystemData> m_solarSystem;
	std::vector<SolarSystemJump> m_solarSystemJump;
	std::vector<SolarSystemData> m_currentRegionSolarSystem;

	std::vector<std::shared_ptr<UIText>> regionTexts; // 用于存储星域名称文本对象
	std::vector<std::shared_ptr<UIText>> solarSystemTexts; // 用于存储星域名称文本对象
	std::vector<RegionData> m_regions; // 存储星域数据

	int m_viewType = 0;
	int m_viewTypeCooldown = 0;
	int temp = 100;

};
