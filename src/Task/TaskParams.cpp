#include "Task/TaskParams.h"
#include "GameObject.h"

namespace {
	constexpr const char* kTaskType = "taskType";
	constexpr const char* kHandlerType = "handlerType";
	constexpr const char* kEquipmentTaskType = "equipmentTaskType";
	constexpr const char* kStorageTaskType = "storageTaskType";
	constexpr const char* kSkillTaskType = "skillTaskType";
}

void ApplyEntityTaskType(Task& task, const std::string& taskType)
{
	(*task.paramsPtr)[kTaskType] = taskType;
}

std::string ReadEntityTaskType(const Task& task)
{
	return task.getParamOrDefault<std::string>(kTaskType, "");
}

void ApplySolarSystemHandlerType(Task& task, const std::string& handlerType)
{
	(*task.paramsPtr)[kHandlerType] = handlerType;
}

CreateObjectParams ReadCreateObjectParams(const Task& task)
{
	CreateObjectParams params;
	params.typeID = task.getParamOrDefault("typeID", 0);
	params.ownerID = task.getParamOrDefault("OwnerID", 0);
	params.bagId = task.getParamOrDefault("ContainerID", 0);
	params.solarSystemID = task.getParamOrDefault("SolarSystemID", 0);
	params.x = task.getParamOrDefault("x", 0.0);
	params.y = task.getParamOrDefault("y", 0.0);
	params.z = task.getParamOrDefault("z", 0.0);
	params.qw = task.getParamOrDefault("qw", 0.0);
	params.qx = task.getParamOrDefault("qx", 0.0);
	params.qy = task.getParamOrDefault("qy", 0.0);
	params.qz = task.getParamOrDefault("qz", 0.0);
	if (task.paramsPtr->find("attributes") != task.paramsPtr->end()) {
		try {
			params.attributes = std::any_cast<std::vector<Attribute>>((*task.paramsPtr)["attributes"]);
		}
		catch (const std::bad_any_cast&) {
		}
	}
	return params;
}

void ApplyCreateObjectParams(Task& task, const CreateObjectParams& params)
{
	ApplySolarSystemHandlerType(task, SolarSystemHandlerType::CreateObject);
	(*task.paramsPtr)["createObject"] = 0;
	(*task.paramsPtr)["typeID"] = params.typeID;
	(*task.paramsPtr)["OwnerID"] = params.ownerID;
	(*task.paramsPtr)["ContainerID"] = params.bagId;
	(*task.paramsPtr)["SolarSystemID"] = params.solarSystemID;
	(*task.paramsPtr)["x"] = params.x;
	(*task.paramsPtr)["y"] = params.y;
	(*task.paramsPtr)["z"] = params.z;
	(*task.paramsPtr)["qw"] = params.qw;
	(*task.paramsPtr)["qx"] = params.qx;
	(*task.paramsPtr)["qy"] = params.qy;
	(*task.paramsPtr)["qz"] = params.qz;
	if (params.attributes.has_value()) {
		(*task.paramsPtr)["attributes"] = params.attributes.value();
	}
}

TransferObjectParams ReadTransferObjectParams(const Task& task)
{
	TransferObjectParams params;
	params.bagId = task.getParamOrDefault("ContainerID", 0);
	return params;
}

void ApplyTransferObjectParams(Task& task, const TransferObjectParams& params)
{
	ApplySolarSystemHandlerType(task, SolarSystemHandlerType::TransferObject);
	(*task.paramsPtr)["ContainerID"] = params.bagId;
}

std::optional<EquipSwitchParams> TryReadEquipSwitchParams(const Task& task)
{
	if (ReadEntityTaskType(task) != EntityTaskType::Equipments)
		return std::nullopt;
	if (task.getParamOrDefault<std::string>(kEquipmentTaskType, "") != "switch")
		return std::nullopt;
	try {
		EquipSwitchParams params;
		params.slotType = std::any_cast<std::string>((*task.paramsPtr)["slotType"]);
		params.slotIndex = std::any_cast<int>((*task.paramsPtr)["slotIndex"]);
		params.targetObjectID = task.getParamOrDefault("targetObjectID", -1);
		return params;
	}
	catch (const std::bad_any_cast&) {
		return std::nullopt;
	}
}

void ApplyEquipSwitchParams(Task& task, const EquipSwitchParams& params)
{
	ApplyEntityTaskType(task, EntityTaskType::Equipments);
	(*task.paramsPtr)[kEquipmentTaskType] = std::string("switch");
	(*task.paramsPtr)["slotType"] = params.slotType;
	(*task.paramsPtr)["slotIndex"] = params.slotIndex;
	(*task.paramsPtr)["targetObjectID"] = params.targetObjectID;
}

EquipInstallParams ReadEquipInstallParams(const Task& task)
{
	EquipInstallParams params;
	params.objectID = task.getParamOrDefault("objectID", -1);
	params.groupID = task.getParamOrDefault("groupID", -1);
	return params;
}

void ApplyEquipInstallParams(Task& task, const EquipInstallParams& params)
{
	ApplyEntityTaskType(task, EntityTaskType::Equipments);
	(*task.paramsPtr)[kEquipmentTaskType] = std::string("installEquipment");
	(*task.paramsPtr)["objectID"] = params.objectID;
	(*task.paramsPtr)["groupID"] = params.groupID;
}

std::optional<CargoStorageRequestParams> TryReadCargoStorageRequestParams(const Task& task)
{
	if (ReadEntityTaskType(task) != EntityTaskType::CargoStorage)
		return std::nullopt;
	if (task.getParamOrDefault<std::string>(kStorageTaskType, "") != "RequestObject")
		return std::nullopt;
	try {
		CargoStorageRequestParams params;
		params.requestTarget = std::any_cast<int>((*task.paramsPtr)["RequestTarget"]);
		params.volume = std::any_cast<double>((*task.paramsPtr)["volume"]);
		return params;
	}
	catch (const std::bad_any_cast&) {
		return std::nullopt;
	}
}

void ApplyCargoStorageRequestParams(Task& task, const CargoStorageRequestParams& params)
{
	ApplyEntityTaskType(task, EntityTaskType::CargoStorage);
	(*task.paramsPtr)[kStorageTaskType] = std::string("RequestObject");
	(*task.paramsPtr)["volume"] = params.volume;
	(*task.paramsPtr)["RequestTarget"] = params.requestTarget;
}

void ApplyCargoStorageRefreshParams(Task& task)
{
	ApplyEntityTaskType(task, EntityTaskType::CargoStorage);
	(*task.paramsPtr)[kStorageTaskType] = std::string("Refresh");
}

std::optional<AddObjectParams> TryReadAddObjectParams(const Task& task)
{
	if (ReadEntityTaskType(task) != EntityTaskType::AddObject)
		return std::nullopt;
	AddObjectParams params;
	params.addType = task.getParamOrDefault<std::string>("addType", "");
	params.addTargetId = task.getParamOrDefault("addTargetId", -1);
	params.bagId = task.getParamOrDefault("containerID", -1);
	params.volume = task.getParamOrDefault("volume", 0.0);
	return params;
}

void ApplyAddObjectParams(Task& task, const AddObjectParams& params)
{
	ApplyEntityTaskType(task, EntityTaskType::AddObject);
	(*task.paramsPtr)["addType"] = params.addType;
	(*task.paramsPtr)["addTargetId"] = params.addTargetId;
	(*task.paramsPtr)["containerID"] = params.bagId;
	(*task.paramsPtr)["volume"] = params.volume;
}

std::optional<LockDirectionParams> TryReadLockDirectionParams(const Task& task)
{
	try {
		LockDirectionParams params;
		params.direction = std::any_cast<int>((*task.paramsPtr)["direction"]);
		return params;
	}
	catch (const std::bad_any_cast&) {
		return std::nullopt;
	}
}

void ApplyLockDirectionParams(Task& task, const LockDirectionParams& params)
{
	ApplyEntityTaskType(task, EntityTaskType::CargoStorage);
	(*task.paramsPtr)["direction"] = params.direction;
}

SkillAddToQueueParams ReadSkillAddToQueueParams(const Task& task)
{
	SkillAddToQueueParams params;
	params.skillTypeId = task.getParamOrDefault("skillTypeId", 0);
	return params;
}

void ApplySkillAddToQueueParams(Task& task, const SkillAddToQueueParams& params)
{
	ApplyEntityTaskType(task, EntityTaskType::SkillComponent);
	(*task.paramsPtr)[kSkillTaskType] = std::string("AddToActiveQueue");
	(*task.paramsPtr)["skillTypeId"] = params.skillTypeId;
}

SkillRemoveFromQueueParams ReadSkillRemoveFromQueueParams(const Task& task)
{
	SkillRemoveFromQueueParams params;
	params.skillTypeId = task.getParamOrDefault("skillTypeId", 0);
	params.skillLevel = task.getParamOrDefault("skillLevel", 0);
	return params;
}

void ApplySkillRemoveFromQueueParams(Task& task, const SkillRemoveFromQueueParams& params)
{
	ApplyEntityTaskType(task, EntityTaskType::SkillComponent);
	(*task.paramsPtr)[kSkillTaskType] = std::string("RemoveFromActiveQueue");
	(*task.paramsPtr)["skillTypeId"] = params.skillTypeId;
	(*task.paramsPtr)["skillLevel"] = params.skillLevel;
}

void ApplySkillRefreshParams(Task& task)
{
	ApplyEntityTaskType(task, EntityTaskType::SkillComponent);
	(*task.paramsPtr)[kSkillTaskType] = std::string("Refresh");
}

TargetObjectIdParams ReadTargetObjectIdParams(const Task& task)
{
	TargetObjectIdParams params;
	params.targetObjectId = task.getParamOrDefault("TargetObjectId", -1);
	return params;
}

void ApplyTargetObjectIdParams(Task& task, const TargetObjectIdParams& params)
{
	(*task.paramsPtr)["TargetObjectId"] = params.targetObjectId;
}

void ApplyUiWindowDragParams(Task& task, const UiWindowDragParams& params)
{
	(*task.paramsPtr)["x"] = params.x;
	(*task.paramsPtr)["y"] = params.y;
	(*task.paramsPtr)["ItemDrag"] = 1;
	(*task.paramsPtr)["objectID"] = params.objectID;
}

namespace TaskFactory {

std::shared_ptr<Task> MakeInnerEntityTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target,
	const std::string& taskType)
{
	auto task = std::make_shared<Task>();
	task->isInnerTask = true;
	task->taskID = -1;
	task->publisher = publisher;
	task->target = target;
	ApplyEntityTaskType(*task, taskType);
	return task;
}

std::shared_ptr<Task> MakeEquipSwitchTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target,
	const EquipSwitchParams& params,
	int taskTypeId)
{
	auto task = std::make_shared<Task>();
	task->isInnerTask = true;
	task->taskID = -1;
	task->taskTypeId = taskTypeId;
	task->publisher = publisher;
	task->target = target;
	ApplyEquipSwitchParams(*task, params);
	return task;
}

std::shared_ptr<Task> MakeEquipInstallTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target,
	const EquipInstallParams& params)
{
	auto task = std::make_shared<Task>();
	task->isInnerTask = true;
	task->publisher = publisher;
	task->target = target;
	ApplyEquipInstallParams(*task, params);
	return task;
}

std::shared_ptr<Task> MakeTransferObjectTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target,
	int bagId)
{
	auto task = std::make_shared<Task>();
	task->isInnerTask = true;
	task->publisher = publisher;
	task->target = target;
	task->targetSystem = SOLAR_SYSTEM;
	ApplyTransferObjectParams(*task, TransferObjectParams{ bagId });
	return task;
}

std::shared_ptr<Task> MakeCreateObjectTask(
	const std::shared_ptr<GameObject>& publisher,
	const CreateObjectParams& params)
{
	auto task = std::make_shared<Task>();
	task->targetSystem = SOLAR_SYSTEM;
	task->publisher = publisher;
	ApplyCreateObjectParams(*task, params);
	return task;
}

std::shared_ptr<Task> MakeDestroyObjectTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target)
{
	auto task = std::make_shared<Task>();
	task->publisher = publisher;
	task->target = target;
	task->targetSystem = SOLAR_SYSTEM;
	ApplySolarSystemHandlerType(*task, SolarSystemHandlerType::DestroyObject);
	return task;
}

std::shared_ptr<Task> MakeCargoStorageRequestTask(
	int publisherId,
	int targetId,
	const CargoStorageRequestParams& params)
{
	auto task = std::make_shared<Task>();
	task->publisherId = publisherId;
	task->targetId = targetId;
	ApplyCargoStorageRequestParams(*task, params);
	return task;
}

std::shared_ptr<Task> MakeCargoStorageRefreshTask(int objectID)
{
	auto task = std::make_shared<Task>();
	task->publisherId = objectID;
	task->targetId = objectID;
	ApplyCargoStorageRefreshParams(*task);
	return task;
}

std::shared_ptr<Task> MakeAddObjectTask(
	int publisherId,
	int targetId,
	const AddObjectParams& params)
{
	auto task = std::make_shared<Task>();
	task->publisherId = publisherId;
	task->targetId = targetId;
	ApplyAddObjectParams(*task, params);
	return task;
}

std::shared_ptr<Task> MakeLockDirectionTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target,
	int direction,
	int taskTypeId)
{
	auto task = std::make_shared<Task>();
	task->isInnerTask = true;
	task->taskID = -1;
	task->taskTypeId = taskTypeId;
	task->publisher = publisher;
	task->target = target;
	ApplyLockDirectionParams(*task, LockDirectionParams{ direction });
	return task;
}

std::shared_ptr<Task> MakeSkillAddToQueueTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target,
	int skillTypeId)
{
	auto task = std::make_shared<Task>();
	task->publisher = publisher;
	task->target = target;
	ApplySkillAddToQueueParams(*task, SkillAddToQueueParams{ skillTypeId });
	return task;
}

std::shared_ptr<Task> MakeSkillRemoveFromQueueTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target,
	int skillTypeId,
	int skillLevel)
{
	auto task = std::make_shared<Task>();
	task->publisher = publisher;
	task->target = target;
	ApplySkillRemoveFromQueueParams(*task, SkillRemoveFromQueueParams{ skillTypeId, skillLevel });
	return task;
}

std::shared_ptr<Task> MakeSkillRefreshTask(int objectID)
{
	auto task = std::make_shared<Task>();
	task->publisherId = objectID;
	task->targetId = objectID;
	ApplySkillRefreshParams(*task);
	return task;
}

std::shared_ptr<Task> MakeTargetObjectIdTask(
	int publisherId,
	const std::shared_ptr<GameObject>& target,
	int targetObjectId)
{
	auto task = std::make_shared<Task>();
	task->isInnerTask = true;
	task->taskID = 0;
	task->publisherId = publisherId;
	task->target = target;
	ApplyTargetObjectIdParams(*task, TargetObjectIdParams{ targetObjectId });
	return task;
}

std::shared_ptr<Task> MakeRefreshEquipmentTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target)
{
	return MakeInnerEntityTask(publisher, target, EntityTaskType::RefreshEquipment);
}

std::shared_ptr<Task> MakeRefiningObjectTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target)
{
	auto task = std::make_shared<Task>();
	task->publisher = publisher;
	task->target = target;
	task->targetSystem = REFINING;
	ApplySolarSystemHandlerType(*task, SolarSystemHandlerType::RefiningObject);
	return task;
}

std::shared_ptr<Task> MakeUndockTask(
	const std::shared_ptr<GameObject>& publisher,
	const std::shared_ptr<GameObject>& target)
{
	return MakeInnerEntityTask(publisher, target, EntityTaskType::Undock);
}

}
