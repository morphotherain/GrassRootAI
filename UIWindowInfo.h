
#pragma once
#include <fstream>
#include "UIWindow.h"


class UIWindowInfo : public UIWindow
{
public:
	UIWindowInfo();
	UIWindowInfo(HINSTANCE _hInstance) :UIWindow(_hInstance) {}
	~UIWindowInfo() = default;

	struct ItemData {
		std::wstring Name;
		std::wstring Description;
		std::wstring IconPathPrint;
		std::string IconPath;
		int IconId;
	}testItem;;


	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { x = _x, y = _y, deltaX = _deltaX, deltaY = _deltaY; }
	void setTypeID(int _typeID ) { typeID = _typeID; };
private:
	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	ComPtr<ID3D11ShaderResourceView> window_textureArraySRV;
	ComPtr<ID3D11InputLayout> window_m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> window_m_pVertexBuffer;		// 顶点缓冲区
	std::fstream fs;

	ComPtr<ID3D11PixelShader> m_pTestPixelShader;	// 像素着色器

	std::wstring windowTitle;

	int typeID;
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

