#pragma once
#include "DatabaseManager.h"
#include <vector>

class mapDenormalizeManager : public SingletonBase<mapDenormalizeManager> {
private:
    sqlite3_stmt* stmt;
    sqlite3* db;

public:
    mapDenormalizeManager() { db = DatabaseManager::getInstance()->getDatabase(); };

    unsigned int getTypeIDByContainerId(int container_id);


};

