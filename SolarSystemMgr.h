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

	//总体相关
	std::unordered_map<UINT, std::shared_ptr<SolarSystem>> SolarSystems;
	std::vector<std::shared_ptr<Pilot>> Pilots;

	//当前相关
	std::shared_ptr<SolarSystem> currentSolarSystem;
	std::shared_ptr<Pilot> currentPilot;
	UINT currentPilotID = 0;

	//缓冲区相关
	std::shared_ptr < std::vector<std::shared_ptr<GameObject>>> p_starGateTransferObjects;

	std::shared_ptr<std::unordered_map<UINT, std::shared_ptr<GameObject>>> p_mapObject;

	void Init();
	void getCurrentPilot();
	void setCurrentPilot();
	void Update(UINT tick);

	std::shared_ptr<SolarSystem> loadSolarSystem(int id);

	void distributeTasksFromTaskMgr();
	void handleStarGateTransferObjects();

private:
};