#pragma once
#include "include/thirdparty/sqlite/sqlite3.h"
#include <iostream>
#include <fstream>
#include <string>
#include "SingletonBase.h"

class taskDatabaseManager : public SingletonBase<taskDatabaseManager> {
private:
	sqlite3* db;

public:
	taskDatabaseManager() {
		int rc = sqlite3_open("Task.sqlite", &db);
		if (rc) {
			// 可以选择抛出异常或采取其他错误处理方式
		}
		// 进行数据库初始化操作，如创建表等
	}

	sqlite3* getDatabase() {
		return db;
	}
};
