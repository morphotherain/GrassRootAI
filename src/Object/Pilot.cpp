#include "Pilot.h"
#include "dynGameObjectsManager.h"

Pilot::Pilot(UINT _objectID, UINT _PilotID) :PilotObjectID(_objectID), PilotID(_PilotID)
{
	UINT containerID = dynGameObjectsManager::getInstance()->getContainerIdByObjectID(_objectID);
	currentSolarSystemID = dynGameObjectsManager::getInstance()->getSolarSystemIdByObjectID(containerID);
}

void Pilot::Init()
{
	m_pBase = std::make_shared<BaseComponent>(objectID);
	AddComponent<Component>(m_pBase);
}

void Pilot::Update(UINT tick)
{
	m_pBase->needStore = true;
	if (tick % 60 == 0) {
		if (m_pBase->needStore) {
			m_pBase->store();
		}
	}
}