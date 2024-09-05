#pragma once
#include "sqlite3.h"
#include <iostream>
#include <fstream>
#include <string>

class DatabaseManager {
private:
    sqlite3* db;
    static DatabaseManager* instance;

    DatabaseManager();

public:
    static DatabaseManager* getInstance();
    sqlite3* getDatabase();

    // 其他数据库操作方法，如查询、插入等
    std::wstring sqlite3_column_wstring(sqlite3_stmt* stmt, int column_index);
    std::string sqlite3_column_string(sqlite3_stmt* stmt, int column_index);

    std::wstring getNameByTypeId(int id);
};



void testQueryAndWriteToFile(int typeID);