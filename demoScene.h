#pragma once
#include "Scene.h"
class demoScene : public Scene
{
public :
	demoScene() = default;
	demoScene(HINSTANCE _hInstance) :Scene(_hInstance) {}
	~demoScene() = default;

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

};

