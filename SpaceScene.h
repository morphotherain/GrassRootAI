#pragma once
#include <fstream>
#include "Scene.h"
#include "SolarSystem.h"
#include "UIText.h"
#include "UISkyBox.h"


class SpaceScene : public Scene
{
public:
	SpaceScene() = default;
	SpaceScene(HINSTANCE _hInstance);
	~SpaceScene() = default;

	struct VertexPosColor
	{
		DirectX::XMFLOAT2 pos;       // 2D 屏幕坐标
		float texIndex;              // 纹理索引
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
	};

	struct Vertex3DPosIndex 
	{
		DirectX::XMFLOAT3 pos;       // 2D 屏幕坐标
		float texIndex;              // 纹理索引
		std::wstring name;
		std::shared_ptr<UIText> text;
	};


	struct MapVertexPosColor {
		DirectX::XMFLOAT2 position;  // 2D 屏幕坐标
		float texIndex;              // 纹理索引
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
	std::shared_ptr<Effect> m_pBracketEffect;
	
	ComPtr<ID3D11ShaderResourceView> textureArraySRV;
	std::fstream fs;

	std::vector<MapVertexPosColor> GenerateVertices(int n);
	std::vector<MapVertexPosColor> vertices;
	std::vector<Vertex3DPosIndex> vertices3D;

	ComPtr<ID3D11PixelShader> m_pPixelShaderBillboard;	// 像素着色器
	ComPtr<ID3D11VertexShader> m_pVertexShaderBillboard;	// 顶点着色器
	ComPtr<ID3D11GeometryShader> m_pGeometryShader;	// 几何着色器

	std::shared_ptr<SolarSystem> m_pSolarSystem;
	std::shared_ptr<UISkyBox> m_skybox;

	std::vector<std::shared_ptr<UIText>> m_denormalizeTexts; // 用于存储星域名称文本对象


};
