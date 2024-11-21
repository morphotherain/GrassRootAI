#include "Astro.h"

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
}
