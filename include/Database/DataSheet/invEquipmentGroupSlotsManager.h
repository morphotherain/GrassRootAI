// InvTypesManager.h
#pragma once
#include "DatabaseManager.h"

class invEquipmentGroupSlotsManager : public SingletonBase<invEquipmentGroupSlotsManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;
	std::unordered_map<int, int> groupSlotMap;

public:
	invEquipmentGroupSlotsManager() { db = 
		DatabaseManager::getInstance()->getDatabase(); 
		loadGroupSlots();
	};

	void loadGroupSlots();
	int getSlotByGroupID(int groupID) const;
};


