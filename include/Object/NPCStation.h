#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "StationComponent.h"
#include <functional>
#include <string>
#include <unordered_map>

struct Task;

class NPCStation : public GameObject {
public:
	NPCStation() = default;
	explicit NPCStation(UINT _objectID) { objectID = _objectID; }
	~NPCStation() = default;

	virtual void Init();
	virtual void Update(UINT tick);

	void fillObjectName();

	virtual void handleTask(const Task& task);
	std::unordered_map<std::string, std::function<void(const Task&)>> taskHandlers;
	void initTaskHandlers();
};
