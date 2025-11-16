#pragma once
#include "DatabaseManager.h"

class industryActivitiesManager : public SingletonBase<industryActivitiesManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	industryActivitiesManager() { db = DatabaseManager::getInstance()->getDatabase(); }

	// 返回单位：秒；若不存在返回0
	int getTimeByBlueprint(int blueprintTypeID, int activityID = 1);
};

