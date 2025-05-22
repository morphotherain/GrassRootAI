#pragma once
#include "DatabaseManager.h"
#include <vector>

class mapDenormalizeManager : public SingletonBase<mapDenormalizeManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	mapDenormalizeManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	unsigned int getTypeIDByContainerId(int container_id);
	unsigned int getSolarSystemIDByObjectId(int object_id);
	std::vector<double> getPosByObjectID(int object_id);
};
