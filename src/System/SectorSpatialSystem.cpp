#include "SectorSpatialSystem.h"

#include "BaseComponent.h"
#include "Pilot.h"
#include "SpaceTransformComponent.h"

void SectorSpatialSystem::Bind(
	int* solarSystemId,
	std::vector<std::weak_ptr<GameObject>>* spaceObjects,
	std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>* starGateTransferObjects,
	std::shared_ptr<Pilot>* currentPilot,
	std::shared_ptr<Sector>* currentSector)
{
	m_solarSystemId = solarSystemId;
	m_spaceObjects = spaceObjects;
	m_starGateTransferObjects = starGateTransferObjects;
	m_currentPilot = currentPilot;
	m_currentSector = currentSector;
}

void SectorSpatialSystem::Update(UINT tick)
{
	if (tick % 30 == 0) {
		checkObjectsInSector();
		setCurrentSector();
	}
}

void SectorSpatialSystem::addObjectToSector(std::shared_ptr<GameObject> object)
{
	auto* tran = object->GetComponent<SpaceTransformComponent>();
	if (!tran) {
		return;
	}

	auto sector = getSector(tran->x, tran->y, tran->z);
	if (sector == nullptr) {
		sector = addSector(tran->x, tran->y, tran->z);
	}
	sector->addObject(object);
}

void SectorSpatialSystem::clearCurrentSector()
{
	if (m_currentSector) {
		m_currentSector->reset();
	}
}

std::shared_ptr<Sector> SectorSpatialSystem::addSector(double x, double y, double z)
{
	std::shared_ptr<Sector> newSector = std::make_shared<Sector>();

	const long long int gridSideLength = 10000000;
	long long int xInt = static_cast<long long int>(x / gridSideLength);
	long long int yInt = static_cast<long long int>(y / gridSideLength);
	long long int zInt = static_cast<long long int>(z / gridSideLength);
	if (xInt < 0) xInt -= 1;
	if (yInt < 0) yInt -= 1;
	if (zInt < 0) zInt -= 1;

	std::unordered_map<long long int, std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>>::iterator outerIt;
	std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>::iterator middleIt;
	std::unordered_map<long long int, std::shared_ptr<Sector>>::iterator innerIt;

	outerIt = m_Sectors.find(xInt);
	if (outerIt == m_Sectors.end()) {
		std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>> newMiddleMap;
		m_Sectors[xInt] = newMiddleMap;
		outerIt = m_Sectors.find(xInt);
	}

	middleIt = outerIt->second.find(yInt);
	if (middleIt == outerIt->second.end()) {
		std::unordered_map<long long int, std::shared_ptr<Sector>> newInnerMap;
		outerIt->second[yInt] = newInnerMap;
		middleIt = outerIt->second.find(yInt);
	}

	innerIt = middleIt->second.find(zInt);
	if (innerIt == middleIt->second.end()) {
		double xFull = static_cast<double>(xInt * 10000000);
		double yFull = static_cast<double>(yInt * 10000000);
		double zFull = static_cast<double>(zInt * 10000000);
		newSector->x = xFull + newSector->radius;
		newSector->y = yFull + newSector->radius;
		newSector->z = zFull + newSector->radius;
		newSector->x_Min = xFull;
		newSector->y_Min = yFull;
		newSector->z_Min = zFull;
		newSector->x_Max = xFull + 2 * newSector->radius;
		newSector->y_Max = yFull + 2 * newSector->radius;
		newSector->z_Max = zFull + 2 * newSector->radius;

		middleIt->second[zInt] = newSector;
		innerIt = middleIt->second.find(zInt);
	}

	return innerIt->second;
}

std::shared_ptr<Sector> SectorSpatialSystem::getSector(double x, double y, double z)
{
	const long long int gridSideLength = 10000000;
	long long int xInt = static_cast<long long int>(x / gridSideLength);
	long long int yInt = static_cast<long long int>(y / gridSideLength);
	long long int zInt = static_cast<long long int>(z / gridSideLength);
	if (xInt < 0) xInt -= 1;
	if (yInt < 0) yInt -= 1;
	if (zInt < 0) zInt -= 1;

	std::unordered_map<long long int, std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>>::iterator outerIt;
	std::unordered_map<long long int, std::unordered_map<long long int, std::shared_ptr<Sector>>>::iterator middleIt;
	std::unordered_map<long long int, std::shared_ptr<Sector>>::iterator innerIt;

	outerIt = m_Sectors.find(xInt);
	if (outerIt != m_Sectors.end()) {
		middleIt = outerIt->second.find(yInt);
		if (middleIt != outerIt->second.end()) {
			innerIt = middleIt->second.find(zInt);
			if (innerIt != middleIt->second.end()) {
				return innerIt->second;
			}
		}
	}
	return nullptr;
}

void SectorSpatialSystem::checkObjectsInSector()
{
	if (!m_solarSystemId || !m_spaceObjects || !m_starGateTransferObjects) {
		return;
	}

	for (const auto& outerPair : m_Sectors) {
		const auto& middleMap = outerPair.second;

		for (const auto& middlePair : middleMap) {
			const auto& innerMap = middlePair.second;

			for (const auto& innerPair : innerMap) {
				const std::shared_ptr<Sector>& sector = innerPair.second;

				size_t size = sector->space_objects.size();
				for (int i = 0; i < static_cast<int>(size); ++i) {
					const auto object = sector->space_objects[i];
					const auto p = object.lock();
					if (!p)
					{
						sector->space_objects.erase(sector->space_objects.begin() + i);
						i--;
						size--;
						continue;
					}

					auto* tran = p->GetComponent<SpaceTransformComponent>();
					auto* base = p->GetComponent<BaseComponent>();

					if (base->solarSystemID != static_cast<UINT>(*m_solarSystemId)) {
						for (auto it = m_spaceObjects->begin(); it != m_spaceObjects->end(); ) {
							if ((*it).lock() == p) {
								m_spaceObjects->erase(it);
								(*m_starGateTransferObjects)->push_back(p);
								break;
							}
							else {
								++it;
							}
						}

						sector->space_objects.erase(sector->space_objects.begin() + i);
						i--;
						size--;
						continue;
					}

					if (sector->isInSector(tran->x, tran->y, tran->z)) {
						continue;
					}

					sector->space_objects.erase(sector->space_objects.begin() + i);
					addObjectToSector(p);
					i--;
					size--;
				}
			}
		}
	}
}

void SectorSpatialSystem::setCurrentSector()
{
	if (!m_currentPilot || !m_currentSector || !*m_currentPilot) {
		return;
	}

	auto ship = (*m_currentPilot)->currentShip;
	if (!ship) {
		return;
	}

	auto* tran = ship->GetComponent<SpaceTransformComponent>();
	if (!tran) {
		return;
	}

	auto sector = getSector(tran->x, tran->y, tran->z);
	if (sector == nullptr) {
		*m_currentSector = addSector(tran->x, tran->y, tran->z);
		return;
	}
	*m_currentSector = sector;
}
