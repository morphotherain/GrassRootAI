#include "dynObjectAttributesManager.h"

std::shared_ptr<std::unordered_map<int, Attribute>> dynObjectAttributesManager::getAttributesByObjectID(int object_id)
{
	std::shared_ptr<std::unordered_map<int, Attribute>> result = std::make_shared<std::unordered_map<int, Attribute>>();
	std::string query = "SELECT attributeID,value FROM dynObjectAttributes WHERE ObjectID = ?";
	int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, object_id);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			Attribute temp;
			temp.attributeID = sqlite3_column_int(stmt, 0);
			temp.value = sqlite3_column_int(stmt, 1);
			(*result)[temp.attributeID] = temp;
		}
	}
	sqlite3_finalize(stmt);
	return result;
}

void dynObjectAttributesManager::updateAttributesByObjectID(int object_id, const std::unordered_map<int, Attribute>& newAttributes) {
	std::string updateQuery = "UPDATE dynObjectAttributes SET value =? WHERE ObjectID =? AND attributeID =?";
	int rc = sqlite3_prepare_v2(db, updateQuery.c_str(), -1, &stmt, nullptr);
	if (rc == SQLITE_OK) {
		for (const auto& attrPair : newAttributes) {
			const Attribute& attr = attrPair.second;
			// 绑定新的值、对象ID以及属性ID到SQL语句中对应的参数位置
			sqlite3_bind_double(stmt, 1, attr.value);
			sqlite3_bind_int(stmt, 2, object_id);
			sqlite3_bind_int(stmt, 3, attr.attributeID);
			// 执行更新语句
			sqlite3_step(stmt);
			// 重置语句状态，以便下一次循环使用
			sqlite3_reset(stmt);
		}
	}
	sqlite3_finalize(stmt);
}

void dynObjectAttributesManager::resetAttributesByObjectID(int object_id, const std::unordered_map<int, Attribute>& newAttributes) {
	// 准备删除语句，用于删除对应ObjectID的所有已有属性记录
	std::string deleteQuery = "DELETE FROM dynObjectAttributes WHERE ObjectID =?";
	int deleteRc = sqlite3_prepare_v2(db, deleteQuery.c_str(), -1, &stmt, nullptr);
	if (deleteRc == SQLITE_OK) {
		// 绑定对象ID到删除语句参数位置
		sqlite3_bind_int(stmt, 1, object_id);
		// 执行删除操作
		sqlite3_step(stmt);
		sqlite3_reset(stmt);
		sqlite3_finalize(stmt);
	}

	// 准备插入语句，用于插入新的属性记录
	std::string insertQuery = "INSERT INTO dynObjectAttributes (ObjectID, attributeID, value) VALUES (?,?,?)";
	int insertRc = sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr);
	if (insertRc == SQLITE_OK) {
		for (const auto& attrPair : newAttributes) {
			const Attribute& attr = attrPair.second;
			// 绑定参数到插入语句
			sqlite3_bind_int(stmt, 1, object_id);
			sqlite3_bind_int(stmt, 2, attr.attributeID);
			sqlite3_bind_double(stmt, 3, attr.value);
			// 执行插入操作
			sqlite3_step(stmt);
			// 重置插入语句状态，以便下一次循环使用
			sqlite3_reset(stmt);
		}
		sqlite3_finalize(stmt);
	}
}