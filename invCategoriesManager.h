// InvTypesManager.h
#pragma once
#include "DatabaseManager.h"

class invCategoriesManager : public SingletonBase<invCategoriesManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	invCategoriesManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	// 假设在 invtypes 表中根据 type_id 查找 name 列
	std::vector<int> getAllCategoriesID();
	std::wstring getNameByCategoriesID(int category_id);
};


