// InvTypesManager.h
#pragma once
#include "DatabaseManager.h"
#include <vector>

class eveBracketsManager : public SingletonBase<eveBracketsManager> {
private:
    sqlite3_stmt* stmt;
    sqlite3* db;

public:
    eveBracketsManager() { db = DatabaseManager::getInstance()->getDatabase(); };

    // 假设在 invtypes 表中根据 type_id 查找 name 列
    std::string getPathByTypeId(int bracketID);
    std::vector<std::string> getAllDdsPaths();
};

