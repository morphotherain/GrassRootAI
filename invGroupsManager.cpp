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
    sqlite3_finalize(stmt);
    return bracketID;
}

