#pragma once
#include "SolarSystem.h"
#include "Ship.h"
#include "Pilot.h"
#include "dynGameObjectsManager.h"

#include "TaskMgr.h"
#include "TaskHandler.h"

class SolarSystemMgr {
public:

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
	UINT currentPilotID = 1;

	//缓冲区相关
	std::shared_ptr < std::vector<std::shared_ptr<GameObject>>> p_starGateTransferObjects;

	std::shared_ptr<std::unordered_map<UINT, std::shared_ptr<GameObject>>> p_mapObject;

	void Init();
	void getCurrentPilot();
	void setCurrentPilot();
	void Update(UINT tick);
	void handleTask(Task& task);

	std::shared_ptr<SolarSystem> loadSolarSystem(int id);
	void switchToSolarSystem(UINT solarSystemID);

	void distributeTasksFromTaskMgr();
	void handleStarGateTransferObjects();

	void Shutdown();

	void InitPilots();

	std::shared_ptr<GameObject> getObjectById(int id);
	UINT getOwnerIdById(int id);

private:
	std::unordered_map<std::string, std::unique_ptr<TaskHandler>> taskHandlers;

public:
	// 注册Handler
	void registerHandler(std::unique_ptr<TaskHandler> handler) {
		auto type = handler->getTaskType();
		taskHandlers[type] = std::move(handler);
	}

};