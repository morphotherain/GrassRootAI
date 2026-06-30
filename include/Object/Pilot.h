#pragma once
#include "GameObject.h"
#include "Ship.h"
#include "SkillComponent.h"
#include "EntityTaskHandlers.h"

class Pilot : public GameObject
{
public:
	Pilot() = default;
	Pilot(UINT _objectID, UINT _PilotID);
	~Pilot() = default;

	virtual void Init();
	virtual void Update(UINT tick);

protected:
	void registerEntityTaskHandlers(EntityTaskHandlerMap& handlers) override;

public:
	UINT PilotID;
	UINT currentShipID = 0;
	UINT currentSolarSystemID = 0;
	std::shared_ptr<Ship> currentShip;
};
