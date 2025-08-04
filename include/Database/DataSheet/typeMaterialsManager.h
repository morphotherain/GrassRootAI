#pragma once
#include "DatabaseManager.h"
#include <vector>

class typeMaterialsManager : public SingletonBase<typeMaterialsManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	typeMaterialsManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	// 假设在 invtypes 表中根据 type_id 查找 name 列
	void getRefiningResultById(int typeId, std::vector<std::pair<int, int>>& materialTypes);
};
