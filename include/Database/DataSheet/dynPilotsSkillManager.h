#pragma once
#include "DatabaseManager.h"

class dynPilotsSkillManager : public SingletonBase<dynPilotsSkillManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	dynPilotsSkillManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	int getSkillLevelBySkillId(int pilot_id, int skill_id);
	std::vector<std::pair<int, int>> getAllSkillsByPilotId(int pilot_id);

	bool updateSkillLevel(int pilot_id, int skill_id, int new_level);
	bool addOrUpdateSkill(int pilot_id, int skill_id, int level);
};
