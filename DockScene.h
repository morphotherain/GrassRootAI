#pragma once
#include <fstream>
#include "Scene.h"


class DockScene : public Scene
{
public:
	DockScene() = default;
	DockScene(HINSTANCE _hInstance);
	~DockScene() = default;

	struct VertexPosColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 TexCoor;
		float index;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
	};


	struct MapVertexPosColor {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;
		float texIndex;    // 纹理索引，作为浮点数存储
	};
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
	virtual void UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, int& switchScene);
	virtual void DrawScene();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
private:
	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	ComPtr<ID3D11ShaderResourceView> button_textureArraySRV;
	ComPtr<ID3D11InputLayout> button_m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> button_m_pVertexBuffer;		// 顶点缓冲区
	std::fstream fs;

	std::vector<MapVertexPosColor> GenerateVertices(int n);
};
