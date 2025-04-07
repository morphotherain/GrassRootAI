#pragma once

#include "sqlite3.h"
#include <iostream>
#include <fstream>
#include <string>
#include "SingletonBase.h"
#include "d3dUtil.h"

class DatabaseManager : public SingletonBase<DatabaseManager> {
private:
	sqlite3* db;

public:
	DatabaseManager() {
		int rc = sqlite3_open("EVEdata.sqlite", &db);
		if (rc) {
			// 可以选择抛出异常或采取其他错误处理方式
		}
		// 进行数据库初始化操作，如创建表等
	}

	sqlite3* getDatabase() {
		return db;
	}

	// 其他数据库操作方法，如查询、插入等
	std::wstring sqlite3_column_wstring(sqlite3_stmt* stmt, int column_index);
	std::string sqlite3_column_string(sqlite3_stmt* stmt, int column_index);

	std::wstring getNameByTypeId(int id);
};

void testQueryAndWriteToFile(int typeID);