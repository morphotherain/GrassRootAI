
#include "mapDenormalizeManager.h"

unsigned int mapDenormalizeManager::getTypeIDByContainerId(int container_id)
{
    unsigned int  id;
    std::string query = "SELECT typeID FROM mapDenormalize WHERE itemID =?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, container_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return id;
}
