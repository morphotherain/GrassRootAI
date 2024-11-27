#pragma once
#include <fstream>
#include "Scene.h"
#include "UIButton.h"


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
	virtual void UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawScene();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
private:

	std::shared_ptr<Effect> m_effect;
	std::shared_ptr<UIButton> m_button;

};
