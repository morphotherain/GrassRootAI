#include "dynPilotsSkillManager.h"

int dynPilotsSkillManager::getSkillLevelBySkillId(int pilot_id, int skill_id) {
	int skillLevel = -1;
	std::string query = "SELECT SkillLevel FROM dynPilotSkill WHERE PilotID = ? AND  typeID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, pilot_id);
		sqlite3_bind_int(stmt, 2, skill_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			skillLevel = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return skillLevel;
}

std::vector<std::pair<int, int>> dynPilotsSkillManager::getAllSkillsByPilotId(int pilot_id) {
    std::vector<std::pair<int, int>> skills; // 存储<技能ID, 技能等级>的向量
    std::string query = "SELECT typeID, SkillLevel FROM dynPilotSkill WHERE PilotID = ?";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, pilot_id);

        // 循环获取所有匹配的行
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int skill_id = sqlite3_column_int(stmt, 0);
            int skill_level = sqlite3_column_int(stmt, 1);
            skills.emplace_back(skill_id, skill_level);
        }
    }

    sqlite3_finalize(stmt);
    return skills;
}

bool dynPilotsSkillManager::addOrUpdateSkill(int pilot_id, int skill_id, int level) {
    // 先检查技能是否存在
    int currentLevel = getSkillLevelBySkillId(pilot_id, skill_id);

    std::string query;
    if (currentLevel == -1) {
        // 技能不存在，插入新记录
        query = "INSERT INTO dynPilotSkill (PilotID, typeID, SkillLevel) VALUES (?, ?, ?)";
    }
    else {
        // 技能存在，更新等级
        query = "UPDATE dynPilotSkill SET SkillLevel = ? WHERE PilotID = ? AND typeID = ?";
    }

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }

    if (currentLevel == -1) {
        sqlite3_bind_int(stmt, 1, pilot_id);
        sqlite3_bind_int(stmt, 2, skill_id);
        sqlite3_bind_int(stmt, 3, level);
    }
    else {
        sqlite3_bind_int(stmt, 1, level);
        sqlite3_bind_int(stmt, 2, pilot_id);
        sqlite3_bind_int(stmt, 3, skill_id);
    }

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

// 更新已有技能的等级
bool dynPilotsSkillManager::updateSkillLevel(int pilot_id, int skill_id, int new_level) {
    // 确保技能存在
    int currentLevel = getSkillLevelBySkillId(pilot_id, skill_id);
    if (currentLevel == -1) {
        return false; // 技能不存在
    }

    // 确保新等级高于当前等级（根据业务需求可调整）
    if (new_level <= currentLevel) {
        return false;
    }

    std::string query = "UPDATE dynPilotSkill SET SkillLevel = ? WHERE PilotID = ? AND typeID = ?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_int(stmt, 1, new_level);
    sqlite3_bind_int(stmt, 2, pilot_id);
    sqlite3_bind_int(stmt, 3, skill_id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}