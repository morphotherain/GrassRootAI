
#pragma once
#include <fstream>
#include "UIWindow.h"

#include "UIButton.h"
#include "UIText.h"
#include "UIRButtonMenu.h"

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
	void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY);

	struct Row {
		Row() = default;
		~Row() = default;
		UINT objectID;
		double distance;
		double velocity;
		std::wstring typeName;
		std::wstring Name;
		std::vector<std::shared_ptr<UIText>> texts;

		void Init(float x,float y, float Height);
		void Update();
		void Draw();
		void setIndex(float x, float y, float TitleHeight);

		std::wstring getDisplay();
		
	};
	struct RowMgr {
		RowMgr(float _x,float _y, float _Height) :x(_x), y(_y), Height(_Height) {}
		float x;
		float y;
		float Height;
		std::vector<std::shared_ptr<Row>> Rows;
		void Init();
		void Update(UINT tick);
		void Draw();

		int index = 0;
		int next_index = 0;
	};

	std::shared_ptr<RowMgr> m_RowMgr;

protected:
	void InitWindowComponent();


private:


	std::shared_ptr<UIRButtonMenu> m_RButtonMenu;

};

