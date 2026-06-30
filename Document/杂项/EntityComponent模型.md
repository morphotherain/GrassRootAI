上级目录：[[文档]]

# Entity/Component 模型（Phase 7 目标架构）

> 与 [[架构重构TodoList#Phase 7 — Entity/Component 模型优化（渐进，非 EnTT 重写）]] 对照阅读。
>
> 重构**用意与全局约定**见 [[架构重构指南]]。

## 现状定位

GrassRootAI 使用 **OOP Entity + Component 混合体**，不是 EnTT 式纯 ECS：

- **Entity**：`GameObject` 子类（`Ship`、`Pilot`、`StarGate`…）仍承载少量实体特有状态机（如跃迁 `currentWarpState`）
- **Component**：数据 + 部分逻辑（`PhysicsComponent`、`BaseComponent`…）
- **System**：轻量模块（`EntityUpdateSystem`、`SectorSpatialSystem`），由 `SolarSystem::Update` 编排

## 对象创建流程

```
dynGameObject (DB)
  → ObjectFactory::CreateFromDynObject
  → EntityComponentAssembler::Assemble (按 EntityArchetype 挂组件)
  → 子类 Init() (业务绑定 + initEntityTaskHandlers + ResolveDependencies)
  → SolarSystem::addObjectToSector (有 SpaceTransform 时)
```

新增实体类型：改 `EntityArchetype` + `EntityComponentAssembler` + `ObjectFactory` 解析，不必在每个子类重复 `AddComponent`。

## System 列表（SolarSystem 层）

| System | 职责 | 状态 |
|--------|------|------|
| `EntityUpdateSystem` | 清理 expired weak_ptr、遍历 `space_objects` 调 `GameObject::Update` | ✅ |
| `SectorSpatialSystem` | sector 网格、跨 sector 迁移、currentSector | ✅ |

`SolarSystem::Update(tick)` 顺序：

1. `EntityUpdateSystem::Update`
2. `SectorSpatialSystem::Update`（每 30 tick 做 sector 检查）

## Task 路由（Entity 层）

```
TaskMgr → SolarSystemMgr::handleTask
  → target->handleTask(task)
    → GameObject::dispatchTask
      → DispatchEntityTask(entityTaskHandlers, task)
```

各 Entity 通过 override `registerEntityTaskHandlers` 注册 taskType → lambda，handler map 由 `GameObject` 基类持有（`unique_ptr` 堆分配）。

全局 Handler（create/transfer/destroy、Refining）仍在 TaskMgr / SolarSystemMgr，不在 Entity 层。

## 持久化

- Component 内 `needStore` / `shouldUpdate` 脏标记
- 关键字段变更（`setContainerID`、`setSolarSystemID`）即时落库
- `SolarSystemMgr::Shutdown` 全量 flush

`BaseComponent` 通过 `IGameObjectRepository` 访问 dyn 数据（试点），默认实现 `DynGameObjectRepository` 包装 `dynGameObjectsManager`。

## 依赖注入

`GameObject::ResolveDependencies` 用 `std::type_index` 匹配依赖类型。

组件侧可用 `ComponentDependencyBinder` 注册 `Bind` / `BindShared`，减少 `InjectDependency` 里手写 cast 链（已在 `PhysicsComponent`、`LockingComponent` 试点）。

## 与 ECS 的距离

| ECS 概念 | 本项目对应 |
|----------|------------|
| Archetype | `EntityArchetype` + Assembler |
| Component 存储 | `GameObject::componentByType` (AoS, shared_ptr) |
| System | `EntityUpdateSystem`、`SectorSpatialSystem` |
| Entity ID | 仍用 `GameObject` 子类 + `objectID` |

**7.9 Pilot → PlayerController** 留作 Phase 6 衔接：需改 `SolarSystemMgr`、全部 UI 对 `currentPilot->currentShip` 的引用，独立 PR。

## Phase 7 回归清单

- [ ] 新建档 → 太空 → 锁定/跃迁/装备
- [ ] 进站 / 出站（NPCStation dock/undock Task）
- [ ] 星门跳转
- [ ] create / transfer / destroy Task
- [ ] F10 回主菜单 → 再读档，dyn 对象/属性一致
