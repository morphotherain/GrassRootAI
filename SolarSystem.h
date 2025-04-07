#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "DatabaseManager.h"
#include "dynGameObjectsManager.h"
#include "MapManager.h"
#include "GameObject.h"
#include "Pilot.h"
#include "Ship.h"
#include "Astro.h"
#include "Equipment.h"
#include "NPCStation.h"
#include "Sector.h"

struct SolarSystem {
	SolarSystem() = default;
	SolarSystem(int id) :m_solarSystem(id) { };

	SolarSystemData m_solarSystem;

	std::vector<std::shared_ptr<DenormalizeData>> m_denormalizes;

	std::shared_ptr<std::unordered_map<UINT, std::shared_ptr<GameObject>>> p_mapObject;
	std::shared_ptr < std::vector<std::shared_ptr<GameObject>>> p_starGateTransferObjects;
	std::vector<std::shared_ptr<GameObject>> space_objects;
	std::vector<std::shared_ptr<GameObject>> other_objects;
	std::vector<std::shared_ptr<Pilot>> Pilot_objects;
	std::shared_ptr<Pilot> currentPilot;
	std::shared_ptr<Sector> currentSector;

	std::unordered_map<long long int, std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>> m_Sectors;

	void Init();
	void Update(UINT tick);

	void getDenormalizesBySolarSystemID();

	void addGameObject(dynGameObject& objData);

	// 函数用于计算哈希单元的索引
	long long int CalculateHashIndex(long long int x, long long int  y, long long int  z) const { return x + y * 10000000 + z * 100000000000000; }
	long long int CalculateHashIndex(double x, double y, double z);
	void addObjectToSector(std::shared_ptr<GameObject> object);
	std::shared_ptr<Sector> addSector(double x, double y, double z);
	std::shared_ptr<Sector> getSector(double x, double y, double z);
	void checkObjectsInSector();
	void setCurrentSector();
	void clearCurrentSector();

	UINT getSolarSystemID() { return m_solarSystem.solarSystemID; };
	std::vector<std::shared_ptr<Pilot>> getPilots();
	void setCurrentPilots(std::shared_ptr<Pilot> _Pilot);
	void clearCurrentPilots();
};
