
#include "dynGameObjectsManager.h"


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

std::vector<int> dynGameObjectsManager::getSolarSystemIDHasPilot()
{
    std::vector<int> result;
    std::string query = "SELECT dynGameObjects.SolarSystemID FROM dynGameObjects JOIN invTypes ON dynGameObjects.typeID = invTypes.typeID WHERE invTypes.GroupID =23";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int solarSystemID = sqlite3_column_int(stmt, 0);
            result.push_back(solarSystemID);
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

