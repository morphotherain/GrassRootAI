#pragma once
#include "Scene.h"
#include "UIInputState.h"
#include "UIGameplayHost.h"
#include <functional>
#include <memory>

class UIRenderContext;
class UISystem;
class UINavigationStack;

class MainScene : public Scene
{
public:
	MainScene() = default;
	MainScene(HINSTANCE _hInstance);
	~MainScene();

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawScene();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();

	bool InitUIF();
	void UpdateUIFInput(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard);

	void SetGameplayHost(UIGameplayHost host) { m_gameplayHost = std::move(host); }
	void SetNavigationBusy(bool busy);

private:
	std::shared_ptr<Effect> m_effect;

	UIGameplayHost m_gameplayHost;

	bool m_uifEnabled = false;
	std::unique_ptr<UIRenderContext> m_uifContext;
	std::unique_ptr<UISystem> m_uifSystem;
	std::unique_ptr<UINavigationStack> m_navigation;
	UIInputState m_uifInput;
};
