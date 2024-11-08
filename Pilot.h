#pragma once
#include "GameObject.h"
#include "Ship.h"
class Pilot : public GameObject
{
public:
	Pilot() = default;
	Pilot(UINT _objectID, UINT _PilotID) :PilotObjectID(_objectID),PilotID(_PilotID)  {};
	~Pilot() = default;

	virtual void Init();

	UINT PilotID;
	UINT currentShipID;	
	UINT currentSolarSystemID;
	UINT PilotObjectID;
	std::shared_ptr<Ship> currentShip;
	std::shared_ptr<BaseComponent> m_pBase;

};

