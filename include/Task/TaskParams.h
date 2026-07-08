#pragma once

#include "Task.h"
#include "Database/DataSheet/dogmaAttributesManager.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

class GameObject;

namespace EntityTaskType {
	inline constexpr const char* SetApproachTarget = "setApproachTarget";
	inline constexpr const char* SetWarpTarget = "setWarpTarget";
	inline constexpr const char* AddLocked = "addLocked";
	inline constexpr const char* EraseLocked = "eraseLocked";
	inline constexpr const char* Jump = "jump";
	inline constexpr const char* Undock = "undock";
	inline constexpr const char* RefreshEquipment = "refreshEquipment";
	inline constexpr const char* Equipments = "equipments";
	inline constexpr const char* CargoStorage = "cargoStorage";
	inline constexpr const char* SkillComponent = "skillComponent";
	inline constexpr const char* AddObject = "addObject";
}

namespace SolarSystemHandlerType {
	inline constexpr const char* CreateObject = "createObject";
	inline constexpr const char* TransferObject = "transferObject";
	inline constexpr const char* DestroyObject = "destroyObject";
	inline constexpr const char* RefiningObject = "refiningObject";
}

struct CreateObjectParams {
	int typeID = 0;
	int ownerID = 0;
	int bagId = 0;
	int solarSystemID = 0;
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	double qw = 0.0;
	double qx = 0.0;
	double qy = 0.0;
	double qz = 0.0;
	std::optional<std::vector<Attribute>> attributes;
};

CreateObjectParams ReadCreateObjectParams(const Task& task);
void ApplyCreateObjectParams(Task& task, const CreateObjectParams& params);

struct TransferObjectParams {
	int bagId = 0;
};

TransferObjectParams ReadTransferObjectParams(const Task& task);
void ApplyTransferObjectParams(Task& task, const TransferObjectParams& params);

struct EquipSwitchParams {
	std::string slotType;
	int slotIndex = 0;
	int targetObjectID = -1;
};

std::optional<EquipSwitchParams> TryReadEquipSwitchParams(const Task& task);
void ApplyEquipSwitchParams(Task& task, const EquipSwitchParams& params);

struct EquipInstallParams {
	int objectID = -1;
	int groupID = -1;
};

EquipInstallParams ReadEquipInstallParams(const Task& task);
void ApplyEquipInstallParams(Task& task, const EquipInstallParams& params);

struct CargoStorageRequestParams {
	int requestTarget = -1;
	double volume = 0.0;
};

std::optional<CargoStorageRequestParams> TryReadCargoStorageRequestParams(const Task& task);
void ApplyCargoStorageRequestParams(Task& task, const CargoStorageRequestParams& params);
void ApplyCargoStorageRefreshParams(Task& task);

struct AddObjectParams {
	std::string addType;
	int addTargetId = -1;
	int bagId = -1;
	double volume = 0.0;
};

std::optional<AddObjectParams> TryReadAddObjectParams(const Task& task);
void ApplyAddObjectParams(Task& task, const AddObjectParams& params);

struct LockDirectionParams {
	int direction = 0;
};

std::optional<LockDirectionParams> TryReadLockDirectionParams(const Task& task);
void ApplyLockDirectionParams(Task& task, const LockDirectionParams& params);

struct SkillAddToQueueParams {
	int skillTypeId = 0;
};

SkillAddToQueueParams ReadSkillAddToQueueParams(const Task& task);
void ApplySkillAddToQueueParams(Task& task, const SkillAddToQueueParams& params);

struct SkillRemoveFromQueueParams {
	int skillTypeId = 0;
	int skillLevel = 0;
};

SkillRemoveFromQueueParams ReadSkillRemoveFromQueueParams(const Task& task);
void ApplySkillRemoveFromQueueParams(Task& task, const SkillRemoveFromQueueParams& params);
void ApplySkillRefreshParams(Task& task);

struct TargetObjectIdParams {
	int targetObjectId = -1;
};

TargetObjectIdParams ReadTargetObjectIdParams(const Task& task);
void ApplyTargetObjectIdParams(Task& task, const TargetObjectIdParams& params);

struct UiWindowDragParams {
	int x = 0;
	int y = 0;
	int objectID = -1;
};

void ApplyUiWindowDragParams(Task& task, const UiWindowDragParams& params);

void ApplyEntityTaskType(Task& task, const std::string& taskType);
std::string ReadEntityTaskType(const Task& task);
void ApplySolarSystemHandlerType(Task& task, const std::string& handlerType);

namespace TaskFactory {
	std::shared_ptr<Task> MakeInnerEntityTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target,
		const std::string& taskType);

	std::shared_ptr<Task> MakeEquipSwitchTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target,
		const EquipSwitchParams& params,
		int taskTypeId = 5);

	std::shared_ptr<Task> MakeEquipInstallTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target,
		const EquipInstallParams& params);

	std::shared_ptr<Task> MakeTransferObjectTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target,
		int bagId);

	std::shared_ptr<Task> MakeCreateObjectTask(
		const std::shared_ptr<GameObject>& publisher,
		const CreateObjectParams& params);

	std::shared_ptr<Task> MakeDestroyObjectTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target);

	std::shared_ptr<Task> MakeCargoStorageRequestTask(
		int publisherId,
		int targetId,
		const CargoStorageRequestParams& params);

	std::shared_ptr<Task> MakeCargoStorageRefreshTask(int objectID);

	std::shared_ptr<Task> MakeAddObjectTask(
		int publisherId,
		int targetId,
		const AddObjectParams& params);

	std::shared_ptr<Task> MakeLockDirectionTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target,
		int direction,
		int taskTypeId = 6);

	std::shared_ptr<Task> MakeSkillAddToQueueTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target,
		int skillTypeId);

	std::shared_ptr<Task> MakeSkillRemoveFromQueueTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target,
		int skillTypeId,
		int skillLevel);

	std::shared_ptr<Task> MakeSkillRefreshTask(int objectID);

	std::shared_ptr<Task> MakeTargetObjectIdTask(
		int publisherId,
		const std::shared_ptr<GameObject>& target,
		int targetObjectId);

	std::shared_ptr<Task> MakeRefreshEquipmentTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target);

	std::shared_ptr<Task> MakeRefiningObjectTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target);

	std::shared_ptr<Task> MakeUndockTask(
		const std::shared_ptr<GameObject>& publisher,
		const std::shared_ptr<GameObject>& target);
}
