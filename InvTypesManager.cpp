// InvTypesManager.cpp
#include "InvTypesManager.h"

std::wstring InvTypesManager::getNameByTypeId(int type_id) {
	std::wstring name;
	std::string query = "SELECT typeName_zh FROM invtypes WHERE typeID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, type_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			name = DatabaseManager::getInstance()->sqlite3_column_wstring(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return name;
}

int InvTypesManager::getGroupByTypeId(int type_id)
{
	int groupID;
	std::string query = "SELECT groupID FROM invtypes WHERE typeID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, type_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			groupID = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return groupID;
}

std::string getIconPathByTypeID(int typeID)
{
	std::string IconPath = "demoTex\\EVE\\media\\res\\Uprising_V21.03_Types\\Types\\dds\\";
	IconPath += std::to_string(typeID);
	IconPath += "_64.dds";
	return IconPath;
}