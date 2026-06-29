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
	// 游戏整体状态
	enum class GameState { MainMenu, Loading, InGame };

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

	// 从主菜单开始一个新的游戏：创建/加载存档，初始化各系统，然后切换到游戏场景。
	void StartNewGame();

	// 加载已有存档槽并进入游戏
	bool EnterGameFromSlot(int slotID);

	// 结束当前游戏会话并返回主菜单
	void ReturnToMainMenu();

	// 延迟到本帧 UpdateScene 结束后再执行，避免在 UI Update 栈内切场景
	void RequestStartNewGame();
	void RequestReturnToMainMenu();
	void RequestEnterGameFromSlot(int slotID);
	void RequestCreateAndEnterGame(const std::string& displayName);

	GameState GetGameState() const { return m_gameState; }

private:
	bool InitEffect();
	bool InitResource();
	void InitializeGameSystems();
	void ProcessDeferredActions();

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

	// 当前游戏状态（主菜单 / 游戏中）
	GameState m_gameState = GameState::MainMenu;

	bool m_pendingStartNewGame = false;
	bool m_pendingReturnToMainMenu = false;
	bool m_pendingEnterGameFromSlot = false;
	bool m_pendingCreateAndEnterGame = false;
	int m_pendingSlotID = -1;
	std::string m_pendingDisplayName;
};

#endif