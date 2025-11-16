#pragma once
#include "DatabaseManager.h"
#include <vector>

struct IndustryMaterial {
	int materialTypeID;
	int quantity;
};

class industryActivityMaterialsManager : public SingletonBase<industryActivityMaterialsManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	industryActivityMaterialsManager() { db = DatabaseManager::getInstance()->getDatabase(); }

	std::vector<IndustryMaterial> getMaterialsByBlueprint(int blueprintTypeID, int activityID = 1);
};

