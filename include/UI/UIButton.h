#pragma once
#include <fstream>
#include "UIBase.h"
#include "UIText.h"
class UIButton : public UIBase
{
public:
	UIButton() = default;
	UIButton(HINSTANCE _hInstance) :UIBase(_hInstance) {}
	~UIButton() = default;

	//

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _width, const float _height) { x = _x, y = _y, width = _width, height = _height; }
	void setTex(std::string _Tex) { TexPath = _Tex; };
	void setText(std::wstring text);
	void setCooldown(int cd) { cooldown = cd; }
	std::shared_ptr<bool> getClickFlag() { return clickFlag; }

private:
	std::fstream fs;

	std::shared_ptr<bool> clickFlag;
	int curCooldown = 0;
	int cooldown = 60;
	std::shared_ptr<UIText> text;

	float x = 10.0f;
	float y = 30.0f;
	float width = 20.0f;
	float height = 4.0f;
	std::string TexPath;

	std::shared_ptr<Effect> m_effect;

	std::vector<PosTexIndex> GenerateButtonVertices(float x, float y, float deltaX, float deltaY);
};
