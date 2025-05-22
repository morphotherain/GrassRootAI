#pragma once
#include "DatabaseManager.h"
#include "dogmaAttributesManager.h"

class dogmaTypeAttributesManager : public SingletonBase<dogmaTypeAttributesManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	dogmaTypeAttributesManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	// 假设在 invtypes 表中根据 type_id 查找 name 列
	std::shared_ptr<std::unordered_map<int, Attribute>> getTypeAttributesByTypeId(int typeID);
};
