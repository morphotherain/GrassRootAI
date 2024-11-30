#pragma once
#include <fstream>
#include "VertexData.h"
#include "Scene.h"
#include "SolarSystem.h"
#include "UIText.h"
#include "UISkyBox.h"
#include "UIRButtonMenu.h"


class SpaceScene : public Scene
{
public:
	SpaceScene() = default;
	SpaceScene(HINSTANCE _hInstance);
	~SpaceScene() = default;

	struct Vertex3DPosIndex 
	{
		double x;
		double y;
		double z;
		float texIndex;              // 纹理索引
		std::wstring name;
		std::shared_ptr<UIText> text;
	};


	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawScene();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
private:
	std::shared_ptr<Effect> m_pBracketEffect;
	std::shared_ptr<UIRButtonMenu> m_RButtonMenu;
	
	ComPtr<ID3D11ShaderResourceView> textureArraySRV;

	std::vector<Pos2Tex> vertices;
	std::vector<Vertex3DPosIndex> vertices3D;

	std::vector<Pos2Tex> objectVertices;
	std::vector<Vertex3DPosIndex> objectVertices3D;

	ComPtr<ID3D11PixelShader> m_pPixelShaderBillboard;	// 像素着色器
	ComPtr<ID3D11VertexShader> m_pVertexShaderBillboard;	// 顶点着色器
	ComPtr<ID3D11GeometryShader> m_pGeometryShader;	// 几何着色器

	std::shared_ptr<SolarSystem> m_pSolarSystem;
	std::shared_ptr<UISkyBox> m_skybox;

	std::vector<std::shared_ptr<UIText>> m_denormalizeTexts; // 用于存储星域名称文本对象


};
