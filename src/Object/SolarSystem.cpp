#include "SolarSystem.h"
#include "System/EntityUpdateSystem.h"
#include "System/SectorSpatialSystem.h"
#include "InvTypesManager.h"
#include "invGroupsManager.h"
#include "eveBracketsManager.h"
#include "ObjectFactory.h"
#include "mapDenormalizeManager.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"

SolarSystem::SolarSystem(int id) : m_solarSystem(id)
{
}

SolarSystem::~SolarSystem() = default;

void SolarSystem::Init()
{
	m_entityUpdate = std::make_unique<EntityUpdateSystem>();
	m_entityUpdate->Bind(&space_objects);

	m_sectorSpatial = std::make_unique<SectorSpatialSystem>();
	m_sectorSpatial->Bind(
		&m_solarSystem.solarSystemID,
		&space_objects,
		&p_starGateTransferObjects,
		&currentPilot,
		&currentSector);

	getDenormalizesBySolarSystemID();
	auto p_vGameObjectData = dynGameObjectsManager::getInstance()->getGameObjectBySolarSystemID(m_solarSystem.solarSystemID);
	for (auto& objectData : *p_vGameObjectData) {
		addGameObject(objectData);
	}
	for (auto p_object : space_objects) {
		auto p = p_object.lock();
		if (p)
			addObjectToSector(p);
	}
}

void SolarSystem::Update(UINT tick)
{
	if (m_entityUpdate) {
		m_entityUpdate->Update(tick);
	}
	if (m_sectorSpatial) {
		m_sectorSpatial->Update(tick);
	}
}

void SolarSystem::getDenormalizesBySolarSystemID()
{
	auto rows = mapDenormalizeManager::getInstance()->queryStaticObjectsBySolarSystemID(
		static_cast<int>(m_solarSystem.solarSystemID));

	for (auto& row : rows)
	{
		auto p_denormalize = std::make_shared<DenormalizeData>(row.denormalize);
		m_denormalizes.push_back(p_denormalize);
		addGameObject(row.gameObject);
	}

	for (auto p_denormalize : m_denormalizes)
	{
		p_denormalize->groupID = InvTypesManager::getInstance()->getGroupByTypeId(p_denormalize->typeID);
		p_denormalize->bracketID = invGroupsManager::getInstance()->getBracketIDByGroupId(p_denormalize->groupID);
		p_denormalize->dds_path = eveBracketsManager::getInstance()->getPathByTypeId(p_denormalize->bracketID);
	}
}

void SolarSystem::addGameObject(dynGameObject& objectData)
{
	if (p_mapObject->find(objectData.objectID) != p_mapObject->end())
	{
		return;
	}

	auto object = ObjectFactory::CreateFromDynObject(objectData);
	if (!object)
	{
		return;
	}

	(*p_mapObject)[objectData.objectID] = object;
	object->objectID = objectData.objectID;
	if (object->GetComponent<SpaceTransformComponent>() != nullptr)
	{
		space_objects.push_back(object);
	}
	if (object->GetComponent<BaseComponent>() != nullptr)
	{
		auto base = object->GetComponent<BaseComponent>();
		base->objectID = objectData.objectID;
		base->typeID = objectData.typeID;
		base->ownerKind = objectData.OwnerKind;
		base->ownerID = objectData.OwnerID;
		base->locationKind = objectData.LocationKind;
		base->locationRef = objectData.LocationRef;
		base->solarSystemID = objectData.SolarSystemID;
		base->groupID = objectData.groupID;
		base->categoryID = objectData.categoryID;
		if (objectData.name != L"" && base->name == L"")
		{
			base->name = objectData.name;
		}
	}
}

long long int SolarSystem::CalculateHashIndex(double x, double y, double z)
{
	return 0;
}

void SolarSystem::addObjectToSector(std::shared_ptr<GameObject> object)
{
	if (m_sectorSpatial) {
		m_sectorSpatial->addObjectToSector(object);
	}
}

void SolarSystem::setCurrentSector()
{
	if (m_sectorSpatial) {
		m_sectorSpatial->setCurrentSector();
	}
}

void SolarSystem::clearCurrentSector()
{
	if (m_sectorSpatial) {
		m_sectorSpatial->clearCurrentSector();
	}
}

std::vector<std::shared_ptr<Pilot>> SolarSystem::getPilots()
{
	return Pilot_objects;
}

void SolarSystem::setCurrentPilots(std::shared_ptr<Pilot> _Pilot)
{
	currentPilot = _Pilot;
	if (auto* base = currentPilot->GetComponent<BaseComponent>())
	{
		base->setSolarSystemID(getSolarSystemID());
	}
}

void SolarSystem::clearCurrentPilots()
{
	currentPilot.reset();
}
