#include "industryActivitiesManager.h"

int industryActivitiesManager::getTimeByBlueprint(int blueprintTypeID, int activityID)
{
	int timeSeconds = 0;
	const char* sql = "SELECT time FROM industryActivities WHERE blueprintTypeID = ? AND activityID = ?";
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, blueprintTypeID);
		sqlite3_bind_int(stmt, 2, activityID);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			timeSeconds = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return timeSeconds;
}

