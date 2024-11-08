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
};

class dynGameObjectsManager : public SingletonBase<dynGameObjectsManager> {
private:
    sqlite3_stmt* stmt;
    sqlite3* db;

public:
    dynGameObjectsManager() { db = DatabaseManager::getInstance()->getDatabase(); };

    // 假设在 invtypes 表中根据 type_id 查找 name 列
    unsigned int getTypeIdByObjectID(int object_id);
    unsigned int getSolarSystemIdByObjectID(int object_id);
    unsigned int getOwnerIdByObjectID(int object_id);
    unsigned int getContainerIdByObjectID(int object_id);
    std::vector<double> getPosByObjectID(int object_id);
    std::vector<double> getQuaternionByObjectID(int object_id);
    unsigned int getPilotObjectIDByPilotID(int pilot_id);

    std::shared_ptr<std::vector<dynGameObject>> getGameObjectBySolarSystemID(UINT solarSystemID);

    int updateContainerIDByObjectID(int object_id, int newValue);


    std::vector<int> getSolarSystemIDHasPilot();

};

