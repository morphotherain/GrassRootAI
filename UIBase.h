#pragma once
#include "d3dUtil.h"
#include "DXTrace.h"
#include <vector>
#include <DirectXMath.h>
#include <any>
#include "Camera.h"
#include "ddsLoader.h"
#include "RenderProxy.h"
#include "SolarSystemMgr.h"

class UIBase
{
public:
	UIBase() = default;
	UIBase(HINSTANCE hInstance);
	~UIBase() = default;

	virtual void AddUIComponent(std::shared_ptr<UIBase> component) {
		childComponents.push_back(component);
	}

	virtual bool Init() = 0;
	virtual void OnResize() = 0;
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick) = 0;
	virtual void DrawUI() = 0;
	virtual void cleanup() = 0;

	virtual void ParseParameters(std::unordered_map<std::string, std::any> paras) {};

	void setcameraResource(int m_ClientWidth, int m_ClientHeight, std::shared_ptr<Camera> pCamera);
	void setDelta(const float _deltaX, const float _deltaY) 
		{ deltaX = _deltaX; deltaY = _deltaY;};

	enum class CameraMode { FirstPerson, ThirdPerson, Free };
	float AspectRatio()const { return static_cast<float>(m_ClientWidth) / m_ClientHeight; }


	std::vector<PosTexIndex> GenerateRectVertex(std::vector<PosTexIndex>& vertices, float _x, float _y, float _deltaX, float _deltaY, float TexID);
	std::vector<PosTexIndex> GenerateRectVertex(std::vector<PosTexIndex>& vertices, float _x, float _y, float _deltaX, float _deltaY, float angle, float TexID);

protected:

	HWND      m_hMainWnd;        // 主窗口句柄
	HINSTANCE hInstance;

	ddsLoader m_ddsLoader;                                //材质加载

	CameraMode m_CameraMode = CameraMode::Free;									// 摄像机模式
	std::shared_ptr<Camera> m_pCamera;						    // 摄像机
	std::shared_ptr<OrthographicCamera> m_pUICamera;

	// 常用资源
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;        // 深度模板缓冲区
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;   // 渲染目标视图
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;   // 深度模板视图
	D3D11_VIEWPORT m_ScreenViewport = {};                      // 视口

	int m_ClientWidth = 800;                                    // 视口宽度
	int m_ClientHeight = 600;                                   // 视口高度

	float deltaX = 0.0f;
	float deltaY = 0.0f;


	ComPtr<ID3D11InputLayout> m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> m_pVertexBuffer;		// 顶点缓冲区
	ComPtr<ID3D11VertexShader> m_pVertexShader;	// 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;	// 像素着色器


	// 创建常量缓冲区
	ComPtr<ID3D11Buffer> matrixBuffer;

	DirectX::Mouse::ButtonStateTracker m_MouseTracker;			// 鼠标状态追踪器
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;	// 键盘状态追踪器


	// 添加 UI 组件列表成员
	std::vector<std::shared_ptr<UIBase>> childComponents;
};

// 辅助函数，用于从 std::unordered_map<std::string, std::any> 中获取指定类型的参数
template<typename T>
T getParameter(const std::unordered_map<std::string, std::any>& paras, const std::string& key, T defaultValue) {
	auto it = paras.find(key);
	if (it != paras.end()) {
		try {
			return std::any_cast<T>(it->second);
		}
		catch (const std::bad_any_cast&) {
			// 类型不匹配，返回默认值
			return defaultValue;
		}
	}
	// 参数不存在，返回默认值
	return defaultValue;
}

// 封装的方法，用于创建绕组件中心旋转的变换矩阵
DirectX::XMMATRIX CreateRotatedWindowModel(float x, float y, float width, float height, float rotationAngle);