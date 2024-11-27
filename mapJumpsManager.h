#pragma once
#include "DatabaseManager.h"

class mapJumpsManager : public SingletonBase<mapJumpsManager> {
private:
    sqlite3_stmt* stmt = nullptr;
    sqlite3* db;

public:
    mapJumpsManager() { db = DatabaseManager::getInstance()->getDatabase(); };

    // 假设在 invtypes 表中根据 type_id 查找 name 列
    int getDestinationIDByObjectId(int type_id);
};

