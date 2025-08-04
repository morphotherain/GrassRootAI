#pragma once
#include "DatabaseManager.h"
#include <vector>

// 技能队列数据结构
struct DynSkillQueue {
    int skillTypeId;
    int priority;
    int process;
    int pilotId;
    int level;
    int factor;
};

class dynSkillQueueManager : public SingletonBase<dynSkillQueueManager> {
private:
    sqlite3_stmt* stmt = nullptr;
    sqlite3* db;

public:
    dynSkillQueueManager() { db = DatabaseManager::getInstance()->getDatabase(); };

    // 1. 根据飞行员id返回当前队列中所有技能（优先级不为-1）并按优先级由小到大排列
    std::vector<DynSkillQueue> getActiveSkillsByPilotId(int pilotId);

    // 2. 根据飞行员id返回不在当前队列中所有技能（优先级为-1）
    std::vector<DynSkillQueue> getInactiveSkillsByPilotId(int pilotId);

    // 3. 回写当前队列中技能
    bool updateSkills(int pilotId, const std::vector<DynSkillQueue>& skills);

private:
    // 内部辅助方法：删除指定飞行员的所有技能记录
    bool deleteSkillsByPilotId(int pilotId);

    // 内部辅助方法：批量插入技能记录
    bool batchInsertSkills(const std::vector<DynSkillQueue>& skills);
};
