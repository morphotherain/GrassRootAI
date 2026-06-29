#pragma once

#include "EntityArchetype.h"
#include <cstdint>

class GameObject;

class EntityComponentAssembler
{
public:
	static void Assemble(GameObject& object, EntityArchetype archetype, std::uint32_t objectID);
};
