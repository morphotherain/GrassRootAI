// InvTypesManager.h
#pragma once
#include "DatabaseManager.h"

class invGroupsManager : public SingletonBase<invGroupsManager> {
private:
    sqlite3_stmt* stmt = nullptr;
    sqlite3* db;

public:
    invGroupsManager() { db = DatabaseManager::getInstance()->getDatabase(); };

    // 假设在 invtypes 表中根据 type_id 查找 name 列
    int getBracketIDByGroupId(int group_id);
    std::wstring getNameByGroupId(int group_id);
};

