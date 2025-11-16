#pragma once
#include "DatabaseManager.h"
#include <vector>

struct IndustryProduct {
	int productTypeID;
	int quantity;
	double probability;
};

class industryActivityProductsManager : public SingletonBase<industryActivityProductsManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	industryActivityProductsManager() { db = DatabaseManager::getInstance()->getDatabase(); }

	std::vector<IndustryProduct> getProductsByBlueprint(int blueprintTypeID, int activityID = 1);
};

