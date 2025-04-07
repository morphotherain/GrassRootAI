#include "mapJumpsManager.h"

int mapJumpsManager::getDestinationIDByObjectId(int type_id) {
	int id;
	std::string query = "SELECT destinationID FROM mapJumps WHERE stargateID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, type_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			id = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return id;
}