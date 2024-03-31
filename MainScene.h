#pragma once
#include "Scene.h"
class MainScene : public Scene
{
public:
	MainScene() = default;
	MainScene(HINSTANCE _hInstance) :Scene(_hInstance) {}
	~MainScene() = default;

	struct VertexPosColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 TexCoor;
		float index;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
	};


	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard);
	virtual void DrawScene();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
private:
	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	ComPtr<ID3D11ShaderResourceView> button_textureArraySRV;
	ComPtr<ID3D11InputLayout> button_m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> button_m_pVertexBuffer;		// 顶点缓冲区
};

