#include "dogmaAttributesManager.h"


std::shared_ptr<std::unordered_map<int, AttributeInfo>> dogmaAttributesManager::getAttributes()
{
    auto result = std::make_shared<std::unordered_map<int, AttributeInfo>>();
    std::string query = "SELECT dogmaAttributes.attributeID, dogmaAttributes.categoryID,dogmaAttributes.attributeName,dogmaAttributes.displayNameID FROM dogmaAttributes";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            AttributeInfo temp;
            temp.ID = sqlite3_column_int(stmt, 0);
            temp.CategoryID = sqlite3_column_int(stmt, 1);
            temp.name_en = DatabaseManager::getInstance()->sqlite3_column_wstring(stmt, 2);
            temp.name = DatabaseManager::getInstance()->sqlite3_column_wstring(stmt, 3);
            (*result)[temp.ID] = temp;
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

