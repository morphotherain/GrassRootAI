#include "SolarSystemHandler.h"
#include "SolarSystemMgr.h"
#include "dynGameObjectsManager.h"
#include "AttributesComponent.h"
#include "BaseComponent.h"
#include "GameObject.h"
#include "dynContainersManager.h"
#include "Task/TaskParams.h"
#include "Sim/IAssetLocationService.h"

bool CreateObjectHandler::handleTask(Task& task)
{
	try {
		const auto params = ReadCreateObjectParams(task);
		dynGameObject objectData;
		objectData.x = params.x;
		objectData.y = params.y;
		objectData.z = params.z;
		objectData.typeID = params.typeID;
		objectData.SolarSystemID = params.solarSystemID;
		objectData.OwnerKind = static_cast<int>(params.ownerID != 0 ? AssetOwnerKind::Character : AssetOwnerKind::None);
		objectData.OwnerID = params.ownerID;
		objectData.LocationKind = static_cast<int>(AssetLocationKind::ContainerBag);
		objectData.LocationRef = params.bagId;
		objectData.qw = params.qw;
		objectData.qx = params.qx;
		objectData.qy = params.qy;
		objectData.qz = params.qz;
		if (objectData.typeID == 0)return false;

		auto id = dynGameObjectsManager::getInstance()->insertGameObject(objectData);
		if (params.attributes.has_value()) {
			auto attributeComp = SolarSystemMgr::getInstance().getObjectById(id)->GetComponent<AttributesComponent>();
			if (attributeComp != nullptr) {
				attributeComp->UpdateAttributes(params.attributes.value());
				attributeComp->storeAttributes();
			}
		}
		return true;
	}
	catch (const std::bad_any_cast& e) {
		DEBUG_("类型转换错误: {}", e.what());
	}
	return false;
}

bool TransferObjectHandler::handleTask(Task& task)
{
	try {
		auto targetId = task.targetId;
		if (task.target.lock() != nullptr)
			targetId = task.target.lock()->GetComponent<BaseComponent>()->objectID;
		const auto params = ReadTransferObjectParams(task);
		if (!GetAssetLocationService().MoveToBag(static_cast<std::uint32_t>(targetId), params.bagId))
			return false;
		return true;
	}
	catch (const std::bad_any_cast& e) {
		DEBUG_("类型转换错误: {}", e.what());
	}
	return false;
}

bool DestroyObjectHandler::handleTask(Task& task)
{
	try {
		auto target = task.target.lock();
		if (!target)
			return false;
		auto id = target->GetComponent<BaseComponent>()->objectID;
		target->Destroy();
		SolarSystemMgr::getInstance().p_mapObject->erase(id);
		return true;
	}
	catch (const std::bad_any_cast& e) {
		DEBUG_("类型转换错误: {}", e.what());
	}
	return false;
}
