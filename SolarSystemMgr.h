#pragma once
#include "SolarSystem.h"
#include "Ship.h"
#include "Pilot.h"
#include "dynGameObjectsManager.h"

#include "TaskMgr.h"

class SolarSystemMgr {
public :

	// 获取单例
	static SolarSystemMgr& getInstance() {
		static SolarSystemMgr instance;
		return instance;
	}

	SolarSystemMgr() = default;
	~SolarSystemMgr() = default;

	std::unordered_map<UINT, std::shared_ptr<SolarSystem>> SolarSystems;
	/*std::vector<std::shared_ptr<SolarSystem>> SolarSystems;*/
	std::shared_ptr<SolarSystem> currentSolarSystem;
	std::vector<std::shared_ptr<Pilot>> Pilots;
	UINT currentPilotID = 0;
	std::shared_ptr<Pilot> currentPilot;

	std::shared_ptr<std::unordered_map<UINT, std::shared_ptr<GameObject>>> p_mapObject;

	void Init();
	void getCurrentPilot();
	void setCurrentPilot();
	void Update(UINT tick);

	void distributeTasksFromTaskMgr();

private:
};