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

    // �������ݿ�������������ѯ�������
};



void testQueryAndWriteToFile(int typeID);