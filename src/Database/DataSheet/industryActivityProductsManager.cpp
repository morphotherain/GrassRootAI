#include "industryActivityProductsManager.h"

std::vector<IndustryProduct> industryActivityProductsManager::getProductsByBlueprint(int blueprintTypeID, int activityID)
{
	std::vector<IndustryProduct> result;
	const char* sql = "SELECT productTypeID, quantity, probability FROM industryActivityProducts WHERE blueprintTypeID = ? AND activityID = ?";
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, blueprintTypeID);
		sqlite3_bind_int(stmt, 2, activityID);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			IndustryProduct p{};
			p.productTypeID = sqlite3_column_int(stmt, 0);
			p.quantity = sqlite3_column_int(stmt, 1);
			p.probability = sqlite3_column_double(stmt, 2);
			result.push_back(p);
		}
	}
	sqlite3_finalize(stmt);
	return result;
}

