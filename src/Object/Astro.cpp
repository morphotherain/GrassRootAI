#include "Astro.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "WarpGateComponent.h"
#include "mapJumpsManager.h"
#include "mapDenormalizeManager.h"
#include "mapSolarSystemsManager.h"

std::shared_ptr<GameObject> Astro::ConvertBasedOnGroupID(UINT groupID)
{
	switch (groupID) {
	case 6: return std::make_shared<Star>(objectID);
	case 7: return std::make_shared<Planet>(objectID);
	case 8: return std::make_shared<Moon>(objectID);
	case 9: return std::make_shared<AsteroidBelt>(objectID);
	case 10: return std::make_shared<StarGate>(objectID);
	}
	return nullptr;
}

void StarGate::Init()
{
	fillObjectName();
	initTaskHandlers();
}

void StarGate::Update(UINT tick)
{
	processTasks();
}

void StarGate::fillObjectName()
{
	auto* base = GetComponent<BaseComponent>();
	if (!base)
		return;

	UINT destID = mapJumpsManager::getInstance()->getDestinationIDByObjectId(base->objectID);
	UINT solarSystemID = mapDenormalizeManager::getInstance()->getSolarSystemIDByObjectId(destID);
	std::wstring solarSystemName = mapSolarSystemsManager::getInstance()->getNameById(solarSystemID);
	base->name = solarSystemName + L"（星门）";
}

void StarGate::initTaskHandlers() {
	taskHandlers = {
		{"", [this](const Task& task) {}},
		{"jump", [this](const Task& task) {
			auto publisherPtr = task.publisher.lock();
			auto targetPtr = task.target.lock();
			if (!publisherPtr || !targetPtr)
				return;

			auto* base = publisherPtr->GetComponent<BaseComponent>();
			auto* target_base = targetPtr->GetComponent<BaseComponent>();
			auto* tran = publisherPtr->GetComponent<SpaceTransformComponent>();
			auto* gateTran = GetComponent<SpaceTransformComponent>();
			if (!base || !target_base || !tran || !gateTran)
				return;

			auto distance = gateTran->calculateDistance(*tran);
			if (distance < (2500) || true) {
				UINT destID = mapJumpsManager::getInstance()->getDestinationIDByObjectId(target_base->objectID);
				UINT solarSystemID = mapDenormalizeManager::getInstance()->getSolarSystemIDByObjectId(destID);
				auto pos = mapDenormalizeManager::getInstance()->getPosByObjectID(destID);
				base->setSolarSystemID(solarSystemID);
				tran->x = pos[0] + 12000.0f;
				tran->y = pos[1];
				tran->z = pos[2];
			}
		}}
	};
}

void StarGate::handleTask(const Task& task)
{
	auto publisherPtr = task.publisher.lock();
	auto targetPtr = task.target.lock();
	auto taskType = task.getParamOrDefault<std::string>("taskType", "");

	if (!publisherPtr || !targetPtr) {
		return;
	}

	auto it = taskHandlers.find(taskType);
	if (it != taskHandlers.end()) {
		it->second(task);
	}
}
