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
	distributeTasksFromTaskMgr();

	for (const auto& pair : SolarSystems) {
		pair.second->Update(tick);
	}
}

void SolarSystemMgr::distributeTasksFromTaskMgr()
{
	//TaskMgr& taskMgr = TaskMgr::getInstance();
	//auto allTasks = taskMgr.tasks;

	//for (const auto& task : allTasks) {
	//	UINT targetObjectId = task->targetId;  // 假设Task结构体中有objectId属性用于指定目标GameObject的ID
	//	auto it = p_mapObject->find(targetObjectId);
	//	if (it != p_mapObject->end()) {
	//		// 找到了对应的GameObject，这里可以进行任务分发相关的操作，比如将任务添加到GameObject的任务队列中（假设GameObject有这样的结构来存储待处理任务）
	//		std::shared_ptr<GameObject>& targetGameObject = it->second;
	//		targetGameObject->addTask(task);  // 假设GameObject类有addTask方法来接收任务
	//	}
	//	else {
	//		// 如果没找到对应的GameObject，可以选择记录日志或者进行其他错误处理逻辑，这里简单打印提示信息
	//	}
	//}
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

