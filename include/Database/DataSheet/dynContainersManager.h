#pragma once
#include "DatabaseManager.h"

class dynContainersManager : public SingletonBase<dynContainersManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	dynContainersManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	bool createContainerRecord(int objectID, int containerTypeID);    // 创建新的 container 记录

	int getContainerID(int objectID, int containerTypeID);           // 根据 objectID 和 containerType 查询 containerID

	int getObjectIDByContainerID(int containerID);

	bool removeContainerByID(int containerID);
};
