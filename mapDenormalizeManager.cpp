
#include "mapDenormalizeManager.h"

unsigned int mapDenormalizeManager::getTypeIDByContainerId(int container_id)
{
    unsigned int  id;
    std::string query = "SELECT typeID FROM mapDenormalize WHERE itemID =?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, container_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return id;
}


std::vector<double> mapDenormalizeManager::getPosByObjectID(int object_id) {
    std::vector<double> pos(3, 0.0f); // 初始化vector，长度为3，初始值都为0.0f，表示三维坐标
    std::string query = "SELECT x, y, z FROM mapDenormalize WHERE itemID =?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, object_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            pos[0] = (sqlite3_column_double(stmt, 0));
            pos[1] = (sqlite3_column_double(stmt, 1));
            pos[2] = (sqlite3_column_double(stmt, 2));
        }
    }
    sqlite3_finalize(stmt);
    return pos;
}