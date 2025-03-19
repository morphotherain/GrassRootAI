#include "Equipment.h"
#include "mapJumpsManager.h"
#include "mapDenormalizeManager.h"
#include "mapSolarSystemsManager.h"

std::shared_ptr<GameObject> Equipment::ConvertBasedOnGroupID(UINT groupID)
{
	switch (groupID) {
	case 54: {                                   
		return std::make_shared<Miner>(objectID);        //采矿激光器
	}

	}
	return nullptr;
}

void Miner::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
	m_pSpaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	AddComponent<Component>(m_pSpaceTran);
}
