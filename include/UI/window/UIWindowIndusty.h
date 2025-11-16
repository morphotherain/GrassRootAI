#pragma once
#include <fstream>
#include "UIWindow.h"
#include "UIText.h"
#include "UIButton.h"

#include <unordered_map>
#include <vector>

class UIWindowIndusty : public UIWindow
{
public:
	UIWindowIndusty();
	UIWindowIndusty(HINSTANCE _hInstance) :UIWindow(_hInstance) {}
	~UIWindowIndusty() = default;

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

	virtual void ParseParameters(std::unordered_map<std::string, std::any> paras);

protected:
	void InitWindowComponent();

private:
	// 选中的蓝图
	int m_selectedBlueprintTypeId = -1;

	// 中间蓝图信息
	std::shared_ptr<Effect> m_blueprintIconEffect;
	std::shared_ptr<UIText> m_blueprintNameText;
	std::shared_ptr<UIText> m_titleLeftText;
	std::shared_ptr<UIText> m_titleRightText;
	std::shared_ptr<UIText> m_queueTitleText;
	std::shared_ptr<UIButton> m_startButton;

	// 左侧材料
	std::vector<std::shared_ptr<Effect>> m_materialIconEffects;
	std::vector<std::shared_ptr<UIText>> m_materialTexts;

	// 右侧产出
	std::vector<std::shared_ptr<Effect>> m_productIconEffects;
	std::vector<std::shared_ptr<UIText>> m_productTexts;

	struct IndustryJob {
		int blueprintTypeId;
		int remainingSeconds;
		std::shared_ptr<UIText> lineText;
	};
	std::vector<IndustryJob> m_jobs;

	void LoadBlueprint(int blueprintTypeId);
	void BuildMaterialsUI(int blueprintTypeId);
	void BuildProductsUI(int blueprintTypeId);
	void BuildCenterUI(int blueprintTypeId);
	void BuildQueueLine(IndustryJob& job, float offsetY);

	std::wstring FormatTime(int totalSeconds);

public:
	virtual void handleTask(Task& task) override;

};
