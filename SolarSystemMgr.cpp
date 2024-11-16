#include "SolarSystemMgr.h"

void SolarSystemMgr::Init()
{
	p_mapObject = std::make_shared<std::unordered_map<UINT, std::shared_ptr<GameObject>>>();
	auto IDs = dynGameObjectsManager::getInstance()->getSolarSystemIDHasPilot();
	for (auto id : IDs) {
		auto p_solarSystem = std::make_shared<SolarSystem>(id);
		p_solarSystem->p_mapObject = p_mapObject;
		p_solarSystem->Init();
		SolarSystems[id] = (p_solarSystem);

		std::vector<std::shared_ptr<Pilot>> otherPilots = p_solarSystem->getPilots();
		Pilots.insert(Pilots.end(), otherPilots.begin(), otherPilots.end());
	}
	return;
}

void SolarSystemMgr::Update(UINT tick) {
	for (const auto& pair : SolarSystems) {
		pair.second->Update(tick);
	}
}

void SolarSystemMgr::getCurrentPilot()
{
	for (auto p : Pilots) {
		if (p->PilotID == currentPilotID)
		{
			currentPilot = p;
			auto Base = currentPilot->GetComponent<BaseComponent>();
			currentSolarSystem = SolarSystems[Base->solarSystemID];
		}
	}
}

void SolarSystemMgr::setCurrentPilot()
{
	getCurrentPilot();
	for (const auto& pair : SolarSystems) {
		pair.second->setCurrentPilots(currentPilot);
		pair.second->setCurrentSector();
	}
}

