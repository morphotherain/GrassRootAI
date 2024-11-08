#include "Ship.h"

void Ship::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	AddComponent<Component>(m_pSpaceTran);
	m_pPhysics = std::make_shared<PhysicsComponent>(objectID);
	AddComponent<Component>(m_pPhysics);
	m_pPhysics->SpaceTran = m_pSpaceTran;
}

void Ship::Update(UINT tick)
{
	m_pPhysics->Update(tick);
}
