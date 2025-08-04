#include "RefiningSystemHandler.h"
#include "SolarSystemMgr.h"

#include "dynContainersManager.h"
#include "typeMaterialsManager.h"

bool RefiningObjectHandler::handleTask(Task& task) {
	if (!task.publisher.lock() || !task.target.lock())
		return false;
	auto publisher = task.publisher.lock();
	auto target = task.target.lock();
	auto pilot = dynamic_cast<Pilot*>(publisher.get());
	auto pilotId = pilot->PilotID;
	UINT containerId = target->GetComponent<BaseComponent>()->containerID;
	UINT typeId = target->GetComponent<BaseComponent>()->typeID;
	double amount = target->GetComponent<AttributesComponent>()->getAttrValueById(ATTR_ID_QUANTITY, 0.0);
	double volume = target->GetComponent<AttributesComponent>()->getAttrValueById(ATTR_ID_VOLUME, 1.0);

	std::vector<std::pair<int ,int>> materialTypeIds;
	typeMaterialsManager::getInstance()->getRefiningResultById(typeId, materialTypeIds);
	for (auto material : materialTypeIds) {
		std::shared_ptr<Task> pTask = std::make_shared<Task>();
		pTask->targetSystem = SOLAR_SYSTEM;
		pTask->publisher = publisher;
		(*pTask->paramsPtr)["handlerType"] = std::string("createObject");
		(*pTask->paramsPtr)["typeID"] = static_cast<int>(material.first);
		(*pTask->paramsPtr)["OwnerID"] = pilotId;
		(*pTask->paramsPtr)["ContainerID"] = static_cast<int>(containerId);
		std::vector<Attribute> attributes = {
			{ATTR_ID_QUANTITY, static_cast<double>(material.second * amount * volume) },
		};
		(*pTask->paramsPtr)["attributes"] = attributes;

		TaskMgr::getInstance().addTask(pTask);
	}
	std::shared_ptr<Task> pTask = std::make_shared<Task>();
	pTask->target = target;
	pTask->publisher = publisher;
	pTask->targetSystem = SOLAR_SYSTEM;
	(*pTask->paramsPtr)["handlerType"] = std::string("destroyObject");

	TaskMgr::getInstance().addTask(pTask);
	return true;
}