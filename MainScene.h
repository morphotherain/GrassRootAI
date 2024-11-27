#pragma once
#include "Scene.h"


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
private:

	std::shared_ptr<Effect> m_effect;
};
