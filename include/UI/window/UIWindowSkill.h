#pragma once
#include <fstream>
#include "UIWindow.h"
#include "UIText.h"
#include "UIButton.h"
#include "UITable.h"

#include "InvTypesManager.h"
#include "invGroupsManager.h"
#include "invCategoriesManager.h"
#include "dynMarketOrderManager.h"

class UIWindowSkill : public UIWindow
{
public:
    UIWindowSkill();
    UIWindowSkill(HINSTANCE _hInstance) : UIWindow(_hInstance) {}
    ~UIWindowSkill() = default;

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

    void DrawIndex();
    void DrawInfo();

    void UpdateUIInfo(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);

    bool InitResource();
    bool InitEffect();
    void setSize(const float _x, const float _y, const float _deltaX, const float _deltaY) { x = _x, y = _y, deltaX = _deltaX, deltaY = _deltaY; }

    virtual void ParseParameters(std::unordered_map<std::string, std::any> paras);

protected:
    void InitWindowComponent();
    void ExpandCategory(int categoryID);
    void ExpandGroup(int groupID);
    void CollapseCategory(int categoryID);
    void CollapseGroup(int groupID);

    void switchSelectType(int typeID);
    std::shared_ptr<Effect>  GenerateSkillLevelIndicators( int level, float startX, float startY);

    void InitSkillQueue();
    void UpdateSkillQueue(float dt, DirectX::Mouse& mouse, DirectX::Keyboard& keyboard, UINT tick);
    void DrawSkillQueue();
    std::wstring FormatTime(int totalSeconds);
    std::wstring ConvertToRoman(int level);

private:
    std::unordered_map<int, bool> categoryExpanded;
    std::unordered_map<int, bool> groupExpanded;
    std::unordered_map<int, std::shared_ptr<UIText>> categoryTexts;
    std::unordered_map<int, std::shared_ptr<Effect>> categoryArrows;
    std::unordered_map<int, std::shared_ptr<Effect>> categoryArrowsExpanded;
    std::unordered_map<int, std::shared_ptr<Effect>> categoryIcons;
    std::unordered_map<int, std::shared_ptr<UIText>> groupTexts;
    std::unordered_map<int, std::shared_ptr<Effect>> groupArrows;
    std::unordered_map<int, std::shared_ptr<Effect>> groupArrowsExpanded;
    std::unordered_map<int, std::shared_ptr<Effect>> groupIcons;
    std::unordered_map<int, std::shared_ptr<UIText>> typeTexts;

    class skillQueueEffect {
    public:
        std::shared_ptr<UIText> skillNameText;
        std::shared_ptr<UIButton> skillRemoveButton;
        std::shared_ptr<Effect> m_skillIndicator;
        DynSkillQueue skill;
    };
    std::vector<skillQueueEffect> m_SkillQueueEffects;

    std::unordered_map < std::string, std::shared_ptr<UIText >> infoTexts;

    std::shared_ptr<Effect> selectBrightEffect;
    std::shared_ptr<Effect> categoryBgrEffect;
    std::shared_ptr<Effect> typeInfoEffect;

    std::shared_ptr<Effect> selectTypeImgEffect;
    std::shared_ptr<UIText> selectTypeNameText;
    std::shared_ptr<UIText> selectTypeGroupText;

    std::shared_ptr<UIText> skillQueueTileText;
    std::shared_ptr<UIButton> skillAddButton;

    std::shared_ptr<UIText> selectTypeBuyerText;
    std::shared_ptr<UIText> selectTypeSellerText;

    std::shared_ptr<UIText> selectOrderDataText;
    std::shared_ptr<UIText> selectPriceHistoryText;

    std::shared_ptr<Effect> m_skillIndicator;


    int selectTypeId = 0;
    int lastIndexClickY = -1;
    int offsetY = 0;

    const float LARGE_SQUARE_SIZE = 12.0f;   // 大正方形尺寸
    const float SMALL_SQUARE_SIZE = 6.0f;    // 小正方形尺寸
    const float SPACING = 4.0f;              // 正方形之间的间距

    std::unordered_map<int, std::vector<int>> categoryGroups;
    std::unordered_map<int, std::vector<int>> groupTypes;
    float currentY; // 记录当前纵坐标

    DirectX::Mouse::ButtonStateTracker m_MouseTracker;			// 鼠标状态追踪器
    DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;	// 键盘状态追踪器


};