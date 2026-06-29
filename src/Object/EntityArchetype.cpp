#include "EntityArchetype.h"
#include "Ship.h"
#include "Equipment.h"
#include "Astro.h"
#include "NPCStation.h"
#include "Asteroid.h"
#include "Material.h"

EntityArchetype ResolveEntityArchetype(GameObject& object)
{
	if (dynamic_cast<StarGate*>(&object))
	{
		return EntityArchetype::StarGate;
	}
	if (dynamic_cast<Star*>(&object) || dynamic_cast<Planet*>(&object) ||
		dynamic_cast<Moon*>(&object) || dynamic_cast<AsteroidBelt*>(&object))
	{
		return EntityArchetype::AstroCelestial;
	}
	if (dynamic_cast<NPCStation*>(&object))
	{
		return EntityArchetype::NPCStation;
	}
	if (dynamic_cast<Mineral*>(&object))
	{
		return EntityArchetype::Mineral;
	}
	if (dynamic_cast<EnergyWeaponEquipments*>(&object))
	{
		return EntityArchetype::EnergyWeaponEquipment;
	}
	if (dynamic_cast<MinerEquipments*>(&object))
	{
		return EntityArchetype::MinerEquipment;
	}
	if (dynamic_cast<Equipment*>(&object))
	{
		return EntityArchetype::Equipment;
	}
	if (dynamic_cast<Ship*>(&object))
	{
		return EntityArchetype::Ship;
	}
	if (dynamic_cast<Asteroid*>(&object))
	{
		return EntityArchetype::Asteroid;
	}
	return EntityArchetype::None;
}
