// MarketOrderManager.cpp
#include "dynMarketOrderManager.h"
#include "InvTypesManager.h"
#include <string>
#include <vector>

bool dynMarketOrderManager::createTable() {
    std::string query =
        "CREATE TABLE IF NOT EXISTS dynMarketOrders ("
        "orderID INTEGER PRIMARY KEY,"
        "orderItemType INTEGER NOT NULL,"
        "amount INTEGER NOT NULL,"
        "price REAL NOT NULL,"
        "locationObjectID INTEGER NOT NULL,"
        "availbleRange INTEGER NOT NULL,"
        "createTime INTEGER NOT NULL,"
        "isSellOrder BOOLEAN NOT NULL,"
        "regionID INTEGER NOT NULL"
        ")";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, 0, &errMsg);

    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool dynMarketOrderManager::insertOrder(const MarketOrder& order) {
    std::string query =
        "INSERT INTO dynMarketOrders ("
        "orderItemType, amount, price, locationObjectID, "
        "availbleRange, createTime, isSellOrder, regionID) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, order.orderItemType);
    sqlite3_bind_int(stmt, 2, order.amount);
    sqlite3_bind_double(stmt, 3, order.price);
    sqlite3_bind_int(stmt, 4, order.locationObjectID);
    sqlite3_bind_int(stmt, 5, order.availbleRange);
    sqlite3_bind_int(stmt, 6, order.createTime);
    sqlite3_bind_int(stmt, 7, order.isSellOrder ? 1 : 0);
    sqlite3_bind_int(stmt, 8, order.regionID);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

std::vector<MarketOrder> dynMarketOrderManager::getOrdersByTypeIDandRegionID(int typeID, int regionID) {
    std::vector<MarketOrder> orders;
    std::string query =
        "SELECT orderID, orderItemType, amount, price, locationObjectID, "
        "availbleRange, createTime, isSellOrder, regionID "
        "FROM dynMarketOrders WHERE orderItemType = ? AND regionID = ?";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return orders;

    sqlite3_bind_int(stmt, 1, typeID);
    sqlite3_bind_int(stmt, 2, regionID);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        MarketOrder order;
        order.orderID = sqlite3_column_int(stmt, 0);
        order.orderItemType = sqlite3_column_int(stmt, 1);
        order.amount = sqlite3_column_int(stmt, 2);
        order.price = sqlite3_column_double(stmt, 3);
        order.locationObjectID = sqlite3_column_int(stmt, 4);
        order.availbleRange = sqlite3_column_int(stmt, 5);
        order.createTime = sqlite3_column_int(stmt, 6);
        order.isSellOrder = (sqlite3_column_int(stmt, 7) != 0);
        order.regionID = sqlite3_column_int(stmt, 8);

        // 获取locationObjectIDText (假设通过InvTypesManager获取)
        order.locationObjectIDText = InvTypesManager::getInstance()->getNameByTypeId(order.locationObjectID);

        orders.push_back(order);
    }

    sqlite3_finalize(stmt);
    return orders;
}

MarketOrder dynMarketOrderManager::getOrderByID(int orderID) {
    MarketOrder order;
    std::string query =
        "SELECT orderID, orderItemType, amount, price, locationObjectID, "
        "availbleRange, createTime, isSellOrder, regionID "
        "FROM dynMarketOrders WHERE orderID = ?";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return order;

    sqlite3_bind_int(stmt, 1, orderID);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        order.orderID = sqlite3_column_int(stmt, 0);
        order.orderItemType = sqlite3_column_int(stmt, 1);
        order.amount = sqlite3_column_int(stmt, 2);
        order.price = sqlite3_column_double(stmt, 3);
        order.locationObjectID = sqlite3_column_int(stmt, 4);
        order.availbleRange = sqlite3_column_int(stmt, 5);
        order.createTime = sqlite3_column_int(stmt, 6);
        order.isSellOrder = (sqlite3_column_int(stmt, 7) != 0);
        order.regionID = sqlite3_column_int(stmt, 8);

        // 获取locationObjectIDText
        order.locationObjectIDText = InvTypesManager::getInstance()->getNameByTypeId(order.locationObjectID);
    }

    sqlite3_finalize(stmt);
    return order;
}

bool dynMarketOrderManager::updateOrder(const MarketOrder& order) {
    std::string query =
        "UPDATE dynMarketOrders SET "
        "orderItemType = ?, amount = ?, price = ?, locationObjectID = ?, "
        "availbleRange = ?, createTime = ?, isSellOrder = ?, regionID = ? "
        "WHERE orderID = ?";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, order.orderItemType);
    sqlite3_bind_int(stmt, 2, order.amount);
    sqlite3_bind_double(stmt, 3, order.price);
    sqlite3_bind_int(stmt, 4, order.locationObjectID);
    sqlite3_bind_int(stmt, 5, order.availbleRange);
    sqlite3_bind_int(stmt, 6, order.createTime);
    sqlite3_bind_int(stmt, 7, order.isSellOrder ? 1 : 0);
    sqlite3_bind_int(stmt, 8, order.regionID);
    sqlite3_bind_int(stmt, 9, order.orderID);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

bool dynMarketOrderManager::deleteOrder(int orderID) {
    std::string query = "DELETE FROM dynMarketOrders WHERE orderID = ?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    sqlite3_bind_int(stmt, 1, orderID);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}