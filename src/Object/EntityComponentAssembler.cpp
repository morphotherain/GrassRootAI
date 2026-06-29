#include "EntityComponentAssembler.h"
#include "GameObject.h"
#include "BaseComponent.h"
#include "AttributesComponent.h"
#include "SpaceTransformComponent.h"
#include "PhysicsComponent.h"
#include "EquipmentsComponent.h"
#include "StorageComponent.h"
#include "LockingComponent.h"
#include "StationComponent.h"
#include "WarpGateComponent.h"
#include "EquipmentComponent.h"
#include "WeaponComponent.h"
#include "MinerComponent.h"

void EntityComponentAssembler::Assemble(GameObject& object, EntityArchetype archetype, std::uint32_t objectID)
{
	switch (archetype)
	{
	case EntityArchetype::AstroCelestial:
		object.AddComponent<Component>(std::make_shared<BaseComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<SpaceTransformComponent>(objectID));
		break;

	case EntityArchetype::StarGate:
		object.AddComponent<Component>(std::make_shared<BaseComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<SpaceTransformComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<WarpGateComponent>(objectID));
		break;

	case EntityArchetype::NPCStation:
		object.AddComponent<Component>(std::make_shared<BaseComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<SpaceTransformComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<StationComponent>(objectID));
		break;

	case EntityArchetype::Mineral:
		object.AddComponent<Component>(std::make_shared<BaseComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<AttributesComponent>(objectID));
		break;

	case EntityArchetype::Ship:
		object.AddComponent<Component>(std::make_shared<BaseComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<AttributesComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<EquipmentsComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<SpaceTransformComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<PhysicsComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<PilotStorageComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<CargoContainerComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<HighSlotComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<MediumSlotComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<LowSlotComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<RigSlotComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<LockingComponent>(objectID));
		break;

	case EntityArchetype::Equipment:
		object.AddComponent<Component>(std::make_shared<BaseComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<AttributesComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<EquipmentComponent>(objectID));
		break;

	case EntityArchetype::EnergyWeaponEquipment:
		object.AddComponent<Component>(std::make_shared<BaseComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<AttributesComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<EquipmentComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<WeaponComponent>(objectID));
		break;

	case EntityArchetype::MinerEquipment:
		object.AddComponent<Component>(std::make_shared<BaseComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<AttributesComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<EquipmentComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<MinerComponent>(objectID));
		break;

	case EntityArchetype::Asteroid:
		object.AddComponent<Component>(std::make_shared<BaseComponent>(objectID));
		object.AddComponent<Component>(std::make_shared<AttributesComponent>(objectID));
		break;

	case EntityArchetype::None:
	default:
		break;
	}
}
