#include <string>
#include <vector>
#include "DatabaseManager.h"

// 定义用于存储恒星系数据的结构
struct SolarSystemData
{
    double x;                       // 恒星系的 x 坐标
    double y;                       // 恒星系的 y 坐标
    double z;                       // 恒星系的 z 坐标
    std::wstring solarSystemName;   // 恒星系名称
    float luminosity;              // 光度
    int solarSystemID;
    int constellationID;
    int regionalID;
};

// 定义用于存储恒星系数据的结构
struct RegionData
{
    double x;                       // 恒星系的 x 坐标
    double y;                       // 恒星系的 y 坐标
    double z;                       // 恒星系的 z 坐标
    std::wstring regionName;   // 恒星系名称
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

// 从数据库中获取所有恒星系数据的函数
std::vector<SolarSystemData> getSolarSystems();

// 获取星门跳跃数据的函数声明
std::vector<SolarSystemJump> getSolarSystemJumps();

// 获取星门跳跃数据的函数声明
std::vector<RegionData> getRegions();