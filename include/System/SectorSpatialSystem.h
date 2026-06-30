#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "GameObject.h"
#include "Sector.h"

class Pilot;

class SectorSpatialSystem {
public:
	void Bind(
		int* solarSystemId,
		std::vector<std::weak_ptr<GameObject>>* spaceObjects,
		std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>* starGateTransferObjects,
		std::shared_ptr<Pilot>* currentPilot,
		std::shared_ptr<Sector>* currentSector);

	void Update(UINT tick);
	void addObjectToSector(std::shared_ptr<GameObject> object);
	void clearCurrentSector();
	void setCurrentSector();

private:
	int* m_solarSystemId = nullptr;
	std::vector<std::weak_ptr<GameObject>>* m_spaceObjects = nullptr;
	std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>* m_starGateTransferObjects = nullptr;
	std::shared_ptr<Pilot>* m_currentPilot = nullptr;
	std::shared_ptr<Sector>* m_currentSector = nullptr;

	std::unordered_map<long long int, std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>> m_Sectors;

	std::shared_ptr<Sector> addSector(double x, double y, double z);
	std::shared_ptr<Sector> getSector(double x, double y, double z);
	void checkObjectsInSector();
};
