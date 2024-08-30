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
};



void testQueryAndWriteToFile(int typeID);