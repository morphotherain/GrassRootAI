#pragma once
#include "d3dUtil.h"
#include "DXTrace.h"
#include <vector>
#include <DirectXMath.h>
#include "Camera.h"
#include "core.h"
#include "ddsLoader.h"
#include "Component.h"

enum RenderType {Player, Map, Entity};

class RenderComponent : public Component {
public:
	struct VertexPosColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 TexCoor;
		float index;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
	};
public:
	RenderComponent() = default;
	RenderComponent(HINSTANCE hInstance);
	~RenderComponent() = default;

	bool Init();
	void OnResize();
	void Update(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard);
	void Draw();
	void cleanup();

	void setd3dResource(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, IDXGISwapChain* pSwapChain, HWND hMainWnd, ID3D11RenderTargetView* pRenderTargetView, ID3D11DepthStencilView* m_pDepthStencilView);
	void setcameraResource(int m_ClientWidth, int m_ClientHeight, std::shared_ptr<Camera> pCamera);

	
	enum class CameraMode { FirstPerson, ThirdPerson, Free };
	float AspectRatio()const { return static_cast<float>(m_ClientWidth) / m_ClientHeight; }
	void setpos(const float _x, const float _y, const float _z) { posX = _x, posY = _y, posZ = _z; }

	bool InitResource();
	bool InitEffect();

protected:

	HWND      m_hMainWnd;        // 主窗口句柄
	HINSTANCE hInstance;

	ddsLoader m_ddsLoader;                                //材质加载

	CameraMode m_CameraMode;									// 摄像机模式
	std::shared_ptr<Camera> m_pCamera;						    // 摄像机

	ComPtr<ID3D11Device> m_pd3dDevice;
	ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;   // D3D11设备上下文
	ComPtr<IDXGISwapChain> m_pSwapChain;                  // D3D11交换链

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


	// 创建常量缓冲区
	ComPtr<ID3D11Buffer> matrixBuffer;
	ComPtr<ID3D11ShaderResourceView> textureArraySRV;

	DirectX::Mouse::ButtonStateTracker m_MouseTracker;			// 鼠标状态追踪器
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;	// 键盘状态追踪器

	float posX;
	float posY;
	float posZ;

public:
	RenderType layer = RenderType::Player;

    
};