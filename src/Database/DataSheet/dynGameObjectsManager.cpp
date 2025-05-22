#include "dynGameObjectsManager.h"
#include "dynContainersManager.h"
#include <unordered_set>

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

unsigned int dynGameObjectsManager::getOwnerIdByObjectID(int object_id)
{
	unsigned int id = 0;
	std::string query = "SELECT OwnerID FROM dynGameObjects WHERE ObjectID =?";
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

unsigned int dynGameObjectsManager::getContainerIdByObjectID(int object_id)
{
	unsigned int id = 0;
	std::string query = "SELECT ContainerID FROM dynGameObjects WHERE ObjectID =?";
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

// 在dynGameObjectsManager类中添加这个函数，用于更新给定object_id对应的SolarSystemId、OwnerId、ContainerId信息到数据库
int dynGameObjectsManager::updateRelatedIdsByObjectID(int object_id, unsigned int solarSystemId, unsigned int ownerId, unsigned int containerId) {
	std::string query = "UPDATE dynGameObjects SET SolarSystemID =?, OwnerID =?, ContainerID =? WHERE ObjectID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		// 绑定要更新的各个id值
		sqlite3_bind_int(stmt, 1, solarSystemId);
		sqlite3_bind_int(stmt, 2, ownerId);
		sqlite3_bind_int(stmt, 3, containerId);
		// 绑定要更新的对象ID
		sqlite3_bind_int(stmt, 4, object_id);

		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			// 如果执行更新操作没有成功完成（期望返回SQLITE_DONE），可以在这里进行错误处理，比如打印错误日志等
			std::cerr << "Error updating related IDs for object ID " << object_id << ": " << sqlite3_errmsg(db) << std::endl;
			sqlite3_finalize(stmt);
			return -1;  // 返回错误码表示更新失败
		}
	}
	else {
		// 如果准备语句出错，进行错误处理
		std::cerr << "Error preparing update statement for object ID " << object_id << ": " << sqlite3_errmsg(db) << std::endl;
		return -1;
	}
	sqlite3_finalize(stmt);
	return 0;  // 返回0表示更新成功
}

std::vector<double> dynGameObjectsManager::getPosByObjectID(int object_id) {
	std::vector<double> pos(3, 0.0f); // 初始化vector，长度为3，初始值都为0.0f，表示三维坐标
	std::string query = "SELECT x, y, z FROM dynGameObjects WHERE ObjectID =?";
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

// 在dynGameObjectsManager类中添加这个函数，用于更新游戏对象的位置信息到数据库
int dynGameObjectsManager::updatePosByObjectID(int object_id, const std::vector<double>& pos) {
	std::string query = "UPDATE dynGameObjects SET x =?, y =?, z =? WHERE ObjectID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		// 绑定新的坐标值
		sqlite3_bind_double(stmt, 1, pos[0]);
		sqlite3_bind_double(stmt, 2, pos[1]);
		sqlite3_bind_double(stmt, 3, pos[2]);
		// 绑定要更新的对象ID
		sqlite3_bind_int(stmt, 4, object_id);

		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			// 如果执行更新操作没有成功完成（期望返回SQLITE_DONE），可以在这里进行错误处理，比如打印错误日志等
			std::cerr << "Error updating position for object ID " << object_id << ": " << sqlite3_errmsg(db) << std::endl;
			sqlite3_finalize(stmt);
			return -1;  // 返回错误码表示更新失败
		}
	}
	else {
		// 如果准备语句出错，进行错误处理
		std::cerr << "Error preparing update statement for object ID " << object_id << ": " << sqlite3_errmsg(db) << std::endl;
		return -1;
	}
	sqlite3_finalize(stmt);
	return 0;  // 返回0表示更新成功
}

// 函数实现
std::vector<double> dynGameObjectsManager::getQuaternionByObjectID(int object_id) {
	std::vector<double> quaternion(4, 0.0f); // 初始化vector，长度为4，初始值都为0.0f，对应四元数的四个分量
	std::string query = "SELECT qw, qx, qy, qz FROM dynGameObjects WHERE ObjectID =?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, object_id);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			quaternion[0] = (sqlite3_column_double(stmt, 0));
			quaternion[1] = (sqlite3_column_double(stmt, 1));
			quaternion[2] = (sqlite3_column_double(stmt, 2));
			quaternion[3] = (sqlite3_column_double(stmt, 3));
		}
	}
	sqlite3_finalize(stmt);
	return quaternion;
}

// 假设在dynGameObjectsManager类中
int dynGameObjectsManager::updateContainerIDByObjectID(int object_id, int newValue)
{
	std::string query = "UPDATE dynGameObjects SET ContainerID =? WHERE ObjectID =?";
	int rc;
	sqlite3_stmt* stmt;

	rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << "Error preparing UPDATE statement: " << sqlite3_errmsg(db) << std::endl;
		return -1;
	}

	sqlite3_bind_int(stmt, 1, newValue);
	sqlite3_bind_int(stmt, 2, object_id);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		std::cerr << "Error executing UPDATE statement: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_finalize(stmt);
	return 0;
}

bool dynGameObjectsManager::queryObjectsByContainerID(int containerID, std::vector<std::pair<int, int>>& result)
{
	sqlite3_stmt* stmt;
	// 准备 SQL 查询语句
	std::string query = "SELECT objectID, typeID FROM dynGameObjects WHERE ContainerID =?;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	// 将传入的 containerID 绑定到查询语句中的参数占位符
	sqlite3_bind_int(stmt, 1, containerID);

	// 执行查询并处理结果
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		int objectID = sqlite3_column_int(stmt, 0);
		int typeID = sqlite3_column_int(stmt, 1);
		result.emplace_back(objectID, typeID);
	}

	if (rc != SQLITE_DONE) {
		std::cerr << "SQL step error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		return false;
	}

	// 解除绑定并释放资源
	sqlite3_finalize(stmt);
	return true;
}

dynGameObject dynGameObjectsManager::getGameObjectByObjectID(int object_id)
{
	dynGameObject result;
	// 修正后的SQL查询语句，根据传入的solarSystemID动态查询
	std::string query = "SELECT dynGameObjects.*, invtypes.groupID, invGroups.categoryID FROM dynGameObjects JOIN invtypes ON dynGameObjects.typeID = invtypes.typeID JOIN invGroups ON invtypes.groupID = invGroups.groupID WHERE dynGameObjects.ObjectID =?;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		// 将传入的solarSystemID绑定到查询语句中的参数占位符
		sqlite3_bind_int(stmt, 1, object_id);

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			dynGameObject temp;
			// 根据实际列索引正确读取数据
			result.objectID = sqlite3_column_int(stmt, 0);
			result.typeID = sqlite3_column_int(stmt, 1);
			result.x = sqlite3_column_double(stmt, 2);
			result.y = sqlite3_column_double(stmt, 3);
			result.z = sqlite3_column_double(stmt, 4);
			result.SolarSystemID = sqlite3_column_int(stmt, 5);
			result.OwnerID = sqlite3_column_int(stmt, 6);
			result.ContainerID = sqlite3_column_int(stmt, 7);
			result.qw = sqlite3_column_double(stmt, 9);
			result.qx = sqlite3_column_double(stmt, 10);
			result.qy = sqlite3_column_double(stmt, 11);
			result.qz = sqlite3_column_double(stmt, 12);
			result.groupID = sqlite3_column_int(stmt, 13);
			result.categoryID = sqlite3_column_int(stmt, 14);
		}

		// 解除绑定
		sqlite3_bind_null(stmt, 1);
	}
	sqlite3_finalize(stmt);
	return result;
}

// 自动获取下一个自增的objectID然后根据参数插入一条新记录
int dynGameObjectsManager::insertGameObject(const dynGameObject& obj) {
	// 获取当前最大的objectID
	std::string getMaxIDQuery = "SELECT MAX(ObjectID) FROM dynGameObjects;";
	int rc = sqlite3_prepare_v2(db, getMaxIDQuery.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQLite prepare error: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	int nextObjectID = 1;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		int maxID = sqlite3_column_int(stmt, 0);
		nextObjectID = maxID + 1;
	}
	sqlite3_finalize(stmt);

	// 插入新记录
	std::string insertQuery = "INSERT INTO dynGameObjects (ObjectID, typeID, x, y, z, SolarSystemID, OwnerID, ContainerID, qw, qx, qy, qz) VALUES (?,?,?,?,?,?,?,?,?,?,?,?);";
	rc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQLite prepare error: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	sqlite3_bind_int(stmt, 1, nextObjectID);
	sqlite3_bind_int(stmt, 2, obj.typeID);
	sqlite3_bind_double(stmt, 3, obj.x);
	sqlite3_bind_double(stmt, 4, obj.y);
	sqlite3_bind_double(stmt, 5, obj.z);
	sqlite3_bind_int(stmt, 6, obj.SolarSystemID);
	sqlite3_bind_int(stmt, 7, obj.OwnerID);
	sqlite3_bind_int(stmt, 8, obj.ContainerID);
	sqlite3_bind_double(stmt, 9, obj.qw);
	sqlite3_bind_double(stmt, 10, obj.qx);
	sqlite3_bind_double(stmt, 11, obj.qy);
	sqlite3_bind_double(stmt, 12, obj.qz);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		std::cerr << "SQLite insert error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);
	return nextObjectID;
}

// 根据objectID删除指定的记录
bool dynGameObjectsManager::deleteGameObject(int objectID) {
	std::string deleteQuery = "DELETE FROM dynGameObjects WHERE ObjectID =?;";
	int rc = sqlite3_prepare_v2(db, deleteQuery.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQLite prepare error: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	sqlite3_bind_int(stmt, 1, objectID);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		std::cerr << "SQLite delete error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);
	return true;
}

unsigned int dynGameObjectsManager::getPilotObjectIDByPilotID(int pilot_id)
{
	unsigned int id = 0;
	std::string query = "SELECT ObjectID FROM dynGameObjects WHERE OwnerID =? AND TypeID =164";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, pilot_id);
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
	// 修正后的SQL查询语句，根据传入的solarSystemID动态查询
	std::string query = "SELECT dynGameObjects.*, invtypes.groupID, invGroups.categoryID FROM dynGameObjects JOIN invtypes ON dynGameObjects.typeID = invtypes.typeID JOIN invGroups ON invtypes.groupID = invGroups.groupID WHERE dynGameObjects.SolarSystemID =?;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		// 将传入的solarSystemID绑定到查询语句中的参数占位符
		sqlite3_bind_int(stmt, 1, solarSystemID);

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			dynGameObject temp;
			// 根据实际列索引正确读取数据
			temp.objectID = sqlite3_column_int(stmt, 0);
			temp.typeID = sqlite3_column_int(stmt, 1);
			temp.x = sqlite3_column_double(stmt, 2);
			temp.y = sqlite3_column_double(stmt, 3);
			temp.z = sqlite3_column_double(stmt, 4);
			temp.SolarSystemID = solarSystemID;
			temp.OwnerID = sqlite3_column_int(stmt, 6);
			temp.ContainerID = sqlite3_column_int(stmt, 7);
			temp.qw = sqlite3_column_double(stmt, 9);
			temp.qx = sqlite3_column_double(stmt, 10);
			temp.qy = sqlite3_column_double(stmt, 11);
			temp.qz = sqlite3_column_double(stmt, 12);
			temp.groupID = sqlite3_column_int(stmt, 13);
			temp.categoryID = sqlite3_column_int(stmt, 14);
			result->push_back(temp);
		}

		// 解除绑定
		sqlite3_bind_null(stmt, 1);
	}
	sqlite3_finalize(stmt);
	return result;
}

std::shared_ptr<std::vector<dynGameObject>> dynGameObjectsManager::getPilots()
{
	auto result = std::make_shared<std::vector<dynGameObject>>();
	// 修正后的SQL查询语句，根据传入的solarSystemID动态查询
	std::string query = "SELECT dynGameObjects.*, invtypes.groupID, invGroups.categoryID FROM dynGameObjects JOIN invtypes ON dynGameObjects.typeID = invtypes.typeID JOIN invGroups ON invtypes.groupID = invGroups.groupID WHERE invTypes.GroupID = 23;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		// 将传入的solarSystemID绑定到查询语句中的参数占位符

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			dynGameObject temp;
			// 根据实际列索引正确读取数据
			temp.objectID = sqlite3_column_int(stmt, 0);
			temp.typeID = sqlite3_column_int(stmt, 1);
			temp.x = sqlite3_column_double(stmt, 2);
			temp.y = sqlite3_column_double(stmt, 3);
			temp.z = sqlite3_column_double(stmt, 4);
			temp.SolarSystemID = sqlite3_column_int(stmt, 5);
			temp.OwnerID = sqlite3_column_int(stmt, 6);
			temp.ContainerID = sqlite3_column_int(stmt, 7);
			temp.qw = sqlite3_column_double(stmt, 9);
			temp.qx = sqlite3_column_double(stmt, 10);
			temp.qy = sqlite3_column_double(stmt, 11);
			temp.qz = sqlite3_column_double(stmt, 12);
			temp.groupID = sqlite3_column_int(stmt, 13);
			temp.categoryID = sqlite3_column_int(stmt, 14);
			result->push_back(temp);
		}

		// 解除绑定
		sqlite3_bind_null(stmt, 1);

		// 再根据获取到的containerID去查找对应的SolarSystemID
		std::string querySecondStepTemplate = "SELECT dynGameObjects.SolarSystemID FROM dynGameObjects WHERE dynGameObjects.objectID =?";
		sqlite3_stmt* stmtSecondStep;
		rc = sqlite3_prepare_v2(db, querySecondStepTemplate.c_str(), -1, &stmtSecondStep, nullptr);
		if (rc == SQLITE_OK) {
			for (auto& pilot : *result) {
				sqlite3_reset(stmtSecondStep);
				sqlite3_bind_int(stmtSecondStep, 1, pilot.ContainerID);
				while (sqlite3_step(stmtSecondStep) == SQLITE_ROW) {
					int solarSystemID = sqlite3_column_int(stmtSecondStep, 0);
					pilot.SolarSystemID = solarSystemID;
				}
			}
			sqlite3_finalize(stmtSecondStep);
		}
	}
	sqlite3_finalize(stmt);
	return result;
}

std::vector<int> dynGameObjectsManager::getItemsByContainerID(UINT containerID)
{
	auto result = std::vector<int>();
	// 修正后的SQL查询语句，根据传入的solarSystemID动态查询
	std::string query = "SELECT ObjectID FROM dynGameObjects WHERE ContainerID = ?;";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		// 将传入的solarSystemID绑定到查询语句中的参数占位符
		sqlite3_bind_int(stmt, 1, containerID);

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			result.push_back(sqlite3_column_int(stmt, 0));
		}

		// 解除绑定
		sqlite3_bind_null(stmt, 1);
	}
	sqlite3_finalize(stmt);
	return result;
}

std::vector<int> dynGameObjectsManager::getSolarSystemIDHasPilot()
{
	std::vector<int> result;
	std::unordered_set<int> uniqueIDs;  // 用于记录已经出现过的SolarSystemID，实现去重

	// 先查询出group为23的object对应的containerID
	std::string queryFirstStep = "SELECT dynGameObjects.containerID FROM dynGameObjects JOIN invTypes ON dynGameObjects.typeID = invTypes.typeID WHERE invTypes.GroupID = 23";
	sqlite3_stmt* stmtFirstStep;
	int rcFirstStep = sqlite3_prepare_v2(db, queryFirstStep.c_str(), -1, &stmtFirstStep, nullptr);
	if (rcFirstStep == SQLITE_OK) {
		std::vector<int> containerIDs;
		std::vector<int> objectIDs;
		while (sqlite3_step(stmtFirstStep) == SQLITE_ROW) {
			int containerID = sqlite3_column_int(stmtFirstStep, 0);
			containerIDs.push_back(containerID);
		}
		sqlite3_finalize(stmtFirstStep);

		for (int containerID : containerIDs) {
			int objectID = dynContainersManager::getInstance()->getObjectIDByContainerID(containerID);
			objectIDs.push_back(objectID);
		}

		// 再根据获取到的objectID去查找对应的SolarSystemID
		std::string querySecondStepTemplate = "SELECT dynGameObjects.SolarSystemID FROM dynGameObjects WHERE dynGameObjects.objectID =?";
		sqlite3_stmt* stmtSecondStep;
		rcFirstStep = sqlite3_prepare_v2(db, querySecondStepTemplate.c_str(), -1, &stmtSecondStep, nullptr);
		if (rcFirstStep == SQLITE_OK) {
			for (int objectID : objectIDs) {
				sqlite3_reset(stmtSecondStep);
				sqlite3_bind_int(stmtSecondStep, 1, objectID);
				while (sqlite3_step(stmtSecondStep) == SQLITE_ROW) {
					int solarSystemID = sqlite3_column_int(stmtSecondStep, 0);
					// 检查当前SolarSystemID是否已经存在于uniqueIDs中，如果不存在则添加到result并记录到uniqueIDs中
					if (uniqueIDs.find(solarSystemID) == uniqueIDs.end()) {
						result.push_back(solarSystemID);
						uniqueIDs.insert(solarSystemID);
					}
				}
			}
			sqlite3_finalize(stmtSecondStep);
		}
	}
	return result;
}

void dynGameObjectsManager::removeObjectByObjectID(int object_id)
{
	DEBUG_("开始删除 ObjectID: {}", object_id);

	std::string query = "DELETE FROM dynGameObjects WHERE ObjectID = ?";
	sqlite3_stmt* stmt = nullptr;

	// 准备 SQL 语句
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		DEBUG_("SQL 语句准备失败 (错误码: {}): {}", rc, sqlite3_errmsg(db));
		return;
	}

	// 绑定参数
	rc = sqlite3_bind_int(stmt, 1, object_id);
	if (rc != SQLITE_OK) {
		DEBUG_("参数绑定失败 (错误码: {}): {}", rc, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return;
	}

	// 执行删除
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_DONE) {
		DEBUG_("成功删除 ObjectID: {}", object_id);
	}
	else {
		DEBUG_("删除失败 (错误码: {}): {}", rc, sqlite3_errmsg(db));
	}

	// 清理资源
	sqlite3_finalize(stmt);
}