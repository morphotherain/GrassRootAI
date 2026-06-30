#include "ObjectFactory.h"
#include "EntityArchetype.h"
#include "EntityComponentAssembler.h"
#include "Astro.h"
#include "NPCStation.h"
#include "Material.h"
#include "Ship.h"
#include "Equipment.h"
#include "Asteroid.h"

namespace
{
	std::shared_ptr<GameObject> CreatePrototype(UINT categoryID, UINT groupID, UINT objectID)
	{
		switch (categoryID)
		{
		case 2:
			switch (groupID)
			{
			case 6: return std::make_shared<Star>(objectID);
			case 7: return std::make_shared<Planet>(objectID);
			case 8: return std::make_shared<Moon>(objectID);
			case 9: return std::make_shared<AsteroidBelt>(objectID);
			case 10: return std::make_shared<StarGate>(objectID);
			default: return std::make_shared<Astro>(objectID);
			}
		case 3:
			return std::make_shared<NPCStation>(objectID);
		case 4:
			switch (groupID)
			{
			case 18: return std::make_shared<Mineral>(objectID);
			default: return std::make_shared<Material>(objectID);
			}
		case 5:
			return nullptr;
		case 6:
			switch (groupID)
			{
			case 25: return std::make_shared<Frigate>(objectID);
			case 26: return std::make_shared<Cruiser>(objectID);
			case 27: return std::make_shared<Battleship>(objectID);
			case 29: return std::make_shared<Capsule>(objectID);
			default: return std::make_shared<Ship>(objectID);
			}
		case 7:
			switch (groupID)
			{
			case 46: return std::make_shared<ThrustEquipments>(objectID);
			case 53: return std::make_shared<EnergyWeaponEquipments>(objectID);
			case 54: return std::make_shared<MinerEquipments>(objectID);
			case 546: return std::make_shared<MiningImprovementEquipments>(objectID);
			default: return std::make_shared<Equipment>(objectID);
			}
		case 25:
			switch (groupID)
			{
			case 462: return std::make_shared<Veldspar>(objectID);
			default: return std::make_shared<Asteroid>(objectID);
			}
		default:
			return nullptr;
		}
	}
}

std::shared_ptr<GameObject> ObjectFactory::CreateFromDynObject(const dynGameObject& objectData)
{
	auto object = CreatePrototype(objectData.categoryID, objectData.groupID, objectData.objectID);
	if (!object)
	{
		return nullptr;
	}

	const EntityArchetype archetype = ResolveEntityArchetype(*object);
	EntityComponentAssembler::Assemble(*object, archetype, objectData.objectID);
	object->Init();
	return object;
}
