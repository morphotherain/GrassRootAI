#include <string>
#include <vector>
#include "DatabaseManager.h"

// �������ڴ洢����ϵ���ݵĽṹ
struct SolarSystemData
{
    double x;                       // ����ϵ�� x ����
    double y;                       // ����ϵ�� y ����
    double z;                       // ����ϵ�� z ����
    std::wstring solarSystemName;   // ����ϵ����
    float luminosity;              // ���
    int solarSystemID;
    int constellationID;
    int regionalID;
};

// �������ڴ洢����ϵ���ݵĽṹ
struct RegionData
{
    double x;                       // ����ϵ�� x ����
    double y;                       // ����ϵ�� y ����
    double z;                       // ����ϵ�� z ����
    std::wstring regionName;   // ����ϵ����
    int regionID;
};



// �������ڴ洢������Ծ���ݵĽṹ
struct SolarSystemJump
{
    int fromRegionID;          // ������� ID
    int fromConstellationID;   // ������� ID
    int fromSolarSystemID;     // �����ϵ ID
    int toSolarSystemID;       // �յ���ϵ ID
    int toConstellationID;     // �յ����� ID
    int toRegionID;            // �յ����� ID
};

// �����ݿ��л�ȡ���к���ϵ���ݵĺ���
std::vector<SolarSystemData> getSolarSystems();

// ��ȡ������Ծ���ݵĺ�������
std::vector<SolarSystemJump> getSolarSystemJumps();

// ��ȡ������Ծ���ݵĺ�������
std::vector<RegionData> getRegions();