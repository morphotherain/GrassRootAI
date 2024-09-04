#include "SolarSystem.h"

std::vector<SolarSystemData> getSolarSystems()
{
    std::vector<SolarSystemData> solarSystems;

    // ��ȡ���ݿ�ʵ��
    DatabaseManager* dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager->getDatabase();

    // SQL ��ѯ��䣬��ȡ���к���ϵ�� x, y, z, solarSystemName �� luminosity
    std::string sql = "SELECT x, y, z, solarSystemName, luminosity ,solarSystemID ,constellationID,regional FROM mapSolarSystems;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return solarSystems;
    }

    // ������ѯ����������ݴ洢�� solarSystems �ṹ��
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        SolarSystemData system;

        // ��ȡ�������ݣ�ȷ���������� SELECT ����˳��ƥ��
        system.x = sqlite3_column_double(stmt, 0);
        system.y = sqlite3_column_double(stmt, 1);
        system.z = sqlite3_column_double(stmt, 2);
        system.solarSystemName = dbManager->sqlite3_column_wstring(stmt, 3); // ��ȡ wstring ���͵�����
        system.luminosity = sqlite3_column_double(stmt, 4);
        system.solarSystemID = sqlite3_column_int(stmt, 5);
        system.constellationID = sqlite3_column_int(stmt, 6);
        system.regionalID = sqlite3_column_int(stmt, 7);

        // ��������ӵ�������
        solarSystems.push_back(system);
    }

    // �ͷ������Դ
    sqlite3_finalize(stmt);

    return solarSystems;
}

// �����ݿ��л�ȡ����������Ծ���ݵĺ���
std::vector<SolarSystemJump> getSolarSystemJumps()
{
    std::vector<SolarSystemJump> solarSystemJumps;

    // ��ȡ���ݿ�ʵ��
    DatabaseManager* dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager->getDatabase();

    // SQL ��ѯ��䣬��ȡ������Ծ����
    std::string sql = "SELECT fromRegionID, fromConstellationID, fromSolarSystemID, toSolarSystemID, toConstellationID, toRegionID FROM mapSolarSystemJumps;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return solarSystemJumps;
    }

    // ������ѯ����������ݴ洢�� solarSystemJumps �ṹ��
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        SolarSystemJump jump;

        // ��ȡ�������ݣ�ȷ���������� SELECT ����˳��ƥ��
        jump.fromRegionID = sqlite3_column_int(stmt, 0);
        jump.fromConstellationID = sqlite3_column_int(stmt, 1);
        jump.fromSolarSystemID = sqlite3_column_int(stmt, 2);
        jump.toSolarSystemID = sqlite3_column_int(stmt, 3);
        jump.toConstellationID = sqlite3_column_int(stmt, 4);
        jump.toRegionID = sqlite3_column_int(stmt, 5);

        // ��������ӵ�������
        solarSystemJumps.push_back(jump);
    }

    // �ͷ������Դ
    sqlite3_finalize(stmt);

    return solarSystemJumps;
}

std::vector<RegionData> getRegions()
{
    std::vector<RegionData> regions;

    // ��ȡ���ݿ�ʵ��
    DatabaseManager* dbManager = DatabaseManager::getInstance();
    sqlite3* db = dbManager->getDatabase();

    // SQL ��ѯ��䣬��ȡ������Ծ����
    std::string sql = "SELECT x, y, z,regionName,regionID FROM mapRegions;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return regions;
    }

    // ������ѯ����������ݴ洢�� solarSystemJumps �ṹ��
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        RegionData region;

        // ��ȡ�������ݣ�ȷ���������� SELECT ����˳��ƥ��
        region.x = sqlite3_column_double(stmt, 0);
        region.y = sqlite3_column_double(stmt, 1);
        region.z = sqlite3_column_double(stmt, 2);
        region.regionName = dbManager->sqlite3_column_wstring(stmt, 3); // ��ȡ wstring ���͵�����
        region.regionID = sqlite3_column_int(stmt, 4);

        // ��������ӵ�������
        regions.push_back(region);
    }

    // �ͷ������Դ
    sqlite3_finalize(stmt);

    return regions;
}
