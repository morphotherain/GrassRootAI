#pragma once

#include "DatabaseManager.h"
#include <vector>

// 定义用于存储恒星系数据的结构
struct RegionData
{
	double x;                       //  x 坐标
	double y;                       //  y 坐标
	double z;                       //  z 坐标
	std::wstring regionName;   // 名称
	int regionID;
};

// 定义用于存储星门跳跃数据的结构
struct SolarSystemJump
{
	int fromRegionID;          // 起点星域 ID
	int fromConstellationID;   // 起点星座 ID
	int fromSolarSystemID;     // 起点星系 ID
	int toSolarSystemID;       // 终点星系 ID
	int toConstellationID;     // 终点星座 ID
	int toRegionID;            // 终点星域 ID
};

// 定义用于存储恒星系数据的结构
struct SolarSystemData
{
public:
	SolarSystemData() = default;
	SolarSystemData(int id);

	double x;                       // 恒星系的 x 坐标
	double y;                       // 恒星系的 y 坐标
	double z;                       // 恒星系的 z 坐标
	std::wstring solarSystemName;   // 恒星系名称
	std::wstring constellationName;   // 名称
	std::wstring regionName;   // 名称
	double luminosity;              // 光度
	int solarSystemID;
	int constellationID;
	int regionalID;

	void getConstellationName();
	void getRegionaName();
};

struct DenormalizeData {
public:
	DenormalizeData() = default;
	DenormalizeData(int id);

	std::wstring name = L"";
	double x = 0;                       //  x 坐标
	double y = 0;                       //  y 坐标
	double z = 0;                       //  z 坐标
	int nameID = 0;   // 名称
	int regionID = 0;
	int constellationID = 0;
	int solarSystemID = 0;
	double radius = 0;
	int itemID = 0;
	int typeID = 0;
	int groupID = 0;
	int celestialIndex = 0;
	int orbitIndex = 0;
	int bracketID = 0;
	std::string dds_path = "";
};

class MapManager : public SingletonBase<MapManager> {
private:
    sqlite3_stmt* stmt = nullptr;
    sqlite3* db;

public:
    MapManager() { db = DatabaseManager::getInstance()->getDatabase(); };

};

// 从数据库中获取所有恒星系数据的函数

std::vector<SolarSystemData> getSolarSystems();

SolarSystemData getSolarsystem(int id);

// 获取星门跳跃数据的函数声明
std::vector<SolarSystemJump> getSolarSystemJumps();

// 获取星门跳跃数据的函数声明
std::vector<RegionData> getRegions();
