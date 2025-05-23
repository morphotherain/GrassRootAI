﻿#pragma once
#include "d3dUtil.h"
#include "DXTrace.h"
#include <vector>
#include <DirectXMath.h>
#include "Camera.h"
#include "ddsLoader.h"
#include "RenderProxy.h"

class UIBase;
class GameObject;

class Scene
{
public:
	Scene() = default;
	Scene(HINSTANCE hInstance);
	~Scene() = default;

	virtual bool Init() = 0;
	virtual void OnResize() = 0;
	virtual void UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick) = 0;
	virtual void DrawScene() = 0;
	virtual void cleanup() = 0;
	virtual void AddUIComponent(std::shared_ptr<UIBase> component) {
		uiComponents.push_back(component);
	}
	virtual void RemoveUIComponent(std::shared_ptr<UIBase> component) {
		// 从uiComponents中移除指定的UI组件
	}

	virtual void AddGameObject(std::shared_ptr<GameObject> object) {
		GameObjects.push_back(object);
	}
	virtual void RemoveGameObject(std::shared_ptr<GameObject> object) {
		// 从GameObjects中移除指定的object
	}

	void setd3dResource(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, IDXGISwapChain* pSwapChain, HWND hMainWnd, ID3D11RenderTargetView* pRenderTargetView, ID3D11DepthStencilView* m_pDepthStencilView);
	void setd2dResource(ID2D1RenderTarget* pd2dRenderTarget, ID2D1SolidColorBrush* pColorBrush, IDWriteTextFormat* pTextFormat);
	void setcameraResource(int m_ClientWidth, int m_ClientHeight, std::shared_ptr<Camera> pCamera);

	enum class CameraMode { FirstPerson, ThirdPerson, Free };
	float AspectRatio()const { return static_cast<float>(m_ClientWidth) / m_ClientHeight; }

protected:

	HWND      m_hMainWnd;        // 主窗口句柄
	HINSTANCE hInstance;

	ddsLoader m_ddsLoader;                                //材质加载

	CameraMode m_CameraMode;									// 摄像机模式
	std::shared_ptr<Camera> m_pCamera;						    // 摄像机
	std::shared_ptr<Camera> m_pLocalCamera;						    // 摄像机

	ComPtr<ID3D11Device> m_pd3dDevice;
	ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;   // D3D11设备上下文
	ComPtr<IDXGISwapChain> m_pSwapChain;                  // D3D11交换链

	ComPtr<ID2D1RenderTarget> m_pd2dRenderTarget;
	ComPtr<ID2D1SolidColorBrush> m_pColorBrush;
	ComPtr<IDWriteTextFormat> m_pTextFormat;

	// 常用资源
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;        // 深度模板缓冲区
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;   // 渲染目标视图
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;   // 深度模板视图
	D3D11_VIEWPORT m_ScreenViewport;                      // 视口

	int m_ClientWidth;                                    // 视口宽度
	int m_ClientHeight;                                   // 视口高度

	ComPtr<ID3D11InputLayout> m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> m_pVertexBuffer;		// 顶点缓冲区
	ComPtr<ID3D11VertexShader> m_pVertexShader;	// 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;	// 像素着色器

	ComPtr<ID3D11BlendState> m_pBlendState;	// 混合状态对象

	// 创建常量缓冲区
	ComPtr<ID3D11Buffer> matrixBuffer;

	DirectX::Mouse::ButtonStateTracker m_MouseTracker;			// 鼠标状态追踪器
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;	// 键盘状态追踪器

	std::vector<std::shared_ptr<UIBase>> uiComponents; // UI组件列表

	std::vector<std::shared_ptr<GameObject>> GameObjects; // obj列表

	int tick;
};
