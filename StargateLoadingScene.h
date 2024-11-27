#pragma once
#include "Scene.h"


class StargateLoadingScene : public Scene
{
public:
	StargateLoadingScene() = default;
	StargateLoadingScene(HINSTANCE _hInstance);
	~StargateLoadingScene() = default;

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
