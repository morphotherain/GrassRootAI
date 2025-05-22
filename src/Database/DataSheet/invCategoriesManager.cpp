// InvTypesManager.cpp
#include "invCategoriesManager.h"

// 获取所有的 categoryID
std::vector<int> invCategoriesManager::getAllCategoriesID() {
	std::vector<int> categoryIDs;
	std::string query = "SELECT categoryID FROM invCategories";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			int categoryID = sqlite3_column_int(stmt, 0);
			categoryIDs.push_back(categoryID);
		}
	}
	sqlite3_finalize(stmt);
	return categoryIDs;
}

// 根据 categoryID 获取 categoryName
std::wstring invCategoriesManager::getNameByCategoriesID(int category_id) {
	std::wstring result;
	std::string query = "SELECT categoryName FROM invCategories WHERE categoryID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, category_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			result = DatabaseManager::getInstance()->sqlite3_column_wstring(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return result;
}