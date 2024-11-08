
#pragma once
#include <fstream>
#include "UIWindow.h"
class UIWindowOverview : public UIWindow
{
public:
	UIWindowOverview();
	UIWindowOverview(HINSTANCE _hInstance) :UIWindow(_hInstance) {}
	~UIWindowOverview() = default;


	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
	virtual void DrawUI();
	virtual void cleanup();

	bool InitResource();
	bool InitEffect();
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { x = _x, y = _y, deltaX = _deltaX, deltaY = _deltaY; }
	void setTex(std::string _Tex) { TexPath = _Tex; };


protected:
	void InitWindowComponent();


private:

	std::wstring windowTitle;

	std::string TexPath;
	std::string TitleTexPath;
	std::string CloseTexPath;
	std::string MaxTexPath;
	std::string MinTexPath;
	std::string BodyTexPath;



	// 窗口标志变量
	bool isVisible = true;
	bool isResizable = false; //是否可调整大小
	bool isMaximized = false;
	bool isMinimized = false;

};

