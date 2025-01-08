#pragma once
#include "DatabaseManager.h"
#include <vector>
#include <unordered_map>

class Attribute {
public:
	Attribute() = default;

	int attributeID;
	double value;
};

class AttributeInfo {
public:
	AttributeInfo() = default;

	int ID;
	int CategoryID;
	std::wstring name_en;
	std::wstring name;
};

class dogmaAttributesManager : public SingletonBase<dogmaAttributesManager> {
private:
    sqlite3_stmt* stmt = nullptr;
    sqlite3* db;

public:
    dogmaAttributesManager() { db = DatabaseManager::getInstance()->getDatabase(); };

    // 假设在 invtypes 表中根据 type_id 查找 name 列
	std::shared_ptr<std::unordered_map<int, AttributeInfo>> getAttributes();
};

