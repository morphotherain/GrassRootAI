#pragma once
#include "GameObject.h"
#include "Ship.h"
class Pilot : public GameObject
{
public:
	Pilot() = default;
	Pilot(UINT _objectID, UINT _PilotID);
	~Pilot() = default;

	virtual void Init();
	virtual void Update(UINT tick);

	UINT PilotID;
	UINT currentShipID = 0;
	UINT currentSolarSystemID = 0;
	UINT PilotObjectID;
	std::shared_ptr<Ship> currentShip;
	std::shared_ptr<BaseComponent> m_pBase;
};
