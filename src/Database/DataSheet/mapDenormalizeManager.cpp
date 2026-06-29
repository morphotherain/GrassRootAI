#include "mapDenormalizeManager.h"

unsigned int mapDenormalizeManager::getTypeIDByContainerId(int container_id)
{
	unsigned int  id;
	std::string query = "SELECT typeID FROM mapDenormalize WHERE itemID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, container_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			id = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return id;
}

unsigned int mapDenormalizeManager::getSolarSystemIDByObjectId(int object_id)
{
	unsigned int  id;
	std::string query = "SELECT solarSystemID FROM mapDenormalize WHERE itemID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, object_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			id = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return id;
}

std::vector<double> mapDenormalizeManager::getPosByObjectID(int object_id) {
	std::vector<double> pos(3, 0.0f); // 初始化vector，长度为3，初始值都为0.0f，表示三维坐标
	std::string query = "SELECT x, y, z FROM mapDenormalize WHERE itemID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, object_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			pos[0] = (sqlite3_column_double(stmt, 0));
			pos[1] = (sqlite3_column_double(stmt, 1));
			pos[2] = (sqlite3_column_double(stmt, 2));
		}
	}
	sqlite3_finalize(stmt);
	return pos;
}

std::vector<MapStaticObjectRow> mapDenormalizeManager::queryStaticObjectsBySolarSystemID(int solarSystemID)
{
	std::vector<MapStaticObjectRow> rows;
	const std::string sql =
		"SELECT x, y, z, nameID, regionID, constellationID, solarSystemID, radius, itemID, "
		"mapDenormalize.typeID, celestialIndex, orbitIndex, invtypes.groupID, invGroups.categoryID "
		"FROM mapDenormalize "
		"JOIN invtypes ON mapDenormalize.typeID = invtypes.typeID "
		"JOIN invGroups ON invtypes.groupID = invGroups.groupID "
		"WHERE solarSystemID = ?;";

	sqlite3_stmt* queryStmt = nullptr;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &queryStmt, nullptr);
	if (rc != SQLITE_OK)
	{
		return rows;
	}

	sqlite3_bind_int(queryStmt, 1, solarSystemID);
	while ((rc = sqlite3_step(queryStmt)) == SQLITE_ROW)
	{
		MapStaticObjectRow row;
		row.denormalize.x = sqlite3_column_double(queryStmt, 0);
		row.denormalize.y = sqlite3_column_double(queryStmt, 1);
		row.denormalize.z = sqlite3_column_double(queryStmt, 2);
		row.denormalize.nameID = sqlite3_column_int(queryStmt, 3);
		row.denormalize.regionID = sqlite3_column_int(queryStmt, 4);
		row.denormalize.constellationID = sqlite3_column_int(queryStmt, 5);
		row.denormalize.solarSystemID = sqlite3_column_int(queryStmt, 6);
		row.denormalize.radius = sqlite3_column_double(queryStmt, 7);
		row.denormalize.itemID = sqlite3_column_int(queryStmt, 8);
		row.denormalize.typeID = sqlite3_column_int(queryStmt, 9);
		row.denormalize.celestialIndex = sqlite3_column_int(queryStmt, 10);
		row.denormalize.orbitIndex = sqlite3_column_int(queryStmt, 11);
		row.denormalize.fillDisplayName();

		row.gameObject.x = row.denormalize.x;
		row.gameObject.y = row.denormalize.y;
		row.gameObject.z = row.denormalize.z;
		row.gameObject.SolarSystemID = row.denormalize.solarSystemID;
		row.gameObject.ContainerID = row.denormalize.solarSystemID;
		row.gameObject.objectID = row.denormalize.itemID;
		row.gameObject.typeID = row.denormalize.typeID;
		row.gameObject.groupID = sqlite3_column_int(queryStmt, 12);
		row.gameObject.categoryID = sqlite3_column_int(queryStmt, 13);
		row.gameObject.name = row.denormalize.name;
		rows.push_back(row);
	}

	sqlite3_finalize(queryStmt);
	return rows;
}