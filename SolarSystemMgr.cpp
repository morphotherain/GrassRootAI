#include "SolarSystemMgr.h"
#include "dynContainersManager.h"

void SolarSystemMgr::Init()
{
	p_mapObject = std::make_shared<std::unordered_map<UINT, std::shared_ptr<GameObject>>>();
	GameObjectMgr::getInstance().setObjectMap(p_mapObject);
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

void SolarSystemMgr::handleTask(const Task& task)
{
	if (task.paramsPtr->find("createObject") != task.paramsPtr->end()) {
		try {
			dynGameObject objectData;
			objectData.x = task.getParamOrDefault<double>("x", 0.0);
			objectData.y = task.getParamOrDefault("y", 0.0);
			objectData.z = task.getParamOrDefault("z", 0.0);
			objectData.typeID = task.getParamOrDefault("typeID", 0);
			objectData.SolarSystemID = task.getParamOrDefault("SolarSystemID", 0);
			objectData.OwnerID = task.getParamOrDefault("OwnerID", 0);
			objectData.ContainerID = task.getParamOrDefault("ContainerID", 0);
			objectData.qw = task.getParamOrDefault("qw", 0.0);
			objectData.qx = task.getParamOrDefault("qx", 0.0);
			objectData.qy = task.getParamOrDefault("qy", 0.0);
			objectData.qz = task.getParamOrDefault("qz", 0.0);
			if (objectData.typeID == 0)return;

			auto id = dynGameObjectsManager::getInstance()->insertGameObject(objectData);
			if (task.paramsPtr->find("attributes") != task.paramsPtr->end()) {
				try {
					auto attributeVec = std::any_cast<std::vector<Attribute>>((*task.paramsPtr)["attributes"]);
					auto attributeComp = getObjectById(id)->GetComponent<AttributesComponent>();
					if (attributeComp != nullptr) {
						attributeComp->UpdateAttributes(attributeVec);
						attributeComp->storeAttributes();
					}
				}
				catch (const std::bad_any_cast& e) {
					DEBUG_("类型转换错误: {}", e.what());
				}
			}
		}
		catch (const std::bad_any_cast& e) {
			DEBUG_("类型转换错误: {}", e.what());
		}
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
	auto& taskMgr = TaskMgr::getInstance();
	while (!taskMgr.isEmpty()) {
		auto task = taskMgr.popTask();
		if (task) {

			if (task->paramsPtr->find("ToSolarSystemMgr") != task->paramsPtr->end())
			{
				handleTask(*task);
			}
			else
			{
				if (task->publisherId != -1) {
					task->publisher = getObjectById(task->publisherId);
				}
				if (task->targetId != -1) {
					task->target = getObjectById(task->targetId);
				}
				DEBUG_("Processing task: ", task->to_string_abstract());
				task->target->handleTask(*task);
			}
			
		}
	}
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

std::shared_ptr<GameObject> SolarSystemMgr::getObjectById(int id)
{
	if (p_mapObject) {
		auto it = p_mapObject->find(id);
		if (it != p_mapObject->end()) {
			return it->second;
		}
		DEBUG_("尝试加载object : {}",id);
		auto objectData = dynGameObjectsManager::getInstance()->getGameObjectByObjectID(id);
		if (SolarSystems.begin() != SolarSystems.end()) 
		{
			SolarSystems.begin()->second->addGameObject(objectData);
			auto it = p_mapObject->find(id);
			if (it != p_mapObject->end()) {
				return it->second;
			}
		}
	}
	return nullptr;
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