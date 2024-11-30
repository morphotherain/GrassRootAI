#pragma once
#include "DatabaseManager.h"


class mapSolarSystemsManager : public SingletonBase<mapSolarSystemsManager> {
private:
    sqlite3_stmt* stmt = nullptr;
    sqlite3* db;

public:
    mapSolarSystemsManager() { db = DatabaseManager::getInstance()->getDatabase(); };

    // 假设在 invtypes 表中根据 type_id 查找 name 列
    std::wstring getNameById(int type_id);
};

