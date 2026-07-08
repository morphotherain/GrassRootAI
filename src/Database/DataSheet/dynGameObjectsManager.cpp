#include "dynGameObjectsManager.h"
#include "dynContainersManager.h"
#include <unordered_set>

namespace
{
	const char* kSelectWithJoin =
		"SELECT dynGameObjects.ObjectID, dynGameObjects.typeID, "
		"dynGameObjects.x, dynGameObjects.y, dynGameObjects.z, "
		"dynGameObjects.SolarSystemID, dynGameObjects.OwnerKind, dynGameObjects.OwnerID, "
		"dynGameObjects.LocationKind, dynGameObjects.LocationRef, "
		"dynGameObjects.qw, dynGameObjects.qx, dynGameObjects.qy, dynGameObjects.qz, "
		"invtypes.groupID, invGroups.categoryID "
		"FROM dynGameObjects "
		"JOIN invtypes ON dynGameObjects.typeID = invtypes.typeID "
		"JOIN invGroups ON invtypes.groupID = invGroups.groupID ";

	void MapRowToDynGameObject(sqlite3_stmt* statement, dynGameObject& row)
	{
		row.objectID = sqlite3_column_int(statement, 0);
		row.typeID = sqlite3_column_int(statement, 1);
		row.x = sqlite3_column_double(statement, 2);
		row.y = sqlite3_column_double(statement, 3);
		row.z = sqlite3_column_double(statement, 4);
		row.SolarSystemID = sqlite3_column_int(statement, 5);
		row.OwnerKind = sqlite3_column_int(statement, 6);
		row.OwnerID = sqlite3_column_int(statement, 7);
		row.LocationKind = sqlite3_column_int(statement, 8);
		row.LocationRef = sqlite3_column_int(statement, 9);
		row.qw = sqlite3_column_double(statement, 10);
		row.qx = sqlite3_column_double(statement, 11);
		row.qy = sqlite3_column_double(statement, 12);
		row.qz = sqlite3_column_double(statement, 13);
		row.groupID = sqlite3_column_int(statement, 14);
		row.categoryID = sqlite3_column_int(statement, 15);
	}
}

unsigned int dynGameObjectsManager::getTypeIdByObjectID(int object_id)
{
	unsigned int id = 0;
	std::string query = "SELECT TypeID FROM dynGameObjects WHERE ObjectID =?";
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

unsigned int dynGameObjectsManager::getSolarSystemIdByObjectID(int object_id)
{
	unsigned int id = 0;
	std::string query = "SELECT SolarSystemID FROM dynGameObjects WHERE ObjectID =?";
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

int dynGameObjectsManager::updatePlacementByObjectID(
	int object_id,
	unsigned int solarSystemId,
	unsigned int ownerKind,
	unsigned int ownerId,
	unsigned int locationKind,
	unsigned int locationRef)
{
	std::string query =
		"UPDATE dynGameObjects SET SolarSystemID=?, OwnerKind=?, OwnerID=?, "
		"LocationKind=?, LocationRef=? WHERE ObjectID=?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Error preparing placement update for object ID " << object_id << ": "
			<< sqlite3_errmsg(db) << std::endl;
		return -1;
	}

	sqlite3_bind_int(stmt, 1, solarSystemId);
	sqlite3_bind_int(stmt, 2, ownerKind);
	sqlite3_bind_int(stmt, 3, ownerId);
	sqlite3_bind_int(stmt, 4, locationKind);
	sqlite3_bind_int(stmt, 5, locationRef);
	sqlite3_bind_int(stmt, 6, object_id);

	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if (rc != SQLITE_DONE) {
		std::cerr << "Error updating placement for object ID " << object_id << std::endl;
		return -1;
	}
	return 0;
}

std::vector<double> dynGameObjectsManager::getPosByObjectID(int object_id) {
	std::vector<double> pos(3, 0.0);
	std::string query = "SELECT x, y, z FROM dynGameObjects WHERE ObjectID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, object_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			pos[0] = sqlite3_column_double(stmt, 0);
			pos[1] = sqlite3_column_double(stmt, 1);
			pos[2] = sqlite3_column_double(stmt, 2);
		}
	}
	sqlite3_finalize(stmt);
	return pos;
}

int dynGameObjectsManager::updatePosByObjectID(int object_id, const std::vector<double>& pos) {
	std::string query = "UPDATE dynGameObjects SET x =?, y =?, z =? WHERE ObjectID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_double(stmt, 1, pos[0]);
		sqlite3_bind_double(stmt, 2, pos[1]);
		sqlite3_bind_double(stmt, 3, pos[2]);
		sqlite3_bind_int(stmt, 4, object_id);

		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		if (rc != SQLITE_DONE) {
			return -1;
		}
	}
	else {
		return -1;
	}
	return 0;
}

std::vector<double> dynGameObjectsManager::getQuaternionByObjectID(int object_id) {
	std::vector<double> quaternion(4, 0.0);
	std::string query = "SELECT qw, qx, qy, qz FROM dynGameObjects WHERE ObjectID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, object_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			quaternion[0] = sqlite3_column_double(stmt, 0);
			quaternion[1] = sqlite3_column_double(stmt, 1);
			quaternion[2] = sqlite3_column_double(stmt, 2);
			quaternion[3] = sqlite3_column_double(stmt, 3);
		}
	}
	sqlite3_finalize(stmt);
	return quaternion;
}

bool dynGameObjectsManager::queryObjectsByBagId(int bagId, std::vector<std::pair<int, int>>& result)
{
	sqlite3_stmt* queryStmt = nullptr;
	std::string query =
		"SELECT objectID, typeID FROM dynGameObjects "
		"WHERE LocationKind = ? AND LocationRef = ?;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &queryStmt, nullptr);
	if (rc != SQLITE_OK) {
		return false;
	}

	sqlite3_bind_int(queryStmt, 1, static_cast<int>(AssetLocationKind::ContainerBag));
	sqlite3_bind_int(queryStmt, 2, bagId);

	while ((rc = sqlite3_step(queryStmt)) == SQLITE_ROW) {
		result.emplace_back(sqlite3_column_int(queryStmt, 0), sqlite3_column_int(queryStmt, 1));
	}

	sqlite3_finalize(queryStmt);
	return rc == SQLITE_DONE;
}

dynGameObject dynGameObjectsManager::getGameObjectByObjectID(int object_id)
{
	dynGameObject result;
	std::string query = std::string(kSelectWithJoin) + "WHERE dynGameObjects.ObjectID =?;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, object_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			MapRowToDynGameObject(stmt, result);
		}
	}
	sqlite3_finalize(stmt);
	return result;
}

int dynGameObjectsManager::insertGameObject(const dynGameObject& obj) {
	std::string getMaxIDQuery = "SELECT MAX(ObjectID) FROM dynGameObjects;";
	int rc = sqlite3_prepare_v2(db, getMaxIDQuery.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		return -1;
	}

	int nextObjectID = 1;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		nextObjectID = sqlite3_column_int(stmt, 0) + 1;
	}
	sqlite3_finalize(stmt);

	std::string insertQuery =
		"INSERT INTO dynGameObjects (ObjectID, typeID, x, y, z, SolarSystemID, "
		"OwnerKind, OwnerID, LocationKind, LocationRef, qw, qx, qy, qz) "
		"VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?);";
	rc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		return -1;
	}

	sqlite3_bind_int(stmt, 1, nextObjectID);
	sqlite3_bind_int(stmt, 2, obj.typeID);
	sqlite3_bind_double(stmt, 3, obj.x);
	sqlite3_bind_double(stmt, 4, obj.y);
	sqlite3_bind_double(stmt, 5, obj.z);
	sqlite3_bind_int(stmt, 6, obj.SolarSystemID);
	sqlite3_bind_int(stmt, 7, obj.OwnerKind);
	sqlite3_bind_int(stmt, 8, obj.OwnerID);
	sqlite3_bind_int(stmt, 9, obj.LocationKind);
	sqlite3_bind_int(stmt, 10, obj.LocationRef);
	sqlite3_bind_double(stmt, 11, obj.qw);
	sqlite3_bind_double(stmt, 12, obj.qx);
	sqlite3_bind_double(stmt, 13, obj.qy);
	sqlite3_bind_double(stmt, 14, obj.qz);

	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if (rc != SQLITE_DONE) {
		return -1;
	}
	return nextObjectID;
}

bool dynGameObjectsManager::deleteGameObject(int objectID) {
	std::string deleteQuery = "DELETE FROM dynGameObjects WHERE ObjectID =?;";
	int rc = sqlite3_prepare_v2(db, deleteQuery.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		return false;
	}

	sqlite3_bind_int(stmt, 1, objectID);
	rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	return rc == SQLITE_DONE;
}

unsigned int dynGameObjectsManager::getPilotObjectIDByPilotID(int pilot_id)
{
	unsigned int id = 0;
	std::string query =
		"SELECT ObjectID FROM dynGameObjects WHERE OwnerKind=? AND OwnerID=? AND TypeID=164";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, static_cast<int>(AssetOwnerKind::Character));
		sqlite3_bind_int(stmt, 2, pilot_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			id = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	return id;
}

std::shared_ptr<std::vector<dynGameObject>> dynGameObjectsManager::getGameObjectBySolarSystemID(UINT solarSystemID)
{
	auto result = std::make_shared<std::vector<dynGameObject>>();
	std::string query = std::string(kSelectWithJoin) + "WHERE dynGameObjects.SolarSystemID =?;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, solarSystemID);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			dynGameObject temp;
			MapRowToDynGameObject(stmt, temp);
			result->push_back(temp);
		}
	}
	sqlite3_finalize(stmt);
	return result;
}

std::shared_ptr<std::vector<dynGameObject>> dynGameObjectsManager::getPilots()
{
	auto result = std::make_shared<std::vector<dynGameObject>>();
	std::string query = std::string(kSelectWithJoin) + "WHERE invTypes.GroupID = 23;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			dynGameObject temp;
			MapRowToDynGameObject(stmt, temp);
			result->push_back(temp);
		}
	}
	sqlite3_finalize(stmt);
	return result;
}

std::vector<int> dynGameObjectsManager::getItemsByBagId(UINT bagId)
{
	std::vector<int> result;
	std::string query =
		"SELECT ObjectID FROM dynGameObjects WHERE LocationKind=? AND LocationRef=?;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, static_cast<int>(AssetLocationKind::ContainerBag));
		sqlite3_bind_int(stmt, 2, static_cast<int>(bagId));
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			result.push_back(sqlite3_column_int(stmt, 0));
		}
	}
	sqlite3_finalize(stmt);
	return result;
}

std::vector<int> dynGameObjectsManager::getSolarSystemIDHasPilot()
{
	std::vector<int> result;
	std::unordered_set<int> uniqueIDs;

	auto pilots = getPilots();
	for (const auto& pilot : *pilots) {
		if (pilot.LocationKind != static_cast<UINT>(AssetLocationKind::ContainerBag))
			continue;
		const int shipObjectId = dynContainersManager::getInstance()->getObjectIDByContainerID(pilot.LocationRef);
		if (shipObjectId == -1)
			continue;
		const int solarSystemId = static_cast<int>(getSolarSystemIdByObjectID(shipObjectId));
		if (uniqueIDs.insert(solarSystemId).second) {
			result.push_back(solarSystemId);
		}
	}
	return result;
}

void dynGameObjectsManager::removeObjectByObjectID(int object_id)
{
	std::string query = "DELETE FROM dynGameObjects WHERE ObjectID = ?";
	sqlite3_stmt* deleteStmt = nullptr;
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &deleteStmt, nullptr);
	if (rc != SQLITE_OK) {
		return;
	}
	sqlite3_bind_int(deleteStmt, 1, object_id);
	sqlite3_step(deleteStmt);
	sqlite3_finalize(deleteStmt);
}
