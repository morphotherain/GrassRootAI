// InvTypesManager.cpp
#include "invEquipmentGroupSlotsManager.h"

// 获取所有的 categoryID
void invEquipmentGroupSlotsManager::loadGroupSlots() {
    std::string query = "SELECT groupID, slot FROM invEquipmentGroupSlots";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int groupID = sqlite3_column_int(stmt, 0);
            int slot = sqlite3_column_int(stmt, 1);
            groupSlotMap[groupID] = slot;
        }
    }

    sqlite3_finalize(stmt);
    stmt = nullptr;
}

int invEquipmentGroupSlotsManager::getSlotByGroupID(int groupID) const {
    auto it = groupSlotMap.find(groupID);
    if (it != groupSlotMap.end()) {
        return it->second;
    }
    return 0;
}