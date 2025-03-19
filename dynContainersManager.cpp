#include "dynContainersManager.h"

bool dynContainersManager::createContainerRecord(int objectID, int containerTypeID) {
    std::string query = "INSERT INTO dynContainers (objectID, containerType) VALUES (?,?)";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, objectID);
        sqlite3_bind_int(stmt, 2, containerTypeID);
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return true;
        }
        else {
            std::cerr << "Failed to insert record: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }
    else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
}

// 根据 objectID 和 containerType 查询 containerID 
int  dynContainersManager::getContainerID(int objectID, int containerTypeID) {
    UINT containerID = -1;
    std::string query = "SELECT containerID FROM dynContainers WHERE objectID =? AND containerType =?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, objectID);
        sqlite3_bind_int(stmt, 2, containerTypeID);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            containerID = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }
    return containerID;
}

int dynContainersManager::getObjectIDByContainerID(int containerID)
{

    int objectID = -1;
    std::string query = "SELECT objectID FROM dynContainers WHERE containerID =?";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, containerID);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            objectID = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }
    return objectID;
}
