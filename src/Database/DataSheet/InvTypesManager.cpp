// InvTypesManager.cpp
#include "InvTypesManager.h"

std::wstring InvTypesManager::getNameByTypeId(int type_id) {
	std::wstring name;
	std::string query = "SELECT typeName_zh FROM invtypes WHERE typeID =?";
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

int InvTypesManager::getGroupByTypeId(int type_id)
{
	int groupID = -1;
	std::string query = "SELECT groupID FROM invtypes WHERE typeID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, type_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			groupID = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return groupID;
}

std::vector<int> InvTypesManager::getAllTypeIDByGroupID(int group_id) {
	std::vector<int> typeIDs;
	std::string query = "SELECT typeID FROM invTypes WHERE groupID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, group_id);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			int typeID = sqlite3_column_int(stmt, 0);
			typeIDs.push_back(typeID);
		}
	}
	sqlite3_finalize(stmt);
	return typeIDs;
}

int InvTypesManager::getMinTypeIdByGroupIDIcon(int group_id)
{
	int typeID = 0;
	std::string query = "SELECT MIN(it.typeID) FROM invtypes it JOIN invGroups ig ON it.iconID = ig.iconID WHERE ig.groupID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, group_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			typeID = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return typeID;
}

int InvTypesManager::getMinTypeIdByCategoryIDIcon(int category_id)
{
	int typeID = 0;
	std::string query = "SELECT MIN(it.typeID) FROM invtypes it JOIN invCategories ig ON it.iconID = ig.iconID WHERE ig.categoryID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, category_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			typeID = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return typeID;
}

std::string getIconPathByTypeID(int typeID)
{
	std::string IconPath = "demoTex\\EVE\\media\\res\\Uprising_V21.03_Types\\Types\\dds\\";
	IconPath += std::to_string(typeID);
	IconPath += "_64.dds";
	return IconPath;
}



// 解析reqskills列，返回所需技能的typeID列表
std::vector<int> InvTypesManager::getReqSkillsById(int type_id) {
    std::vector<int> result;
    std::string query = "SELECT reqskills FROM invtypes WHERE typeID = ?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, type_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // 获取JSON字符串
            const unsigned char* jsonStr = sqlite3_column_text(stmt, 0);
            if (jsonStr) {
                std::string json = reinterpret_cast<const char*>(jsonStr);

                // 简单解析JSON格式的字符串 {"3402": 2, "3413": 3}
                size_t pos = 0;
                // 跳过开头的 {
                if (json.find('{') != std::string::npos) {
                    pos = json.find('{') + 1;
                }

                // 循环解析每个键值对
                while (pos < json.size()) {
                    // 查找引号
                    size_t quoteStart = json.find('"', pos);
                    if (quoteStart == std::string::npos) break;

                    size_t quoteEnd = json.find('"', quoteStart + 1);
                    if (quoteEnd == std::string::npos) break;

                    // 提取引号间的数字字符串并转换为int
                    std::string idStr = json.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
                    try {
                        int id = std::stoi(idStr);
                        result.push_back(id);
                    }
                    catch (...) {
                        // 忽略转换失败的情况
                    }

                    // 移动到下一个可能的键值对
                    pos = json.find(',', quoteEnd) + 1;
                    if (pos == 0) break; // 没找到逗号，说明是最后一个
                }
            }
        }
    }

    sqlite3_finalize(stmt);
    return result;
}

// 解析requiredfor列，返回需要该物品的typeID列表
std::vector<int> InvTypesManager::getRequiredforById(int type_id) {
    std::vector<int> result;
    std::string query = "SELECT requiredfor FROM invtypes WHERE typeID = ?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, type_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // 获取JSON字符串
            const unsigned char* jsonStr = sqlite3_column_text(stmt, 0);
            if (jsonStr) {
                std::string json = reinterpret_cast<const char*>(jsonStr);

                // 使用与getReqSkillsById相同的解析逻辑
                size_t pos = 0;
                if (json.find('{') != std::string::npos) {
                    pos = json.find('{') + 1;
                }

                while (pos < json.size()) {
                    size_t quoteStart = json.find('"', pos);
                    if (quoteStart == std::string::npos) break;

                    size_t quoteEnd = json.find('"', quoteStart + 1);
                    if (quoteEnd == std::string::npos) break;

                    std::string idStr = json.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
                    try {
                        int id = std::stoi(idStr);
                        result.push_back(id);
                    }
                    catch (...) {
                        // 忽略转换失败的情况
                    }

                    pos = json.find(',', quoteEnd) + 1;
                    if (pos == 0) break;
                }
            }
        }
    }

    sqlite3_finalize(stmt);
    return result;
}
