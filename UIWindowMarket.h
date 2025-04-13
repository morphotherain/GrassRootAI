#pragma once
#include <fstream>
#include "UIWindow.h"
#include "UIText.h"
#include "UIButton.h"

class UIWindowMarket : public UIWindow
{
public:
	UIWindowMarket();
	UIWindowMarket(HINSTANCE _hInstance) :UIWindow(_hInstance) {}
	~UIWindowMarket() = default;

	struct Item {
		Item() = default;
		~Item() = default;

		int objectID;
		std::shared_ptr<UIButton> currentSelectBox;
	};



	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { x = _x, y = _y, deltaX = _deltaX, deltaY = _deltaY; }

	virtual void ParseParameters(std::unordered_map<std::string, std::any> paras);

protected:
	void InitWindowComponent();

private:

};
