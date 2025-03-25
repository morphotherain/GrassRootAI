
#pragma once
#include <fstream>
#include "UIBase.h"
#include "InputHandler.h"

class UIWindow : public UIBase
{
public:
	UIWindow();
	UIWindow(HINSTANCE _hInstance) :UIBase(_hInstance) {}
	~UIWindow() = default;


	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	virtual void  ParseParameters(std::unordered_map<std::string, std::any> paras) {};

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { 
		x = _x, y = _y, width = _deltaX, height = _deltaY; 
		DEBUG_("width:{} height:{}", width, height);
	}
	void setTex(std::string _Tex) { TexPath = _Tex; };


	std::shared_ptr<bool> isCatchingMouse = std::make_shared<bool>();
	std::shared_ptr<bool> getCatchingMouse() { return isCatchingMouse; }


	bool IsActive() const { return isActive; }
	bool IsVisible() const { return isVisible; }
	bool IsResizable() const { return isResizable; }
	bool IsMaximized() const { return isMaximized; }
	bool IsMinimized() const { return isMinimized; }
	bool ShouldClose() const { return isClosed; }
	void SetVisible(bool visible) { isVisible = visible; }
	void SetResizable(bool resizable) { isResizable = resizable; }
	void SetMaximized(bool maximized) { isMaximized = maximized; }
	void SetMinimized(bool minimized) { isMinimized = minimized; }
	void SetActive(bool active) { isActive = active; }
	void SetClosed(bool closed) { isClosed = closed; }


	int GetZOrder() const { return m_zOrder; }
	void SetZOrder(int zOrder) { m_zOrder = zOrder; }

	bool isDragging = false;
	InputHandler m_inputHandler;

	void HandleMouseEvent(float dt, DirectX::Mouse& mouse);


protected:
	void InitWindowComponent();

	float x = 0.0f;
	float y = 0.0f;
	float width = 600.0f;
	float height = 600.0f;
	float deltaX = 30.0f;
	float deltaY = 20.0f;

	float TitleHeight = 27.0f;
	std::wstring windowTitle = L"";

	std::shared_ptr<Effect> m_windowEffect;
	std::vector<std::string> textureWindowFileNames;
	std::vector<PosTexIndex> vertices;

	std::shared_ptr<OrthographicCamera> m_pUICamera;


private:
	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	ComPtr<ID3D11ShaderResourceView> window_textureArraySRV;
	ComPtr<ID3D11InputLayout> window_m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> window_m_pVertexBuffer;		// 顶点缓冲区
	std::fstream fs;

	ComPtr<ID3D11PixelShader> m_pTestPixelShader;	// 像素着色器



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
	bool isActive = true;
	bool isClosed = false;

	int m_zOrder = 0;


};

