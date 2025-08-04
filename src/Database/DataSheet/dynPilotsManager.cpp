#include "dynPilotsManager.h"

std::wstring dynPilotsManager::getNameByTypeId(int type_id) {
	std::wstring name;
	std::string query = "SELECT Name FROM dynPilots WHERE PilotID =?";
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