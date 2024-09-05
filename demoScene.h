#pragma once
#include "Scene.h"
#include "GamePlayer.h"
class demoScene : public Scene
{
public:
	demoScene() = default;
	demoScene(HINSTANCE _hInstance);
	~demoScene() = default;

	struct VertexPosColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 TexCoor;
		float index;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
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

	std::shared_ptr<GamePlayer> playerObj; // obj列表
};
