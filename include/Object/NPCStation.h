#pragma once
#include "GameObject.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "StationComponent.h"
#include "EntityTaskHandlers.h"
#include <functional>
#include <memory>
#include <string>

struct Task;

class NPCStation : public GameObject {
public:
	NPCStation() = default;
	explicit NPCStation(UINT _objectID) { objectID = _objectID; }
	~NPCStation() = default;

	virtual void Init();
	virtual void Update(UINT tick);

	void fillObjectName();

protected:
	void registerEntityTaskHandlers(EntityTaskHandlerMap& handlers) override;
};
