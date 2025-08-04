#pragma once
#include "DatabaseManager.h"

class dynPilotsManager : public SingletonBase<dynPilotsManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	dynPilotsManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	// 假设在 invtypes 表中根据 type_id 查找 name 列
	std::wstring getNameByTypeId(int type_id);
};
