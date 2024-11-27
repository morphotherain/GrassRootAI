#pragma once
#include <fstream>
#include "Scene.h"
#include "UIButton.h"


class DockScene : public Scene
{
public:
	DockScene() = default;
	DockScene(HINSTANCE _hInstance);
	~DockScene() = default;

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawScene();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
private:

	std::shared_ptr<Effect> m_effect;
	std::shared_ptr<UIButton> m_button;

};
