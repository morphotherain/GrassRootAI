#include "SolarSystem.h"
#include "InvTypesManager.h"
#include "invGroupsManager.h"
#include "eveBracketsManager.h"

std::vector<SolarSystemData> getSolarSystems()
{
    std::vector<SolarSystemData> solarSystems;

    // 获取数据库实例
    DatabaseManager* dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager->getDatabase();

    // SQL 查询语句，获取所有恒星系的 x, y, z, solarSystemName 和 luminosity
    std::string sql = "SELECT x, y, z, solarSystemName, luminosity ,solarSystemID ,constellationID,regionID FROM mapSolarSystems;";

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
    std::string sql = "SELECT x, y, z, solarSystemName, luminosity  ,constellationID,regionID FROM mapSolarSystems WHERE solarSystemID = " + std::to_string(id) + ";"; 

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

void SolarSystem::getDenormalizesBySolarSystemID()
{

    // 获取数据库实例
    DatabaseManager* dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager->getDatabase();

    // SQL 查询语句，获取所有恒星系的 x, y, z, solarSystemName 和 luminosity
    std::string sql = "SELECT x, y, z, nameID, regionID ,constellationID ,solarSystemID,radius,itemID,typeID,celestialIndex,orbitIndex FROM mapDenormalize WHERE solarSystemID = " + std::to_string(m_solarSystem.solarSystemID) + ";";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        auto temp = sqlite3_errmsg(db);
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }

    // 迭代查询结果并将数据存储到 solarSystems 结构中
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        auto p_denormalize = std::make_shared<DenormalizeData>();

        // 获取各列数据，确保列索引与 SELECT 语句的顺序匹配
        p_denormalize->x = sqlite3_column_double(stmt, 0);
        p_denormalize->y = sqlite3_column_double(stmt, 1);
        p_denormalize->z = sqlite3_column_double(stmt, 2);
        p_denormalize->nameID = sqlite3_column_int(stmt, 3); // 获取 wstring 类型的名称
        p_denormalize->regionID = sqlite3_column_int(stmt, 4);
        p_denormalize->constellationID = sqlite3_column_int(stmt, 5);
        p_denormalize->solarSystemID = sqlite3_column_int(stmt, 6);
        p_denormalize->radius = sqlite3_column_double(stmt, 7);
        p_denormalize->itemID = sqlite3_column_int(stmt, 8);
        p_denormalize->typeID = sqlite3_column_int(stmt, 9);
        p_denormalize->celestialIndex = sqlite3_column_int(stmt, 10);
        p_denormalize->orbitIndex = sqlite3_column_int(stmt, 11);

        m_denormalizes.push_back(p_denormalize);

    }

    // 释放语句资源
    sqlite3_finalize(stmt);

    for (auto p_denormalize : m_denormalizes) {
        p_denormalize->name = dbManager->getNameByTypeId(p_denormalize->typeID);
        int groupID = InvTypesManager::getInstance()->getGroupByTypeId(p_denormalize->typeID);
        int bracketID = invGroupsManager::getInstance()->getBracketIDByGroupId(groupID);
        p_denormalize->bracketID = bracketID;
        p_denormalize->dds_path = eveBracketsManager::getInstance()->getPathByTypeId(bracketID);
    }

}

int SolarSystem::typeIDtoTextureID(int id)
{
    if (idToId.empty())InitIdToIdMap();


    return  idToId[id];
}

void SolarSystem::InitIdToIdMap()
{
    idToId = {
        {6, 0},
        {7, 0},
        {8, 0},
        {9, 0},
        {10, 0},
        {3796, 0},
        {3797, 0},
        {3798, 0},
        {3799, 0},
        {3800, 0},
        {3801, 0},
        {3802, 0},
        {3803, 0},
        {45030, 0},
        {45031, 0},
        {45032, 0 },
        {45033, 0},
        {45034, 0},
        {45035, 0},
        {45036, 0},
        {45037, 0},
        {45038, 0},
        {45039, 0},
        {45040, 0},
        {45041, 0},
        {45042, 0},
        {45046, 0},
        {45047, 0},
        {56082, 0},
        {56083, 0},
        {56084, 0},
        {56085, 0},
        {56086, 0},
        {56097, 0},
        {56098, 0},
        {73909, 0},
        {78350, 0},
        {12, 1},
        {13, 1},
        {11, 1},
        {2014, 1},
        {2015, 1},
        {2016, 1},
        {2017, 1},
        {2063, 1},
        {30889, 1},
        {56018, 1},
        {56019, 1},
        {56020, 1},
        {56021, 1},
        {56022, 1},
        {56023, 1},
        {56024, 1},
        {14, 2},
        {15, 3},
        {29624, 4},
        {2502,5},

    };
}
