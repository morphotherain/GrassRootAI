#pragma once
#include "GameObject.h"
#include "Ship.h"
#include "SkillComponent.h"

class Pilot : public GameObject
{
public:
	Pilot() = default;
	Pilot(UINT _objectID, UINT _PilotID);
	~Pilot() = default;

	virtual void Init();
	virtual void Update(UINT tick);
	virtual void handleTask(const Task& task);

	void initTaskHandlers();
	std::unordered_map<std::string, std::function<void(const Task&)>> taskHandlers;

	UINT PilotID;
	UINT objectID;
	UINT currentShipID = 0;
	UINT currentSolarSystemID = 0;
	std::shared_ptr<Ship> currentShip;
};
