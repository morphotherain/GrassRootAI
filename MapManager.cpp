﻿#include "MapManager.h"

std::wstring intToRoman(int num) {
	// 简单的对应关系示例，处理1-20的情况
	static std::vector<std::wstring> romanMap = {
		L"",
		L"I",
		L"II",
		L"III",
		L"IV",
		L"V",
		L"VI",
		L"VII",
		L"VIII",
		L"IX",
		L"X",
		L"XI",
		L"XII",
		L"XIII",
		L"XIV",
		L"XV",
		L"XVI",
		L"XVII",
		L"XVIII",
		L"XIX",
		L"XX"
	};
	if (num >= 1 && num <= 20) {
		return romanMap[num];
	}
	return L"";  // 如果输入的数字不在处理范围内，返回空字符串，可根据实际需求调整错误处理逻辑
}

std::vector<SolarSystemData> getSolarSystems()
{
	std::vector<SolarSystemData> solarSystems;

	// 获取数据库实例
	DatabaseManager* dbManager = DatabaseManager::getInstance();
	sqlite3* db = dbManager->getDatabase();

	// SQL 查询语句，获取所有恒星系的 x, y, z, solarSystemName 和 luminosity
	std::string sql = "SELECT x, y, z, solarSystemName, luminosity ,solarSystemID ,constellationID,regionID,security FROM mapSolarSystems;";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		auto temp = sqlite3_errmsg(db);
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return solarSystems;
	}

	// 迭代查询结果并将数据存储到 solarSystems 结构中
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		SolarSystemData system;

		// 获取各列数据，确保列索引与 SELECT 语句的顺序匹配
		system.x = sqlite3_column_double(stmt, 0);
		system.y = sqlite3_column_double(stmt, 1);
		system.z = sqlite3_column_double(stmt, 2);
		system.solarSystemName = dbManager->sqlite3_column_wstring(stmt, 3); // 获取 wstring 类型的名称
		system.luminosity = sqlite3_column_double(stmt, 4);
		system.solarSystemID = sqlite3_column_int(stmt, 5);
		system.constellationID = sqlite3_column_int(stmt, 6);
		system.regionalID = sqlite3_column_int(stmt, 7);
		system.security = sqlite3_column_double(stmt, 8);

		// 将数据添加到向量中
		solarSystems.push_back(system);
	}

	// 释放语句资源
	sqlite3_finalize(stmt);

	return solarSystems;
}

std::vector<SolarSystemData> getSolarSystemsByRegionalID(int regionalID) {
	std::vector<SolarSystemData> solarSystems;

	// 获取数据库实例
	DatabaseManager* dbManager = DatabaseManager::getInstance();
	sqlite3* db = dbManager->getDatabase();

	// SQL 查询语句，根据 regionalID 获取指定区域内的所有恒星系
	std::string sql = "SELECT x, y, z, solarSystemName, luminosity, solarSystemID, constellationID, regionID, security "
		"FROM mapSolarSystems "
		"WHERE constellationID =?;";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return solarSystems;
	}

	// 绑定参数
	rc = sqlite3_bind_int(stmt, 1, regionalID);
	if (rc != SQLITE_OK) {
		std::cerr << "Failed to bind parameter: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_finalize(stmt);
		return solarSystems;
	}

	// 迭代查询结果并将数据存储到 solarSystems 结构中
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		SolarSystemData system;

		// 获取各列数据，确保列索引与 SELECT 语句的顺序匹配
		system.x = sqlite3_column_double(stmt, 0);
		system.y = sqlite3_column_double(stmt, 1);
		system.z = sqlite3_column_double(stmt, 2);
		system.solarSystemName = dbManager->sqlite3_column_wstring(stmt, 3); // 获取 wstring 类型的名称
		system.luminosity = sqlite3_column_double(stmt, 4);
		system.solarSystemID = sqlite3_column_int(stmt, 5);
		system.constellationID = sqlite3_column_int(stmt, 6);
		system.regionalID = sqlite3_column_int(stmt, 7);
		system.security = sqlite3_column_double(stmt, 8);

		// 将数据添加到向量中
		solarSystems.push_back(system);
	}

	// 释放语句资源
	sqlite3_finalize(stmt);

	return solarSystems;
}
SolarSystemData getSolarsystem(int id)
{
	return SolarSystemData(id);
}

// 从数据库中获取所有星门跳跃数据的函数
std::vector<SolarSystemJump> getSolarSystemJumps()
{
	std::vector<SolarSystemJump> solarSystemJumps;

	// 获取数据库实例
	DatabaseManager* dbManager = DatabaseManager::getInstance();
	sqlite3* db = dbManager->getDatabase();

	// SQL 查询语句，获取星门跳跃数据
	std::string sql = "SELECT fromRegionID, fromConstellationID, fromSolarSystemID, toSolarSystemID, toConstellationID, toRegionID FROM mapSolarSystemJumps;";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return solarSystemJumps;
	}

	// 迭代查询结果并将数据存储到 solarSystemJumps 结构中
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		SolarSystemJump jump;

		// 获取各列数据，确保列索引与 SELECT 语句的顺序匹配
		jump.fromRegionID = sqlite3_column_int(stmt, 0);
		jump.fromConstellationID = sqlite3_column_int(stmt, 1);
		jump.fromSolarSystemID = sqlite3_column_int(stmt, 2);
		jump.toSolarSystemID = sqlite3_column_int(stmt, 3);
		jump.toConstellationID = sqlite3_column_int(stmt, 4);
		jump.toRegionID = sqlite3_column_int(stmt, 5);

		// 将数据添加到向量中
		solarSystemJumps.push_back(jump);
	}

	// 释放语句资源
	sqlite3_finalize(stmt);

	return solarSystemJumps;
}

std::vector<RegionData> getRegions()
{
	std::vector<RegionData> regions;

	// 获取数据库实例
	DatabaseManager* dbManager = DatabaseManager::getInstance();
	sqlite3* db = dbManager->getDatabase();

	// SQL 查询语句，获取星门跳跃数据
	std::string sql = "SELECT x, y, z,regionName,regionID FROM mapRegions;";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
		return regions;
	}

	// 迭代查询结果并将数据存储到 solarSystemJumps 结构中
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		RegionData region;

		// 获取各列数据，确保列索引与 SELECT 语句的顺序匹配
		region.x = sqlite3_column_double(stmt, 0);
		region.y = sqlite3_column_double(stmt, 1);
		region.z = sqlite3_column_double(stmt, 2);
		region.regionName = dbManager->sqlite3_column_wstring(stmt, 3); // 获取 wstring 类型的名称
		region.regionID = sqlite3_column_int(stmt, 4);

		// 将数据添加到向量中
		regions.push_back(region);
	}

	// 释放语句资源
	sqlite3_finalize(stmt);

	return regions;
}

std::vector<std::shared_ptr<DenormalizeData>> getDenormalizesBySolarSystemID(int id)
{
	return std::vector<std::shared_ptr<DenormalizeData>>();
}

SolarSystemData::SolarSystemData(int id)
{
	// 获取数据库实例
	DatabaseManager* dbManager = DatabaseManager::getInstance();
	sqlite3* db = dbManager->getDatabase();

	// SQL 查询语句，获取所有恒星系的 x, y, z, solarSystemName 和 luminosity
	std::string sql = "SELECT x, y, z, solarSystemName, luminosity  ,constellationID,regionID,security FROM mapSolarSystems WHERE solarSystemID = " + std::to_string(id) + ";";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		auto temp = sqlite3_errmsg(db);
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
	}

	// 迭代查询结果并将数据存储到 solarSystems 结构中
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		// 获取各列数据，确保列索引与 SELECT 语句的顺序匹配
		x = sqlite3_column_double(stmt, 0);
		y = sqlite3_column_double(stmt, 1);
		z = sqlite3_column_double(stmt, 2);
		solarSystemName = dbManager->sqlite3_column_wstring(stmt, 3); // 获取 wstring 类型的名称
		luminosity = sqlite3_column_double(stmt, 4);
		constellationID = sqlite3_column_int(stmt, 5);
		regionalID = sqlite3_column_int(stmt, 6);
		security = sqlite3_column_double(stmt, 7);
		solarSystemID = id;
	}

	// 释放语句资源
	sqlite3_finalize(stmt);

	getConstellationName();
	getRegionaName();
}

void SolarSystemData::getConstellationName()
{
	// 获取数据库实例
	DatabaseManager* dbManager = DatabaseManager::getInstance();
	sqlite3* db = dbManager->getDatabase();

	// SQL 查询语句，获取所有恒星系的 x, y, z, solarSystemName 和 luminosity
	std::string sql = "SELECT constellationName FROM mapConstellations WHERE constellationID = " + std::to_string(constellationID) + ";";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		auto temp = sqlite3_errmsg(db);
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
	}

	// 迭代查询结果并将数据存储到 solarSystems 结构中
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		// 获取各列数据，确保列索引与 SELECT 语句的顺序匹配
		constellationName = dbManager->sqlite3_column_wstring(stmt, 0); // 获取 wstring 类型的名称
	}

	// 释放语句资源
	sqlite3_finalize(stmt);
}

void SolarSystemData::getRegionaName()
{
	// 获取数据库实例
	DatabaseManager* dbManager = DatabaseManager::getInstance();
	sqlite3* db = dbManager->getDatabase();

	// SQL 查询语句，获取所有恒星系的 x, y, z, solarSystemName 和 luminosity
	std::string sql = "SELECT regionName FROM mapRegions WHERE regionID = " + std::to_string(regionalID) + ";";

	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		auto temp = sqlite3_errmsg(db);
		std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
	}

	// 迭代查询结果并将数据存储到 solarSystems 结构中
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		// 获取各列数据，确保列索引与 SELECT 语句的顺序匹配
		regionName = dbManager->sqlite3_column_wstring(stmt, 0); // 获取 wstring 类型的名称
	}

	// 释放语句资源
	sqlite3_finalize(stmt);
}