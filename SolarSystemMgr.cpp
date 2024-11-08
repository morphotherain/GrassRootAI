#include "SolarSystemMgr.h"

void SolarSystemMgr::Init()
{
	auto IDs = dynGameObjectsManager::getInstance()->getSolarSystemIDHasPilot();
	for (auto id : IDs) {
		auto p_solarSystem = std::make_shared<SolarSystem>(id);
		p_solarSystem->Init();
		SolarSystems.push_back(p_solarSystem);

		std::vector<std::shared_ptr<Pilot>> otherPilots = p_solarSystem->getPilots();
		Pilots.insert(Pilots.end(), otherPilots.begin(), otherPilots.end());
	}
	return;
}

void SolarSystemMgr::Update(UINT tick) {
	for (auto p : SolarSystems) {
		p->Update(tick);
	}
}

void SolarSystemMgr::getCurrentPilot()
{
	for (auto p : Pilots) {
		if (p->PilotID == currentPilotID)
		{
			currentPilot = p;
		}
	}
}

void SolarSystemMgr::setCurrentPilot()
{
	getCurrentPilot();
	for (auto pSolarSystem : SolarSystems) {
		pSolarSystem->setCurrentPilots(currentPilot);
		pSolarSystem->setCurrentSector();
	}
}

