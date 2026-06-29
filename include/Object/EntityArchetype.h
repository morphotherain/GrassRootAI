#pragma once

#include <cstdint>

enum class EntityArchetype : std::uint8_t
{
	None = 0,
	AstroCelestial,
	StarGate,
	NPCStation,
	Mineral,
	Ship,
	Equipment,
	EnergyWeaponEquipment,
	MinerEquipment,
	Asteroid,
};

class GameObject;

EntityArchetype ResolveEntityArchetype(GameObject& object);
