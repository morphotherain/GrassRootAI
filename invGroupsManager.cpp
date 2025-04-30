// InvTypesManager.cpp
#include "invGroupsManager.h"

int invGroupsManager::getBracketIDByGroupId(int group_id) {
	int bracketID;
	std::string query = "SELECT bracketID FROM invGroups WHERE groupID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, group_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			bracketID = sqlite3_column_int(stmt, 0);
		}
	}
	else {
		return 0;
	}
	sqlite3_finalize(stmt);
	return bracketID;
}

std::wstring invGroupsManager::getNameByGroupId(int group_id) {
	std::wstring result;
	std::string query = "SELECT groupName FROM invGroups WHERE groupID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, group_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			result = DatabaseManager::getInstance()->sqlite3_column_wstring(stmt, 0);
		}
	}
	else {
		return 0;
	}
	sqlite3_finalize(stmt);
	return result;
}

std::vector<int> invGroupsManager::getAllGroupIDByCategoriesID(int category_id) {
	std::vector<int> groupIDs;
	std::string query = "SELECT groupID FROM invGroups WHERE categoryID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, category_id);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			int groupID = sqlite3_column_int(stmt, 0);
			groupIDs.push_back(groupID);
		}
	}
	sqlite3_finalize(stmt);
	return groupIDs;
}