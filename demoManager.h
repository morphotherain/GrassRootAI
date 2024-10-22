// InvTypesManager.h
#pragma once
#include "DatabaseManager.h"

class demoManager : public SingletonBase<demoManager> {
private:
    sqlite3_stmt* stmt;
    sqlite3* db;

public:
    demoManager() { db = DatabaseManager::getInstance()->getDatabase(); };

    // 假设在 invtypes 表中根据 type_id 查找 name 列
    std::wstring getNameByTypeId(int type_id);
};

