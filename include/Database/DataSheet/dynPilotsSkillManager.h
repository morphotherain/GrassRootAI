#pragma once
#include "DatabaseManager.h"

class dynPilotsSkillManager : public SingletonBase<dynPilotsSkillManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	dynPilotsSkillManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	int getSkillLevelBySkillId(int pilot_id, int skill_id);
};
