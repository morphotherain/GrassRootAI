#include "dynSkillQueueManager.h"

std::vector<DynSkillQueue> dynSkillQueueManager::getActiveSkillsByPilotId(int pilotId) {
    std::vector<DynSkillQueue> result;
    std::string query = "SELECT skillTypeId, priority, process, PilotId, level, factor FROM dynSkillQueue "
        "WHERE PilotId = ? AND priority != -1 "
        "ORDER BY priority ASC";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, pilotId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            DynSkillQueue skill;
            skill.skillTypeId = sqlite3_column_int(stmt, 0);
            skill.priority = sqlite3_column_int(stmt, 1);
            skill.process = sqlite3_column_int(stmt, 2);
            skill.pilotId = sqlite3_column_int(stmt, 3);
            skill.level = sqlite3_column_int(stmt, 4);
            skill.factor = sqlite3_column_int(stmt, 5);
            result.push_back(skill);
        }
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<DynSkillQueue> dynSkillQueueManager::getInactiveSkillsByPilotId(int pilotId) {
    std::vector<DynSkillQueue> result;
    std::string query = "SELECT skillTypeId, priority, process, PilotId, level, factor FROM dynSkillQueue "
        "WHERE PilotId = ? AND priority = -1";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, pilotId);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            DynSkillQueue skill;
            skill.skillTypeId = sqlite3_column_int(stmt, 0);
            skill.priority = sqlite3_column_int(stmt, 1);
            skill.process = sqlite3_column_int(stmt, 2);
            skill.pilotId = sqlite3_column_int(stmt, 3);
            skill.level = sqlite3_column_int(stmt, 4);
            skill.factor = sqlite3_column_int(stmt, 5);
            result.push_back(skill);
        }
    }

    sqlite3_finalize(stmt);
    return result;
}

bool dynSkillQueueManager::updateSkills(int pilotId, const std::vector<DynSkillQueue>& skills) {
    // 开始事务
    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    // 先删除该飞行员的所有记录
    if (!deleteSkillsByPilotId(pilotId)) {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        return false;
    }

    // 插入所有活跃技能
    bool success = batchInsertSkills(skills);

    // 根据操作结果提交或回滚事务
    if (success) {
        sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    }
    else {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
    }

    return success;
}


bool dynSkillQueueManager::deleteSkillsByPilotId(int pilotId) {
    std::string query = "DELETE FROM dynSkillQueue WHERE PilotId = ?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_int(stmt, 1, pilotId);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool dynSkillQueueManager::batchInsertSkills(const std::vector<DynSkillQueue>& skills) {
    std::string query = "INSERT INTO dynSkillQueue (skillTypeId, priority, process, PilotId, level, factor) "
        "VALUES (?, ?, ?, ?, ?, ?)";

    for (const auto& skill : skills) {
        int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_bind_int(stmt, 1, skill.skillTypeId);
        sqlite3_bind_int(stmt, 2, skill.priority);
        sqlite3_bind_int(stmt, 3, skill.process);
        sqlite3_bind_int(stmt, 4, skill.pilotId);
        sqlite3_bind_int(stmt, 5, skill.level);
        sqlite3_bind_int(stmt, 6, skill.factor);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            return false;
        }
    }

    return true;
}
