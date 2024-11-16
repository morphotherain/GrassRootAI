#pragma once
#include "UIBase.h"
#include "UIButton.h"

class UIRButtonMenu : public UIBase
{
public:
	UIRButtonMenu() = default;
	UIRButtonMenu(UINT _source_id,UINT _target_id ) :
		source_object_id(_source_id), target_object_id(_target_id),m_button(){}
	~UIRButtonMenu() = default;

	

	virtual bool Init();
	virtual void OnResize() {};
	virtual void UpdateUI(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick) {};
	virtual void DrawUI();
	virtual void cleanup() {};

	bool InitResource() {};
	bool InitEffect() {};
	void setSize(const float _x, const float _y) { x = _x, y = _y; }
	void addRow(UINT count, std::wstring text);

	std::vector<std::shared_ptr<UIButton>> m_button;
	UINT source_object_id;
	UINT target_object_id;

private:

	float x = 10.0f;
	float y = 30.0f;


};

