#pragma once
#include "DatabaseManager.h"
#include <vector>
#include "d3dUtil.h"

class dynGameObject {
public:
	dynGameObject() = default;
	~dynGameObject() = default;
	UINT objectID;
	UINT typeID;
	double x;
	double y;
	double z;
	UINT SolarSystemID;
	UINT OwnerID;
	UINT ContainerID;
	double qw;
	double qx;
	double qy;
	double qz;
	UINT groupID;
	UINT categoryID;
	std::wstring name = L"";
};

class dynGameObjectsManager : public SingletonBase<dynGameObjectsManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	dynGameObjectsManager() {
		db = DatabaseManager::getInstance()->getDatabase();
	};

	// 假设在 invtypes 表中根据 type_id 查找 name 列
	unsigned int getTypeIdByObjectID(int object_id);
	unsigned int getSolarSystemIdByObjectID(int object_id);
	unsigned int getOwnerIdByObjectID(int object_id);
	unsigned int getContainerIdByObjectID(int object_id);
	int updateRelatedIdsByObjectID(int object_id, unsigned int solarSystemId, unsigned int ownerId, unsigned int containerId);
	std::vector<double> getPosByObjectID(int object_id);
	int updatePosByObjectID(int object_id, const std::vector<double>& pos);
	std::vector<double> getQuaternionByObjectID(int object_id);
	unsigned int getPilotObjectIDByPilotID(int pilot_id);

	bool queryObjectsByContainerID(int containerID,
		std::vector<std::pair<int, int>>& result);

	std::shared_ptr<std::vector<dynGameObject>> getGameObjectBySolarSystemID(UINT solarSystemID);
	dynGameObject getGameObjectByObjectID(int object_id);
	std::shared_ptr<std::vector<dynGameObject>> getPilots();

	std::vector<int> getItemsByContainerID(UINT containerID);

	int updateContainerIDByObjectID(int object_id, int newValue);

	std::vector<int> getSolarSystemIDHasPilot();
};
