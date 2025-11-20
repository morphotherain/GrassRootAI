#pragma once
#include "Scene.h"
#include "UIButton.h"
#include <functional>

class MainScene : public Scene
{
public:
	MainScene() = default;
	MainScene(HINSTANCE _hInstance);
	~MainScene() = default;

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawScene();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();

	// 设置“开始游戏”按钮的回调，由 GameApp 在切换到 MainScene 时注入
	void setStartGameCallback(std::function<void()> cb) { m_onStartGame = std::move(cb); }

private:

	std::shared_ptr<Effect> m_effect;

	// 主菜单的“开始游戏”按钮
	std::shared_ptr<UIButton> m_startButton;

	// 点击开始按钮后要执行的回调（例如 GameApp::StartNewGame）
	std::function<void()> m_onStartGame;
};
