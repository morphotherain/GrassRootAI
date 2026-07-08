#pragma once
#include "DatabaseManager.h"
#include "Sim/AssetLocation.h"
#include <vector>

#include "InvCategoriesMacro.h"
#include "InvGroupsMacro.h"

class dynGameObject {
public:
	dynGameObject() = default;
	~dynGameObject() = default;
	UINT objectID = 0;
	UINT typeID = 0;
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	UINT SolarSystemID = 0;
	UINT OwnerKind = 0;
	UINT OwnerID = 0;
	UINT LocationKind = 0;
	UINT LocationRef = 0;
	double qw = 0.0;
	double qx = 0.0;
	double qy = 0.0;
	double qz = 0.0;
	UINT groupID = 0;
	UINT categoryID = 0;
	std::wstring name = L"";

	AssetOwner GetOwner() const { return AssetOwner::FromDyn(OwnerKind, OwnerID); }
	AssetLocation GetLocation() const { return AssetLocation::FromDyn(LocationKind, LocationRef, SolarSystemID); }
};

class dynGameObjectsManager : public SingletonBase<dynGameObjectsManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	dynGameObjectsManager() {
		db = DatabaseManager::getInstance()->getDatabase();
	};

	unsigned int getTypeIdByObjectID(int object_id);
	unsigned int getSolarSystemIdByObjectID(int object_id);
	std::vector<double> getPosByObjectID(int object_id);
	int updatePlacementByObjectID(
		int object_id,
		unsigned int solarSystemId,
		unsigned int ownerKind,
		unsigned int ownerId,
		unsigned int locationKind,
		unsigned int locationRef);
	int updatePosByObjectID(int object_id, const std::vector<double>& pos);
	std::vector<double> getQuaternionByObjectID(int object_id);
	unsigned int getPilotObjectIDByPilotID(int pilot_id);

	bool queryObjectsByBagId(int bagId, std::vector<std::pair<int, int>>& result);

	std::shared_ptr<std::vector<dynGameObject>> getGameObjectBySolarSystemID(UINT solarSystemID);

	dynGameObject getGameObjectByObjectID(int object_id);
	int insertGameObject(const dynGameObject& obj);
	bool deleteGameObject(int objectID);

	std::shared_ptr<std::vector<dynGameObject>> getPilots();

	std::vector<int> getItemsByBagId(UINT bagId);

	std::vector<int> getSolarSystemIDHasPilot();

	void removeObjectByObjectID(int object_id);
};
