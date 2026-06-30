#include "Astro.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "WarpGateComponent.h"
#include "mapJumpsManager.h"
#include "mapDenormalizeManager.h"
#include "mapSolarSystemsManager.h"

void StarGate::Init()
{
	fillObjectName();
	initEntityTaskHandlers();
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

void StarGate::registerEntityTaskHandlers(EntityTaskHandlerMap& handlers)
{
	handlers.emplace("", [this](const Task& task) {});
	handlers.emplace("jump", [this](const Task& task) {
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
			tran->store();
		}
	});
}
