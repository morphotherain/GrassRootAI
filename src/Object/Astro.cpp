#include "Astro.h"
#include "mapJumpsManager.h"
#include "mapDenormalizeManager.h"
#include "mapSolarSystemsManager.h"
#include "TransformComponent.h"

std::shared_ptr<GameObject> Astro::ConvertBasedOnGroupID(UINT groupID)
{
	switch (groupID) {
	case 6: {
		return std::make_shared<Star>(objectID);
	}
	case 7: {
		return std::make_shared<Planet>(objectID);
	}
	case 8: {
		return std::make_shared<Moon>(objectID);
	}
	case 9: {
		return std::make_shared<AsteroidBelt>(objectID);
	}
	case 10: {
		return std::make_shared<StarGate>(objectID);
	}
	}
	return nullptr;
}

void Star::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	AddComponent<Component>(m_pSpaceTran);
}

void Planet::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	AddComponent<Component>(m_pSpaceTran);
}

void Moon::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	AddComponent<Component>(m_pSpaceTran);
}

void AsteroidBelt::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	AddComponent<Component>(m_pSpaceTran);
}

void StarGate::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	AddComponent<Component>(m_pSpaceTran);
	m_pWarpGate = std::make_shared<WarpGateComponent>(objectID);
	AddComponent<Component>(m_pWarpGate);
	fillObjectName();
}

void StarGate::Update(UINT tick)
{
	processTasks();
}

void StarGate::fillObjectName()
{
	UINT destID = mapJumpsManager::getInstance()->getDestinationIDByObjectId(m_pBase->objectID);
	UINT solarSystemID = mapDenormalizeManager::getInstance()->getSolarSystemIDByObjectId(destID);
	std::wstring solarSystemName = mapSolarSystemsManager::getInstance()->getNameById(solarSystemID);
	m_pBase->name = solarSystemName + L"（星门）";
}

void StarGate::handleTask(const Task& task)
{
	// 从 weak_ptr 中获取 shared_ptr（需检查是否有效）
	auto publisherPtr = task.publisher.lock();
	auto targetPtr = task.target.lock();

	// 检查指针有效性（避免访问已销毁的对象）
	if (!publisherPtr || !targetPtr) {
		return;
	}

	switch (task.taskTypeId) {
	case 0:
	{
		auto base = publisherPtr->GetComponent<BaseComponent>();
		auto target_base = targetPtr->GetComponent<BaseComponent>();
		auto tran = publisherPtr->GetComponent<SpaceTransformComponent>();
		auto distance = m_pSpaceTran->calculateDistance(*tran);
		if (distance < (2500) || true) {
			UINT destID = mapJumpsManager::getInstance()->getDestinationIDByObjectId(target_base->objectID);
			UINT solarSystemID = mapDenormalizeManager::getInstance()->getSolarSystemIDByObjectId(destID);
			auto pos = mapDenormalizeManager::getInstance()->getPosByObjectID(destID);
			base->setSolarSystemID(solarSystemID);
			tran->x = pos[0] + 12000.0f;
			tran->y = pos[1];
			tran->z = pos[2];
		}
		else {
			break;
		}
		break;
	}
	default:;
	}
}