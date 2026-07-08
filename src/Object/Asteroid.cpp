#include "Asteroid.h"
#include "BaseComponent.h"
#include "SpaceTransformComponent.h"
#include "InvTypesManager.h"
#include "TaskMgr.h"
#include "Task/TaskParams.h"

Asteroid::~Asteroid()
{
}

void Asteroid::Init()
{
	auto spaceTran = std::make_shared<SpaceTransformComponent>(objectID);
	if (spaceTran->x == 0.0f && spaceTran->y == 0.0f && spaceTran->z == 0.0f)
	{
		spaceTran.reset();
	}
	else
	{
		AddComponent<Component>(spaceTran);
	}

	if (auto* base = GetComponent<BaseComponent>())
	{
		base->name = InvTypesManager::getInstance()->getNameByTypeId(base->typeID);
	}
}

void Asteroid::handleTask(const Task& task)
{
	auto* attributes = GetComponent<AttributesComponent>();
	auto* base = GetComponent<BaseComponent>();
	if (!attributes || !base)
		return;

	auto type = ReadEntityTaskType(task);
	if (type == EntityTaskType::AddObject) {
		const auto addParams = TryReadAddObjectParams(task);
		if (!addParams.has_value())
			return;
		const auto& addType = addParams->addType;
		const auto addTargetId = addParams->addTargetId;
		const auto bagId = addParams->bagId;
		const auto volume = addParams->volume;
		auto volumePerUnit = (*attributes->objectAttributes)[ATTR_ID_VOLUME].value;

		if(addType == "add") {
			auto addTarget = GameObjectMgr::getInstance().getObject(addTargetId);
			if (addTarget) {
				auto attr = addTarget->GetComponent<AttributesComponent>();
				double itemVolume = attr->getAttrValueById(ATTR_ID_QUANTITY, 0.0);
				itemVolume += std::floor(volume / volumePerUnit);
				attr->setAttrValueById(ATTR_ID_QUANTITY, itemVolume);
				attr->storeAttributes();
			}
		}
		if (addType == "create") {
			CreateObjectParams createParams;
			createParams.typeID = static_cast<int>(base->typeID);
			createParams.ownerID = 0;
			createParams.bagId = bagId;
			createParams.attributes = std::vector<Attribute>{
				{ ATTR_ID_QUANTITY, std::floor(volume / volumePerUnit) },
			};

			auto pTask = TaskFactory::MakeCreateObjectTask(nullptr, createParams);
			pTask->isInnerTask = true;
			pTask->publisherId = objectID;
			TaskMgr::getInstance().addTask(pTask);
		}
	}
}
