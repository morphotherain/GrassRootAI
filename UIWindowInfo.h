
#pragma once
#include <fstream>
#include "InputHandler.h"
#include "UIWindow.h"

#define WINDOW_INFO_ID_TYPE 0
#define WINDOW_INFO_ID_OBJECT 1


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
		int IconId = 0;
	}testItem;

	struct MatrixBufferType
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		float TexIndex; // 使用float来传递索引
		float padding[3]; // 添加padding以确保结构体的大小为16的倍数
	};


	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	virtual void  ParseParameters(std::unordered_map<std::string, std::any> paras);

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

	int typeID = 0;

	std::shared_ptr<Effect> m_itemImgEffect;
};

