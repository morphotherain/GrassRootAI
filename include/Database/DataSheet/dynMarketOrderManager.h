// MarketOrderManager.h
#pragma once
#include "DatabaseManager.h"
#include "dynMarketOrderManager.h"
#include <vector>

struct MarketOrder {
    MarketOrder() = default;
    ~MarketOrder() = default;

    void Init();
    void Draw();

    int orderID = 0;
    int orderItemType = 0;
    int amount = 0;
    double price = 0.0f;
    int locationObjectID = 0;
    int ticksRemain = 0;
    int createTime = 0;
    int regionID = 0;
    int availbleRange = 0;
    bool isSellOrder = false;

    std::wstring locationObjectIDText;
};

class dynMarketOrderManager : public SingletonBase<dynMarketOrderManager> {
private:
    sqlite3_stmt* stmt = nullptr;
    sqlite3* db;

public:
    dynMarketOrderManager() { db = DatabaseManager::getInstance()->getDatabase(); }
    ~dynMarketOrderManager() = default;

    // 创建表
    bool createTable();

    // 插入订单
    bool insertOrder(const MarketOrder& order);

    // 根据类型ID和区域ID获取订单列表
    std::vector<MarketOrder> getOrdersByTypeIDandRegionID(int typeID, int regionID);

    // 根据订单ID获取订单
    MarketOrder getOrderByID(int orderID);

    // 更新订单
    bool updateOrder(const MarketOrder& order);

    // 删除订单
    bool deleteOrder(int orderID);
};