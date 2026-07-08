上级目录：[[任务系统]]

# Task 参数 typed 化（Phase 3）

运行时 Task 仍使用 `paramsPtr`（`unordered_map<string, any>`）序列化；**创建与读取**统一走 `include/Task/TaskParams.h`。

## 已覆盖的参数 struct

| struct | 路由键 | 用途 |
|--------|--------|------|
| `CreateObjectParams` | `handlerType=createObject` | SolarSystem 新建对象 |
| `TransferObjectParams` | `handlerType=transferObject` | 转移 container |
| `EquipSwitchParams` | `taskType=equipments` + `equipmentTaskType=switch` | 快捷键切换装备槽 |
| `EquipInstallParams` | `equipmentTaskType=installEquipment` | 拖拽安装装备 |
| `CargoStorageRequestParams` | `storageTaskType=RequestObject` | 采矿/请求物品 |
| `AddObjectParams` | `taskType=addObject` | 小行星堆叠/创建 |
| `LockDirectionParams` | `direction`（经 `cargoStorage` 路由） | 切换锁定目标 |
| `SkillAddToQueueParams` / `SkillRemoveFromQueueParams` | `skillTaskType` | 技能队列 |
| `TargetObjectIdParams` | `TargetObjectId` | 武器/矿头指向目标 |
| `UiWindowDragParams` | `ItemDrag` | UI 窗口拖拽 |

常量：`EntityTaskType::*`、`SolarSystemHandlerType::*`。

## 创建 Task

优先使用 `TaskFactory::`：

```cpp
// Entity：朝向、跃迁、锁定等（仅 taskType）
source->addTask(TaskFactory::MakeInnerEntityTask(src, tgt, EntityTaskType::SetWarpTarget));

// SolarSystem handler
TaskMgr::getInstance().addTask(TaskFactory::MakeCreateObjectTask(publisher, CreateObjectParams{ ... }));
TaskMgr::getInstance().addTask(TaskFactory::MakeTransferObjectTask(pub, tgt, containerID));
```

## 读取参数（Handler / Component）

```cpp
const auto params = ReadCreateObjectParams(task);
if (auto switchParams = TryReadEquipSwitchParams(task)) { ... }
```

## 扩展新 taskType

1. 在 `TaskParams.h` 增加 struct + `Read`/`Apply` 声明  
2. 在 `TaskParams.cpp` 实现（写入的 key 必须与旧代码一致）  
3. 可选：增加 `TaskFactory::Make...`  
4. 迁移该 taskType 的**创建点 + 消费点**各一处即可合 PR  
5. 更新 `Document/任务/` 对应 handler 文档
