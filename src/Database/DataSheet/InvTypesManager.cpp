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
std::map<int, int> InvTypesManager::getReqSkillsById(int type_id) {
    std::map<int, int> prereqs; // key: 前置技能typeID, value: 所需等级
    std::string query = "SELECT reqskills FROM invtypes WHERE typeID = ?";
    sqlite3* db = DatabaseManager::getInstance()->getDatabase(); // 获取数据库连接
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, type_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* jsonStr = sqlite3_column_text(stmt, 0);
            if (jsonStr) {
                std::string json = reinterpret_cast<const char*>(jsonStr);

                // 解析JSON格式：{"3402": 2, "3413": 3} → 提取<技能ID, 所需等级>
                size_t pos = json.find('{') + 1; // 跳过开头的{
                while (pos < json.size()) {
                    // 查找技能ID的引号
                    size_t idQuoteStart = json.find('"', pos);
                    if (idQuoteStart == std::string::npos) break;

                    size_t idQuoteEnd = json.find('"', idQuoteStart + 1);
                    if (idQuoteEnd == std::string::npos) break;

                    // 提取技能ID
                    std::string idStr = json.substr(idQuoteStart + 1, idQuoteEnd - idQuoteStart - 1);
                    int prereqSkillId = std::stoi(idStr);

                    // 查找所需等级（冒号后的值）
                    size_t colonPos = json.find(':', idQuoteEnd);
                    if (colonPos == std::string::npos) break;

                    size_t levelEnd = json.find(',', colonPos);
                    if (levelEnd == std::string::npos) {
                        levelEnd = json.find('}', colonPos); // 最后一个键值对以}结束
                    }

                    // 提取所需等级
                    std::string levelStr = json.substr(colonPos + 1, levelEnd - colonPos - 1);
                    int requiredLevel = std::stoi(levelStr);

                    prereqs[prereqSkillId] = requiredLevel;
                    pos = levelEnd + 1; // 移动到下一个键值对
                }
            }
        }
    }

    sqlite3_finalize(stmt);
    return prereqs;
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
