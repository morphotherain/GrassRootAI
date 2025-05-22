#pragma once
#include "DatabaseManager.h"
#include "dogmaAttributesManager.h"

class dynObjectAttributesManager : public SingletonBase<dynObjectAttributesManager> {
private:
	sqlite3_stmt* stmt = nullptr;
	sqlite3* db;

public:
	dynObjectAttributesManager() { db = DatabaseManager::getInstance()->getDatabase(); };

	std::shared_ptr<std::unordered_map<int, Attribute>> getAttributesByObjectID(int object_id);
	void updateAttributesByObjectID(int object_id, const std::unordered_map<int, Attribute>& newAttributes);
	void resetAttributesByObjectID(int object_id, const std::unordered_map<int, Attribute>& newAttributes);
	void removeAttributesByObjectID(int object_id);
};
