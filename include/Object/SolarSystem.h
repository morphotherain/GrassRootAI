#pragma once

#include <memory>
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
#include "Asteroid.h"
#include "Material.h"
#include "NPCStation.h"
#include "Sector.h"

class EntityUpdateSystem;
class SectorSpatialSystem;

struct SolarSystem {
	SolarSystem() = default;
	SolarSystem(int id);
	~SolarSystem();

	SolarSystem(const SolarSystem&) = delete;
	SolarSystem& operator=(const SolarSystem&) = delete;

	SolarSystemData m_solarSystem;

	std::vector<std::shared_ptr<DenormalizeData>> m_denormalizes;

	std::shared_ptr<std::unordered_map<UINT, std::shared_ptr<GameObject>>> p_mapObject;
	std::shared_ptr < std::vector<std::shared_ptr<GameObject>>> p_starGateTransferObjects;
	std::vector<std::weak_ptr<GameObject>> space_objects;
	std::vector<std::weak_ptr<GameObject>> other_objects;
	std::vector<std::shared_ptr<Pilot>> Pilot_objects;
	std::shared_ptr<Pilot> currentPilot;
	std::shared_ptr<Sector> currentSector;

	std::unique_ptr<EntityUpdateSystem> m_entityUpdate;
	std::unique_ptr<SectorSpatialSystem> m_sectorSpatial;

	void Init();
	void Update(UINT tick);

	void getDenormalizesBySolarSystemID();

	void addGameObject(dynGameObject& objData);

	// 函数用于计算哈希单元的索引
	long long int CalculateHashIndex(long long int x, long long int  y, long long int  z) const { return x + y * 10000000 + z * 100000000000000; }
	long long int CalculateHashIndex(double x, double y, double z);
	void addObjectToSector(std::shared_ptr<GameObject> object);
	void setCurrentSector();
	void clearCurrentSector();

	UINT getSolarSystemID() { return m_solarSystem.solarSystemID; };
	UINT getRegionID() { return m_solarSystem.regionalID; };
	UINT getconstellationID() { return m_solarSystem.constellationID; };
	std::vector<std::shared_ptr<Pilot>> getPilots();
	void setCurrentPilots(std::shared_ptr<Pilot> _Pilot);
	void clearCurrentPilots();
};
