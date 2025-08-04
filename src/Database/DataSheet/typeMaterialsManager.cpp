#include "typeMaterialsManager.h"

void typeMaterialsManager::getRefiningResultById(int typeId, std::vector<std::pair<int, int>>& materialTypes) {

	std::string query = "SELECT materialTypeID, quantity FROM typeMaterials WHERE typeID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, typeId);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
		}
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			int typeId = (sqlite3_column_int(stmt, 0));
			int amount = (sqlite3_column_int(stmt, 1));
			materialTypes.push_back({typeId, amount});
		}
	}
	sqlite3_finalize(stmt);
}