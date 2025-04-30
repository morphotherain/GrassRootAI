#pragma once
#include <fstream>
#include "UIWindow.h"
#include "UIText.h"
#include "UIButton.h"

#include "InvTypesManager.h"
#include "invGroupsManager.h"
#include "invCategoriesManager.h"

class UIWindowMarket : public UIWindow
{
public:
    UIWindowMarket();
    UIWindowMarket(HINSTANCE _hInstance) : UIWindow(_hInstance) {}
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
    void ExpandCategory(int categoryID);
    void ExpandGroup(int groupID);
    void CollapseCategory(int categoryID);
    void CollapseGroup(int groupID);

private:
    std::unordered_map<int, bool> categoryExpanded;
    std::unordered_map<int, bool> groupExpanded;
    std::unordered_map<int, std::shared_ptr<UIText>> categoryTexts;
    std::unordered_map<int, std::shared_ptr<UIText>> groupTexts;
    std::unordered_map<int, std::shared_ptr<UIText>> typeTexts;
    std::unordered_map<int, std::vector<int>> categoryGroups;
    std::unordered_map<int, std::vector<int>> groupTypes;
    float currentY; // 记录当前纵坐标
};