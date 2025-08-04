#include "dynPilotsSkillManager.h"

int dynPilotsSkillManager::getSkillLevelBySkillId(int pilot_id, int skill_id) {
	int skillLevel = -1;
	std::string query = "SELECT SkillLevel FROM dynPilotsSkill WHERE PilotID = ? AND  typeID =?";
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