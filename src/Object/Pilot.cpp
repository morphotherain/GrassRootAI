#include "Pilot.h"
#include "EntityComponentAssembler.h"
#include "EntityArchetype.h"
#include "BaseComponent.h"
#include "SkillComponent.h"
#include "dynGameObjectsManager.h"
#include "dynContainersManager.h"
#include "Sim/IAssetLocationService.h"

Pilot::Pilot(UINT _objectID, UINT _PilotID) : PilotID(_PilotID)
{
	objectID = _objectID;
	currentSolarSystemID = GetAssetLocationService().ResolveContextSolarSystemId(_objectID);
}

void Pilot::Init()
{
	EntityComponentAssembler::Assemble(*this, EntityArchetype::Pilot, objectID);
	ResolveDependencies();
	initEntityTaskHandlers();
}

void Pilot::Update(UINT tick)
{
	if (auto* base = GetComponent<BaseComponent>())
	{
		base->MarkDirty();
		base->Update(tick);
	}
	if (tick % 60 == 0)
	{
		if (auto* skills = GetComponent<SkillComponent>())
		{
			skills->Update(tick);
		}
	}
}

void Pilot::registerEntityTaskHandlers(EntityTaskHandlerMap& handlers)
{
	handlers.emplace("", [this](const Task& task) {});
	handlers.emplace("skillComponent", [this](const Task& task) {
		if (auto* skills = GetComponent<SkillComponent>())
		{
			skills->handleTask(task);
		}
	});
}
