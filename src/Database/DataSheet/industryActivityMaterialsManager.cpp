#include "industryActivityMaterialsManager.h"

std::vector<IndustryMaterial> industryActivityMaterialsManager::getMaterialsByBlueprint(int blueprintTypeID, int activityID)
{
	std::vector<IndustryMaterial> result;
	const char* sql = "SELECT materialTypeID, quantity FROM industryActivityMaterials WHERE blueprintTypeID = ? AND activityID = ?";
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, blueprintTypeID);
		sqlite3_bind_int(stmt, 2, activityID);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			IndustryMaterial m{};
			m.materialTypeID = sqlite3_column_int(stmt, 0);
			m.quantity = sqlite3_column_int(stmt, 1);
			result.push_back(m);
		}
	}
	sqlite3_finalize(stmt);
	return result;
}

