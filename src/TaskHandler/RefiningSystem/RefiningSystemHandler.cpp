#include "RefiningSystemHandler.h"
#include "SolarSystemMgr.h"

#include "dynContainersManager.h"
#include "typeMaterialsManager.h"
#include "Task/TaskParams.h"

bool RefiningObjectHandler::handleTask(Task& task) {
	if (!task.publisher.lock() || !task.target.lock())
		return false;
	auto publisher = task.publisher.lock();
	auto target = task.target.lock();
	auto pilot = dynamic_cast<Pilot*>(publisher.get());
	auto pilotId = pilot->PilotID;
	const auto* targetBase = target->GetComponent<BaseComponent>();
	const UINT bagId = targetBase->locationRef;
	UINT typeId = target->GetComponent<BaseComponent>()->typeID;
	double amount = target->GetComponent<AttributesComponent>()->getAttrValueById(ATTR_ID_QUANTITY, 0.0);
	double volume = target->GetComponent<AttributesComponent>()->getAttrValueById(ATTR_ID_VOLUME, 1.0);

	std::vector<std::pair<int ,int>> materialTypeIds;
	typeMaterialsManager::getInstance()->getRefiningResultById(typeId, materialTypeIds);
	for (auto material : materialTypeIds) {
		CreateObjectParams createParams;
		createParams.typeID = material.first;
		createParams.ownerID = pilotId;
		createParams.bagId = static_cast<int>(bagId);
		createParams.attributes = std::vector<Attribute>{
			{ ATTR_ID_QUANTITY, static_cast<double>(material.second * amount * volume) },
		};
		TaskMgr::getInstance().addTask(TaskFactory::MakeCreateObjectTask(publisher, createParams));
	}
	TaskMgr::getInstance().addTask(TaskFactory::MakeDestroyObjectTask(publisher, target));
	return true;
}