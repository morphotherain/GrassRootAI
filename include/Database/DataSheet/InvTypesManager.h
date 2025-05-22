// InvTypesManager.h
#pragma once
#include "DatabaseManager.h"

class InvTypesManager : public SingletonBase<InvTypesManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	InvTypesManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	// 假设在 invtypes 表中根据 type_id 查找 name 列
	std::wstring getNameByTypeId(int type_id);
	int getGroupByTypeId(int type_id);
	std::vector<int> getAllTypeIDByGroupID(int group_id);

	int getMinTypeIdByGroupIDIcon(int group_id);
	int getMinTypeIdByCategoryIDIcon(int category_id);
};

std::string getIconPathByTypeID(int typeID);