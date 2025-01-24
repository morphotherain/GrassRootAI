#include "SolarSystemMgr.h"
#include "dynContainersManager.h"

void SolarSystemMgr::Init()
{
	p_mapObject = std::make_shared<std::unordered_map<UINT, std::shared_ptr<GameObject>>>();
	p_starGateTransferObjects = std::make_shared < std::vector<std::shared_ptr<GameObject>>>();
	auto IDs = dynGameObjectsManager::getInstance()->getSolarSystemIDHasPilot();
	InitPilots();

	for (auto id : IDs) {
		loadSolarSystem(id);
	}
	return;
}

void SolarSystemMgr::Update(UINT tick) {
	distributeTasksFromTaskMgr();

	if (tick % 60 == 0) {
		handleStarGateTransferObjects();
	}
	for (const auto& pilot : Pilots) {
		pilot->Update(tick);
	}

	for (const auto& pair : SolarSystems) {
		pair.second->Update(tick);
	}
}

std::shared_ptr<SolarSystem> SolarSystemMgr::loadSolarSystem(int id)
{
	auto p_solarSystem = std::make_shared<SolarSystem>(id);
	p_solarSystem->p_mapObject = p_mapObject;
	p_solarSystem->p_starGateTransferObjects = p_starGateTransferObjects;
	p_solarSystem->Init();
	SolarSystems[id] = (p_solarSystem);

	std::vector<std::shared_ptr<Pilot>> otherPilots = p_solarSystem->getPilots();
	Pilots.insert(Pilots.end(), otherPilots.begin(), otherPilots.end());

	return p_solarSystem;
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

void SolarSystemMgr::handleStarGateTransferObjects()
{
	for (auto p : (*p_starGateTransferObjects)) {
		auto solarSystemID = p->GetComponent<BaseComponent>()->solarSystemID;
		auto it = SolarSystemMgr::getInstance().SolarSystems.find(solarSystemID);
		auto solarSystem = currentSolarSystem;
		if (it != SolarSystemMgr::getInstance().SolarSystems.end()) {
			// 找到了对应的太阳系，获取其值
			solarSystem = it->second;
		}
		else {
			solarSystem = loadSolarSystem(solarSystemID);
		}
		solarSystem->space_objects.push_back(p);
		solarSystem->addObjectToSector(p);
	}
	(*p_starGateTransferObjects).clear();
}

void SolarSystemMgr::InitPilots()
{
	auto p_PilotsData = dynGameObjectsManager::getInstance()->getPilots();
	for (auto& p : *p_PilotsData) {
		auto object = std::make_shared<Pilot>(p.objectID, p.OwnerID);
		object->Init();
		(*p_mapObject)[p.objectID] = (object);
		object->objectID = p.objectID;
		Pilots.push_back(std::dynamic_pointer_cast<Pilot>(object));

		if (object->GetComponent<BaseComponent>() != nullptr) {
			auto base = object->GetComponent<BaseComponent>();
			base->objectID = p.objectID;
			base->typeID = p.typeID;
			base->ownerID = p.OwnerID;
			base->containerID = p.ContainerID;
			base->solarSystemID = p.SolarSystemID;
			base->groupID = p.groupID;
			base->categoryID = p.categoryID;
			if (p.name != L"" && base->name == L"") {
				base->name = p.name;
			}
		}
	}
}


void SolarSystemMgr::getCurrentPilot()
{
	for (auto p : Pilots) {
		if (p->PilotID == currentPilotID)
		{
			currentPilot = p;
			auto Base = currentPilot->GetComponent<BaseComponent>();
			currentSolarSystem = SolarSystems[currentPilot->currentSolarSystemID];
			UINT PilotContainerID = currentPilot->GetComponent<BaseComponent>()->containerID;
			UINT shipID = dynContainersManager::getInstance()->getObjectIDByContainerID(PilotContainerID);
			currentPilot->currentShip = std::reinterpret_pointer_cast<Ship>((*p_mapObject)[shipID]);
		}
	}
}

void SolarSystemMgr::setCurrentPilot()
{
	currentSolarSystem->setCurrentPilots(currentPilot);
	currentSolarSystem->setCurrentSector();
}

