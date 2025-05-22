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

    void DrawIndex();
    void DrawInfo();

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

    std::unordered_map < std:: string, std::shared_ptr<UIText >> infoTexts;

    std::shared_ptr<Effect> selectBrightEffect;
    std::shared_ptr<Effect> categoryBgrEffect;
    std::shared_ptr<Effect> typeInfoEffect;

    std::shared_ptr<Effect> selectTypeImgEffect;
    std::shared_ptr<UIText> selectTypeNameText;
    std::shared_ptr<UIText> selectTypeGroupText;

    std::shared_ptr<UIText> selectTypeBuyerText;
    std::shared_ptr<UIText> selectTypeSellerText;

    std::shared_ptr<UIText> selectOrderDataText;
    std::shared_ptr<UIText> selectPriceHistoryText;

    
    void updateTable(UITable& table, std::vector<MarketOrder>& orders, int firstDataRow, bool isSellOrder);
    
    std::vector<MarketOrder> m_sellOrders;
    std::vector<MarketOrder> m_buyOrders;

    UITable m_sellTable;       // 卖出订单表格
    UITable m_buyTable;        // 买入订单表格
    int m_sellTableFirstDataRow = 1; // 卖出表格数据起始行（表头占第0行）
    int m_buyTableFirstDataRow = 1;  // 买入表格数据起始行

    int selectTypeId = 0;
    int lastIndexClickY = -1;
    int offsetY = 0;

    std::unordered_map<int, std::vector<int>> categoryGroups;
    std::unordered_map<int, std::vector<int>> groupTypes;
    float currentY; // 记录当前纵坐标

    DirectX::Mouse::ButtonStateTracker m_MouseTracker;			// 鼠标状态追踪器
    DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;	// 键盘状态追踪器

    void fillTestOrders(int typeID);
    void fillOrders(int typeID, int regionID); // 从数据库加载订单
};