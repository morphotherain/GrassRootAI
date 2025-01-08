#include "dogmaTypeAttributesManager.h"

std::shared_ptr<std::unordered_map<int, Attribute>> dogmaTypeAttributesManager::getTypeAttributesByTypeId(int typeID)
{
    std::shared_ptr<std::unordered_map<int, Attribute>> result = std::make_shared<std::unordered_map<int, Attribute>>(); 
    std::string query = "SELECT attributeID,value FROM dogmaTypeAttributes WHERE typeID = ?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, typeID);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Attribute temp;
            temp.attributeID = sqlite3_column_int(stmt, 0);
            temp.value = sqlite3_column_int(stmt, 1);
            (*result)[temp.attributeID] = temp;
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

