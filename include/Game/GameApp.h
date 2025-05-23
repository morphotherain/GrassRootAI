#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "SolarSystemMgr.h"
#include "DockScene.h"
#include "MainScene.h"
#include "SpaceScene.h"
#include "StargateLoadingScene.h"

class GameApp : public D3DApp
{
public:
	// 摄像机模式
	enum class CameraMode { FirstPerson, ThirdPerson, Free };

	struct VertexPosColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 TexCoor;
		float index;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
	};

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	// 在GameApp类中修改SwitchToScene方法为模板函数
	void SwitchToScene(std::unique_ptr<Scene> newScene);

private:
	bool InitEffect();
	bool InitResource();

private:
	ComPtr<ID3D11InputLayout> m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> m_pVertexBuffer;		// 顶点缓冲区
	ComPtr<ID3D11VertexShader> m_pVertexShader;	// 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;	// 像素着色器

	ddsLoader m_ddsLoader;                                //材质加载

	CameraMode m_CameraMode;									// 摄像机模式
	std::shared_ptr<Camera> m_pCamera;						    // 摄像机
	// 创建常量缓冲区
	ComPtr<ID3D11Buffer> matrixBuffer;

	ComPtr<ID3D11ShaderResourceView> textureArraySRV;

	std::unique_ptr<Scene> currentScene; //当前的界面
	UINT currentSceneID = 0;
	UINT tick = 0;
};

#endif