// InvTypesManager.cpp
#include "eveBracketsManager.h"

std::string eveBracketsManager::getPathByTypeId(int bracketID) {
	std::string name;
	std::string query = "SELECT dds_path FROM eveBrackets WHERE bracketID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, bracketID);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			name = DatabaseManager::getInstance()->sqlite3_column_string(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return name;
}

std::vector<std::string> eveBracketsManager::getAllDdsPaths()
{
	std::vector<std::string> paths;
	std::string query = "SELECT dds_path FROM eveBrackets";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			std::string path = DatabaseManager::getInstance()->sqlite3_column_string(stmt, 0);
			std::string front = "demoTex\\EVE\\media\\res\\Uprising_V21.03_Icons\\Icons\\items\\Brackets\\dds\\";
			paths.push_back(front + path);
		}
	}
	sqlite3_finalize(stmt);
	return paths;
}