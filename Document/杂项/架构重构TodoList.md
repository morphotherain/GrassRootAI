上级目录：[[文档]]

# 架构重构 TodoList

> 本文档记录 GrassRootAI 架构债务的**拆分任务清单**，与 [[ToDoList]]（功能开发）分开维护。
>
> **原则**：一个 PR / 一次提交只做一件事；每项有「完成标准」，做完可以停，不强行连锁大改。
>
> **当前主线**：Phase 2 ✅（2.5 可选）→ **Phase 7 🟡**（7.1 ✅）。**下一步**：7.4 依赖注入修复 或 7.2 去双轨指针。
>
> **预估工期**：Phase A 约 **3–5 天**；Phase B 约 **2–3 周**；Phase C 约 **1 周**；Phase 0–2 约 **1 周**；Phase 3–6 长期按需；**Phase 7** 与 Phase 2.4 衔接，分阶段做、不必一次换 EnTT。

---

## 进度总览

| Phase | 主题 | 预估 | 状态 |
|-------|------|------|------|
| **A** | **完善存档系统（补完半成品）** | 3–5 天 | ✅ 已完成（2026-06-29，已测） |
| **B** | **新 UI 框架（节点树 + 源文件加载）** | 2–3 周 | ✅ **已完成**（2026-06-29） |
| **C** | **主界面存档管理窗口** | 1 周 | ✅ **已完成**（2026-06-29，UIF 导航 + 存读删 UI） |
| 0 | 安全网 | 0.5 天 | 🟡 进行中 |
| 1 | P0 快速清理 | 1–2 天 | ✅ **已完成**（2026-06-29） |
| 2 | P1 边界抽取 | 3–5 天 | ✅ **已完成**（2026-06-29；2.5 可选未做） |
| 3 | P1 Task 系统加固 | 2–3 天 | ⬜ 未开始 |
| 4 | P2 旧 UI 解耦 | 按需，每项 1–2 天 | ⬜ 未开始 |
| 5 | P2 数据层 | 低优先级 | ⬜ 未开始 |
| 6 | 长期重构 | 1–2 周+ | ⬜ 未开始 |
| **7** | **Entity/Component 模型优化** | 2–4 周（渐进） | 🟡 进行中（7.1 ✅） |

> **背景**：commit `64448a8` 起的「半成品警示」已解除：存档状态机 + UIF 主菜单 + 新建/读档/删档 UI 闭环已通；A.4 临时 `UIButton` 回退已移除。

---

## Phase A — 完善存档系统（补完 `64448a8` 半成品）✅

> **完成日期**：2026-06-29。冒烟：UIF「开始新游戏」→ SpaceScene → F10 回主菜单；`saveSlots` slotID=5 等档与 `save/initial` 模板 dyn 表行数一致。

目标：`SaveGameManager` 已有 attach/detach、模板新建、按 slot 加载；补齐**游戏流程层**缺失的能力，使存档系统从「能写库」变成「能玩」。

**依赖**：无（可先用旧 UI 或键盘临时入口验证）。  
**不做什么**：不在此 Phase 做漂亮的主界面（交给 Phase C + 新 UI 框架）。

### A.1 盘点现状与缺口文档

- [x] 列出 `SaveGameManager` 已有 API vs 游戏流程实际调用的缺口
- [x] 记录当前半成品断点：MainScene 无入口、`StartNewGame()` 只走新建不走读档、回主菜单未 detach 等
- [x] 完成标准：`Document/` 下有一页「存档系统流程」说明（可与代码对照）→ [[存档系统流程]]
- [x] 预估：2h

### A.2 补全 SaveGameManager 缺失能力（按 API 单 PR）

- [x] **A.2a** `deleteSaveSlot(slotID)` — 软删（`isDeleted=1`）+ 可选物理文件归档
- [x] **A.2b** `updateLastPlayedTime(slotID)` — 加载/保存时更新 `lastPlayedTime`
- [x] **A.2c** `getSaveSlotInfo(slotID)` — 单槽查询（供 UI 显示详情）
- [ ] **A.2d**（可选）`saveCurrentGame()` — 明确「写回 dyn 库」语义（ATTACH 下写入即落盘，暂不需要单独 flush API）
- [x] 完成标准：每个 API 独立编译通过；用 sqlite 命令或日志可验证
- [x] 预估：每项 2–4h

### A.3 游戏状态机：`MainMenu` ↔ `InGame`

- [x] 定义 `GameApp` 状态转换规则：主菜单 / 游戏中 / 加载中
- [x] **进入游戏**：`loadSaveBySlotID` 或 `createNewSaveFromTemplate` → Init 各 Mgr → `InGame`
- [x] **返回主菜单**：detach 存档 → 清理 SolarSystemMgr / TaskMgr 运行时状态 → `MainMenu` → MainScene
- [x] 抽取 `EnterGameFromSlot(slotID)` / `ReturnToMainMenu()`，避免逻辑堆在 `StartNewGame()` 一处
- [x] 完成标准：能新建进游戏、能回主菜单、再新建不串档
- [x] 预估：1 天

### A.4 临时入口（旧 UI，已移除）

- [x] MainScene 接最小「开始新游戏」按钮（或 Enter 键），调用 `EnterGameFromSlot` / 新建流程
- [x] 修 `UIButton::Init()` 误 `return false`
- [x] Phase C 完成后删除 `UIF_USE_LEGACY_MAIN_MENU` / `UIButton` 回退路径
- [x] 完成标准：从 MainScene 能进 SpaceScene；作为 Phase B/C 前的冒烟基准
- [x] 预估：半天

### A.5 存档系统回归清单

- [x] 新建档 → 玩 → 回主菜单 → 再读档（Phase C UI 路径已测）
- [x] 连续新建两个档，slotID 不冲突（主库 slotID 2–5 已验证）
- [x] `save/initial` 模板缺失时有明确 ERROR 日志
- [x] 完成标准：手动路径可复现；写入 A.1 文档
- [x] 预估：2h

---

## Phase B — 新 UI 框架（节点树，取代难用的旧 UI）✅

> **状态**：骨架、编辑器、Atlas、DrawUIQuad 与 MainScene 集成已完成；Phase C 存档 UI 基于 `UINavigationStack` 扩展。

**依赖**：无硬性依赖 Phase A，但与 Phase C 强相关。  
**不做什么**：不在此 Phase 迁移现有游戏内窗口（Market、Skill 等）；旧 UI 继续跑，新框架先落地再逐步替换。

### B.1 框架骨架与目录约定

- [x] 新建模块目录、`UIRoot` / `UINode` 基类、`UINodeTree` 容器（`UISystem` + `UIDocument`）
- [x] 生命周期：`Load` → `Layout` → `Update(input)` → `Draw`（`Build/Measure/Arrange` + 每帧 Update/Draw）
- [x] 与旧 UI 共存策略写进文档（新 Scene 用新框架，旧 Scene 暂不动）→ [[UIF框架设计]]
- [x] 完成标准：空节点树可挂载到 MainScene 并绘制占位框
- [x] 预估：1–2 天

### B.2 节点类型（一次一种节点一个 PR）

- [x] **B.2a** `TextNode` — 文本渲染（复用 DWrite 或现有 `UIText` 底层，接口是新框架的）
- [x] **B.2b** `ButtonNode` — 点击区域 + hover/pressed 状态 + 回调
- [x] **B.2c** `SpriteNode` — 贴图/九宫格（DDS 路径或 atlas 区域）
- [x] **B.2d** `PanelNode` — 容器，支持子节点裁剪与布局
- [ ] **B.2e**（可选）`LayoutNode` — 水平/垂直/网格布局（v1 未做，Measure/Arrange pass-through）
- [x] 完成标准：每种节点有最小 demo 场景可交互
- [x] 预估：每种 0.5–1 天

### B.3 美术源文件格式与加载器

- [x] 定义窗口源文件格式（建议 JSON：节点树 + 属性 + 资源引用路径）
- [x] 示例源文件：`resource/ui/dev_main_menu.ui.json`（Phase C 可扩展为 `main_menu.ui.json`）
- [x] `UIDocumentLoader::LoadFromFile(path)` → 返回 `UIDocument` 实例树
- [x] 资源引用约定：贴图路径相对 `resource/`，字体/颜色/矩形用数据表达
- [x] 完成标准：不改 C++ 即可改 JSON 调整按钮位置与文案；加载后渲染正确
- [x] 预估：2–3 天

### B.4 游戏内源文件编辑器（不依赖美术素材）

- [x] 编辑器窗口：`UIEditorSession` 挂 MainScene，**F11** 切换
- [x] 用**点线面 + 文字**绘制：节点边框、选中高亮、属性面板、树状列表
- [x] 支持：选中节点、改 rect、保存 JSON（**Ctrl+S** 保存并重载）；改文本/贴图路径需直接编辑 JSON
- [x] 增删子节点：**Insert** 添加 Panel、**Delete** 删除选中节点（不可删 root）
- [x] **不要求自举**：编辑器 UI 纯 D2D 绘制，不通过 UIF 加载自身
- [x] 完成标准：能在游戏内打开 `dev_main_menu.ui.json`，改按钮坐标，保存后重载生效
- [x] 预估：1 周

### B.5 输入与坐标系

- [x] 统一 UI 坐标：左上原点、与窗口 client 区对齐
- [x] 命中测试：`HitTest(x,y)` 子节点逆序
- [x] 焦点/捕获：按钮按下未释放时捕获鼠标
- [x] 完成标准：多按钮重叠区域点击行为可预期
- [x] 预估：1–2 天

### B.6 框架文档

- [x] `Document/` 下「新 UI 框架」：节点类型表、源文件 schema、加载流程、与旧 UI 边界 → [[UIF框架设计]]
- [x] 补充「渲染职责边界」与低风险升级路线（见 [[UIF框架设计#渲染职责边界（与 Effect / D3DManager）]]）
- [x] 预估：2h

### B.7 图元层抽取（扩展 `UIRenderContext`，独立 PR）

- [x] `UIRenderContext::DrawUIQuad(texture, x, y, w, h)` — VB/绕序/Effect 缓存收进 Context
- [x] `UISpriteNode` 只调 `DrawUIQuad`，不再持有 Effect / VB 逻辑
- [x] `UIBase::GenerateRectVertex` 转调 `GenerateUIVertices`
- [x] 经 `D3DManager` + `GenerateUIVertices`；**不**暴露 `GenerateVertices` 给 UIF 节点
- [x] **不改**（已遵守）：SpaceScene / DockScene 3D 路径、`GenerateVertices` 语义
- [x] 完成标准：MainScene UIF 主菜单渲染正常；Graphics Diagnostics 中 UI Draw 的 PS 有输出
- [x] 预估：1–2 天

---

## Phase C — 主界面存档管理窗口 ✅

目标：在 MainScene 上用**新 UI 框架（Phase B）**做存档管理界面，打通：

```
MainScene → 存档列表 / 新建 / 删除 → 加载选定档 → 进 SpaceScene
                ↓
         返回主菜单（Phase A 状态机）
```

**依赖**：Phase A ✅；Phase B 至少 B.1–B.3 + B.2a/b + B.7 验收通过。

**不做什么**：不在此 Phase 做完整设置页、多 profile、云存档。

### C.1 主菜单窗口源文件

- [x] 用 B.3 格式编写 `resource/ui/main_menu.ui.json`（背景、标题、按钮区）
- [x] 按钮：新游戏、读取存档、快速开始、退出
- [x] 完成标准：MainScene 加载该文件即可显示主菜单骨架
- [x] 预估：半天

### C.2 存档列表子窗口

- [x] 源文件 `resource/ui/save_slot_list.ui.json`
- [x] 调用 `SaveGameManager::listSaveSlots()` 动态填充列表
- [x] 支持选中一条存档；空列表时显示「暂无存档」
- [x] 完成标准：数据来自真实 DB，非 mock
- [x] 预估：1 天

### C.3 新建 / 读取 / 删除 交互

- [x] **新建**：`save_create.ui.json` + 文本输入 → `createNewSaveFromTemplate` → `EnterGameFromSlot`
- [x] **读取**：选中 slot → `loadSaveBySlotID` → `EnterGameFromSlot`
- [x] **删除**：确认框 → `deleteSaveSlot` → 刷新列表
- [x] 完成标准：三条路径均从 UI 走通，不依赖键盘快捷键
- [x] 预估：1–2 天

### C.4 窗口包装与导航

- [x] 主菜单 ↔ 子窗口：`UINavigationStack` Push/Pop
- [x] 加载中状态：`SetBusy` + `loading_overlay`
- [x] Esc 返回上一级（根屏 Esc 退出）
- [x] 完成标准：导航不 leak 节点树；Esc 返回上一级
- [x] 预估：1 天

### C.5 与 GameApp 对接

- [x] MainScene 移除 A.4 临时按钮路径（默认 UIF-only）
- [x] `EnterGameFromSlot` / 新建 / 删档 由 `UIGameplayHost` 回调触发
- [x] 完成标准：Phase 0.2 冒烟路径「新建档 → 太空 → …」从主菜单 UI 启动
- [x] 预估：半天

### C.6 存档管理回归

- [x] 同 Phase A.5 路径，但全部走主界面 UI（新建 / 读档 / 删档 / F10 回主菜单）
- [x] 移除 A.4 `UIF_USE_LEGACY_MAIN_MENU` 回退代码
- [x] 完成标准：可交付的「主菜单 → 进游戏」闭环
- [x] 预估：2h

---

## Phase 0 — 安全网（改之前，0.5 天）

- [x] **0.1** 确认当前工程能编译运行
  - 完成标准：VS 编译通过，能进 SpaceScene / DockScene
  - 预估：30min

- [x] **0.2** 记录 3 条手动冒烟路径
  - 路径：新建档 → 太空 → 进站 → 出站 → 星门跳转，各走一遍
  - 完成标准：路径可复现，作为后续每项改动的回归基准
  - 预估：30min

- [ ] **0.3**（可选）给 TaskMgr / SolarSystemMgr 加 2–3 条日志锚点
  - 完成标准：场景切换、Task 分发时有 INFO 可追踪
  - 预估：1h

> 没有测试框架也可以；**手动路径 + 编译通过** 就是本项目的回归标准。

---

## Phase 1 — P0 快速清理（1–2 天，零行为变更）

每项独立，顺序可乱，但 **1.1 和 1.2 建议先做**。

### 1.1 Task 去渲染依赖

- [x] 改什么：`include/Task/Task.h` 去掉 `#include "D3DManager.h"`，改引 `logger_manager.h`
- [x] 不改什么：Task 字段、TaskMgr 逻辑
- [x] 完成标准：编译通过；Task 相关文件不再 include D3D
- [x] 预估：30min

### 1.2 修 Handler 自注册（ownership bug）

- [x] 改什么：
  - `SolarSystemHandler.h` / `RefiningSystemHandler.h` 构造器去掉 `unique_ptr(this)`
  - `HandlerFactory::initializeHandlers()` 里 `make_unique<XxxHandler>()` + 显式 `registerHandler(std::move(...))`
- [x] 不改什么：`handleTask` 实现
- [x] 完成标准：Handler 生命周期由 Mgr 持有；无构造自注册
- [x] 预估：2–3h

### 1.3 清理 DataSheet 多余 include（单文件单 PR）

- [x] 改什么：`dynGameObjectsManager.h`、`DatabaseManager.h` 去掉 `#include "d3dUtil.h"`
- [x] 策略：**一次只改 3–5 个文件**，编译验证后再下一批（其余 manager 本无 d3dUtil）
- [x] 完成标准：该批文件编译通过，无新增 warning
- [x] 预估：每批 30min，共 4–5 批

### 1.4 清理 Component 多余 include（单文件单 PR）

- [x] 改什么：11 个 Component 头去掉无必要的 `d3dUtil.h`（`RenderComponent` 保留）
- [x] 策略：同 1.3，按文件分批
- [x] 预估：2–3 批，每批 30min

### 1.5 去掉 vcxproj 里 `.h` 当 `.cpp` 编译

- [x] 改什么：`GrassRootAI(2019 Win10).vcxproj` 中 6 个误列 `ClCompile` 的 `.h` 移除（修复 LNK4042 重复符号）
- [x] 完成标准：仅 `.cpp` 参与编译；行为不变
- [x] 预估：1h

---

## Phase 2 — P1 边界抽取（3–5 天，行为不变）

### 2.1 定义 SceneId 枚举

- [x] 改什么：新建 `include/Scene/SceneId.h`，`enum class SceneId { Main, Dock, Space, StargateLoading }`
- [x] 替换：`GameApp` 里 `currentSceneID` 的 1/2/3/4 → `m_currentSceneId`
- [x] 不改什么：切换条件逻辑
- [x] 完成标准：无 magic number 1–4；编译 + 场景切换正常
- [x] 预估：1h

### 2.2 抽取 SceneTransitionService（只搬逻辑，不改规则）

- [x] 改什么：
  - 新建 `SceneTransitionService.h/.cpp`
  - 从 `GameApp::UpdateScene` **剪切** containerID / solarSystemID / `m_currentSceneId` 判断 → `EvaluateTransition(...)` 返回 `std::optional<SceneId>`
  - `GameApp` 只负责 `SwitchToSceneId(...)`
- [x] 不改什么：跳转条件本身
- [x] 完成标准：`GameApp::UpdateScene` 场景部分 ≤30 行；跳转行为与改前一致
- [x] 预估：半天

### 2.3 抽取星门加载中的 SolarSystem 切换逻辑

- [x] 改什么：星门跨系 `loadSolarSystem` / `setCurrentPilot` / `clearCurrentPilots` 块 → `SolarSystemMgr::switchToSolarSystem(id)`
- [x] 不改什么：load 实现细节
- [x] 完成标准：`SceneTransitionService` 不再直接操作多个 Mgr 字段
- [x] 预估：2–3h

### 2.4 抽取 ObjectFactory（只搬 switch）

- [x] 改什么：
  - 新建 `ObjectFactory.h/.cpp`
  - `SolarSystem::addGameObject` 的 categoryID switch → `ObjectFactory::CreateFromDynObject(...)`
- [x] 不改什么：`ConvertBasedOnGroupID`、`Init()` 调用顺序
- [x] 完成标准：`SolarSystem::addGameObject` ≤20 行；新类型只改 Factory
- [x] 预估：半天
- [x] **衔接 Phase 7.1**：Factory 是 Archetype 组装的前置，先搬 switch 再填配方表

### 2.5 Ship::ConvertBasedOnGroupID 迁入 Factory（可选，独立 PR）

- [ ] 改什么：groupID 二次转换逻辑并入 Factory 或 `ObjectFactory::RefineByGroupID`
- [ ] 完成标准：`SolarSystem` / `Ship` 不再含 category/group switch
- [ ] 预估：2–3h
- [ ] **衔接 Phase 7.1b**

### 2.6 SolarSystem 内联 SQL 迁到 Manager

- [x] 改什么：`SolarSystem::getDenormalizesBySolarSystemID()` 的 SQL → `mapDenormalizeManager::queryStaticObjectsBySolarSystemID`
- [x] 不改什么：返回数据结构、`addGameObject` 流程
- [x] 完成标准：`SolarSystem.cpp` 无 raw SQL
- [x] 预估：1–2h

---

## Phase 3 — P1 Task 系统加固（2–3 天，增量）

详见 [[任务系统]]。

### 3.1 为常用 Task 定义参数 struct（一次一种 taskType）

- [ ] 改什么：如 `EquipTaskParams`、`TransferObjectParams`，加 helper 构造/读取
- [ ] 策略：**只覆盖正在改的功能用到的 taskType**，不一次改全
- [ ] 完成标准：该 taskType 不再裸用 `(*paramsPtr)["xxx"]`
- [ ] 预估：每种 1–2h

### 3.2 Task 创建 helper（单 SystemType 一个 PR）

- [ ] 改什么：如 `TaskBuilder::ForSolarSystem("createObject").withTarget(id)...`
- [ ] 完成标准：新代码用 helper；旧代码可暂留
- [ ] 预估：每种 1h

### 3.3 文档对齐：补 Task 参数字段表

- [ ] 改什么：`Document/任务/` 下为已有 handler 补 params 表
- [ ] 完成标准：与代码一致；新功能先更文档再写代码
- [ ] 预估：持续，每项 30min

### 3.4 Entity Task 统一 dispatch（与 Phase 7.7 联动）

- [ ] 改什么：`GameObject::dispatchTask(task)` 统一入口 → 按 `taskType` 路由到 Component / `TaskHandlerRegistry`
- [ ] 不改什么：各 Component 的 `handleTask` 实现
- [ ] 完成标准：逐步删掉 `Ship::initTaskHandlers` / `Pilot::initTaskHandlers` 里重复 lambda
- [ ] 预估：1–2 天（与 7.7 可合并 PR，按 Entity 类型分批）

## Phase 4 — P2 UI 解耦（按窗口拆，每个 1–2 天）

**禁止**「重构全部 UI」这种大任务；按文件、按职责拆。

### 4.1 UIBase 去 SolarSystemMgr 依赖

- [ ] 改什么：`UIBase.h` 去掉 `#include "SolarSystemMgr.h"`
- [ ] 策略：需要 Mgr 的子类在 `.cpp` include，或通过 `ParseParameters` 注入 weak_ptr
- [ ] 完成标准：`UIBase.h` 无 simulation 依赖；编译通过
- [ ] 预估：2–3h（可能连带改 5–10 个 `.cpp`）

### 4.2 UIShip：只抽 Input → Task（不动 DrawUI）

- [ ] 改什么：新建 `ShipInputController.cpp`，搬 `UpdateUI` 里键盘/锁定/装备相关 Task 创建
- [ ] 不改什么：`DrawUI`、渲染、D3D 资源
- [ ] 完成标准：`UIShip::UpdateUI` 委托给 Controller；行为不变
- [ ] 预估：半天

### 4.3 UIWindowStorage：抽 StorageTaskHelper

- [ ] 改什么：drag/drop → Task 的逻辑进 helper
- [ ] 不改什么：窗口布局、DrawUI
- [ ] 预估：半天

### 4.4 UIWindowSkill：DB 查询与 UI 分离（分两步）

- [ ] **4.4a** 抽 `SkillQueryService`（只搬 DB 读）
- [ ] **4.4b** UI 只调 Service + 发 Task
- [ ] 预估：各半天

### 4.5 UIWindowMarket / UIWindowMap

- [ ] 同 4.4 模式，**各自独立 PR**，不要合并

### 4.6 团队约定（零代码任务）

- [ ] 新 UI 功能：**只发 Task，禁止 `SolarSystemMgr::getInstance().xxx = ...`**
- [ ] Code review 检查项写进文档或 README 一节

---

## Phase 5 — P2 数据层（按需、低优先级）

### 5.1 抽象 DataSheet 基类（先做 1 个试点）

- [ ] 改什么：选最简单的 manager（如 `demoManager`）抽 `DataSheetBase<T>` CRUD
- [ ] 完成标准：试点 manager 代码量减少；模式文档化
- [ ] 预估：1 天

### 5.2 逐个迁移 manager（每个独立 PR）

- [ ] 策略：新表用新基类；旧表「碰到了再迁」
- [ ] **禁止**：一次性重写 23 个 manager

### 5.3 删除/归档空 stub

- [ ] 改什么：`taskDatabaseManager.cpp` 等空文件
- [ ] 预估：30min

---

## Phase 7 — Entity/Component 模型优化（渐进，非 EnTT 重写）

> **定位**：当前是 **OOP Entity + Component 混合体**，不是纯 ECS；目标是在不推翻 `GameObject` / `shared_ptr` 的前提下，收敛双轨指针、Update 编排、Task 路由与 DB 边界。
>
> **依赖**：Phase 2.4 / 2.5（ObjectFactory）先做；Phase 3 与 7.7 可并行。
>
> **不做什么**：不引入 EnTT / flecs；不一次性去掉所有 Entity 子类；不做 SOA 数据导向 ECS。

### 现状债务（评估摘要，2026-06-29）

| 问题 | 现状 | 风险 |
|------|------|------|
| 双轨组件引用 | `Ship` 等既 `AddComponent` 又持 `m_pBase` 等成员；`PhysicsComponent` 手工连 `SpaceTran` | 改一处漏一处 |
| `GetComponent` | `vector` + `dynamic_cast` 线性扫描 | 对象/调用增多后性能与类型安全差 |
| `ResolveDependencies` | 用 `typeid(...).hash_code()` 比对 | 理论上不安全；`InjectDependency` cast 链脆弱 |
| Update 编排 | 基类遍历 Component vs `Ship::Update` 手工顺序 | 行为分散，新类型复制粘贴 |
| 持久化 | `BaseComponent::Update` 每 tick `store()` | SQLite 写放大 |
| Task 路由 | TaskMgr Handler + Entity `taskHandlers` + Component `handleTask` 三套 | 调试难、重复 lambda |
| DB 耦合 | Component 构造/Update 直连 `dynGameObjectsManager` 等 | 难单测、难纯内存模拟 |
| Pilot 建模 | `Pilot` 继承 `GameObject` | 控制器与场景实体混淆 |

### 7.0（可选）现状与目标架构一页文档

- [ ] 改什么：`Document/` 下「Entity/Component 模型」：现状图、Archetype 表、System 列表、与 Phase 2.4 Factory 关系
- [ ] 完成标准：新人能对照 `Ship::Init` / `SolarSystem::addGameObject` 理解改法
- [ ] 预估：2h

### 7.1 Archetype 配方 + ObjectFactory 组装

- [x] **7.1a** 定义 `EntityArchetype` 枚举：`AstroCelestial`、`StarGate`、`Ship`、`Equipment` 等
- [x] **7.1b** `EntityComponentAssembler` 按 Archetype 挂载组件清单
- [x] **7.1c** `ObjectFactory` 在 `Init()` 前调用 Assembler；各子类 `Init()` 只做引用绑定与业务逻辑
- [x] `GameObject::GetComponentShared<T>()` 供 Init 绑定成员指针
- [ ] 依赖：Phase **2.4** ✅、**2.5**（group 细化类型仍由 `ConvertBasedOnGroupID`）
- [x] 完成标准：组件 `AddComponent` 集中在 Assembler；新增类型改 Assembler + Archetype 解析
- [x] 预估：1–2 天

### 7.2 去掉 Entity 双轨组件指针

- [ ] 改什么：`Ship` / `Pilot` / `Equipment` 等删除 `m_pBase`、`m_pAttributes`… 成员（或 Init 后仅保留只读缓存 struct，禁止双路径创建）
- [ ] 改什么：`PhysicsComponent` 改走 `ResolveDependencies` 注入 `SpaceTransformComponent`，去掉 `m_pPhysics->SpaceTran = …` 手工赋值
- [ ] 不改什么：对外行为、组件集合
- [ ] 完成标准：组件只存在于 `GameObject` 的 `components` 袋；grep 无 `make_shared<XComponent>` 后又 `AddComponent` 以外的重复持有（Init 缓存除外）
- [ ] 预估：1–2 天（按 Entity 类型分批 PR）

### 7.3 组件袋：`GetComponent` O(1) 或 Init 缓存

- [ ] 改什么：`GameObject` 增加 `unordered_map<type_index, shared_ptr<Component>>` 或 Archetype 已知时的 `ComponentBundle` 成员
- [ ] 不改什么：现有 `GetComponent<T>()` 对外签名（内部换实现）
- [ ] 完成标准：热路径不再每帧 `dynamic_cast` 扫描；编译通过
- [ ] 预估：半天–1 天

### 7.4 修复 `ResolveDependencies`

- [ ] 改什么：`typeid(*depComponent) == depType`（或 `type_index` 直接比较），禁止 `hash_code`
- [ ] 改什么：`InjectDependency` 可改为按 `type_index` 分发表，减少 `dynamic_pointer_cast` 链
- [ ] 完成标准：`EquipmentsComponent` 等依赖注入单测/日志可验证；无 hash 比对
- [ ] 预估：2–3h

### 7.5 轻量 System 层（非完整 ECS）

- [ ] 改什么：从 `Ship::Update` / `SolarSystem::Update` 抽出按职责的 System，例如：
  - `MovementSystem`（Physics + SpaceTransform）
  - `AttributeDirtyFlushSystem`（见 7.6）
  - `SectorSpatialSystem`（已有 sector 逻辑收拢）
- [ ] 不改什么：单帧内调用顺序与改前一致
- [ ] 完成标准：`SolarSystem::Update` 编排 System 列表；`Ship::Update` 只保留跃迁等实体特有状态机
- [ ] 预估：2–3 天

### 7.6 持久化：脏标记 + 批量 flush

- [ ] 改什么：`BaseComponent` / `AttributesComponent` 改内存 + `MarkDirty(objectID, fieldMask)`，去掉每 tick `store()`
- [ ] 改什么：`AttributeSyncSystem` 或 `SaveGameManager` 侧在 F10 回菜单 / 每 N tick / `OnDestroy` 批量写 dyn
- [ ] 完成标准：`BaseComponent::Update` 不再每帧 hit SQLite；回主菜单后 dyn 数据仍正确
- [ ] 预估：1–2 天

### 7.7 Task 统一 dispatch（与 Phase 3.4 联动）

- [ ] 改什么：`GameObject::dispatchTask` → `ComponentTaskRouter` / 各 Component `TaskHandlerRegistry`
- [ ] 改什么：删掉 `Ship::initTaskHandlers`、`Pilot::initTaskHandlers` 重复 map（按 Entity 类型分批）
- [ ] 不改什么：TaskMgr 全局 Handler（create/transfer/destroy）与 Refining 路径
- [ ] 完成标准：Entity 内 Task 只有一条入口；新 taskType 只注册一处
- [ ] 预估：1–2 天

### 7.8 Component 与 DataSheet 边界

- [ ] 改什么：抽 `IGameObjectRepository` / `IAttributeRepository` 接口；Component 只调接口，Manager 在 Sim 层注入
- [ ] 策略：先做 `BaseComponent` + `AttributesComponent` 试点，再按需扩
- [ ] 完成标准：Component `.h` 不再 include 具体 `dyn*Manager.h`（试点范围内）
- [ ] 预估：2–3 天

### 7.9（可选）Pilot 从 GameObject 拆为 PlayerController

- [ ] 改什么：`Pilot` 不再继承 `GameObject`；作为「控制 currentShip 的会话对象」由 `SolarSystemMgr` 持有
- [ ] 完成标准：`Pilot` 无 `AddComponent`；技能/任务仍可用
- [ ] 预估：2–3 天（独立 PR，优先级低）

### 7.10 回归清单

- [ ] 新建档 → 太空 → 锁定/跃迁/装备 → 进站出站 → 星门跳转
- [ ] 删/创建对象 Task（create/transfer/destroy）仍正常
- [ ] F10 回主菜单再读档，dyn 对象/属性一致

---

## Phase 6 — 长期（整块做，别半吊子）

| # | 任务 | 为何单独成 Phase | 预估 |
|---|------|------------------|------|
| 6.1 | CMake target 分层（Platform / Core / Data / Sim / UI） | 要配合 include 禁令，工作量大 | 1–2 周 |
| 6.2 | Singleton → 构造注入（从 GameApp 往下传） | 动全局 wiring | 2+ 周 |
| 6.3 | Entity/Component 模型收敛 | 动核心对象模型；**细则见 Phase 7** | 2–4 周（渐进） |
| 6.4 | Task 参数全面 typed 化 | 依赖 Phase 3 逐步覆盖 | 持续 |

> Phase 6 **等 Phase 1–2 做完再开**；否则边改边界边改全局 wiring，容易失控。

---

## 推荐执行顺序

### 主线（产品闭环，优先）

```
Phase A ✅
  ↓
Phase B ✅
  ↓
Phase C ✅
  ↓
Phase 1 ✅（Task 去 D3D、Handler ownership、include 清理、vcxproj 修 .h 误编译）
  ↓
Phase 2 🟡（2.5 可选）
  ↓
Phase 7.1（Archetype 配方，基于 ObjectFactory）
```

### 架构债务（与主线并行或后置）

```
Phase 0（冒烟，部分已完成）
  ↓
1.1 → 1.2 → 1.3（分批）
  ↓
2.1 → 2.2 → 2.3 → 2.4 → 2.6
  ↓
Phase 7.1–7.4（Factory + 双轨指针 + GetComponent + 依赖注入）
  ↓
Phase 7.5–7.8（System 层、脏标记、Task dispatch、DB 边界）
  ↓
（开发新功能时）3.1；旧 UI 触碰时 Phase 4 按需
  ↓
Phase 5–6 有空再说；7.9 Pilot 拆分按需
```

> **注意**：Phase 4（旧 UI 解耦）在 Phase B 启动后优先级降低——新界面用新框架，旧窗口「碰到了再拆」。

---

## 每项 PR 自检清单

- [ ] 只动 1 个「主题」（如「Task 去 D3D」）
- [ ] 编译通过
- [ ] 手动冒烟 3 条路径至少走 1 条
- [ ] 无「顺便 refactor」的无关改动
- [ ] PR 描述写清：**改了什么 / 没改什么 / 怎么验证**

---

## 背景：模块良构评估摘要

| 良构（方向对） | 非良构（债务重） |
|----------------|------------------|
| Effect / EffectComponent | GameApp（场景 FSM + 每帧 orchestration） |
| GenericChain（属性链） | SolarSystemMgr / SolarSystem |
| SaveGameManager | UI 胖窗口（UIShip、Market、Skill、Map） |
| Camera | DataSheet 23 个 manager 重复 |
| Task 系统（概念层，见 [[任务系统]]） | Object + Component 混合体（见 **Phase 7** 债务表） |
| WindowManager 工厂思路 | 跨层 include（Task→D3D、UIBase→SolarSystemMgr） |

主要混乱来源：**没有编译期模块墙** + **UI / GameApp 当 God orchestrator**。

---

## 变更记录

| 日期 | 说明 |
|------|------|
| 2026-06-29 | 初版：按任务粒度拆分架构重构清单 |
| 2026-06-29 | 新增 Phase A/B/C：完善存档系统、新 UI 节点树框架、主界面存档管理；明确 `64448a8` 半成品背景 |
| 2026-06-29 | Phase A 标记完成（已测）；Phase B 更正为进行中（曾误标完成） |
| 2026-06-29 | Phase B 标记完成；Phase C 导航栈 + 存档 UI Screen 落地 |
| 2026-06-29 | Phase C 标记完成；C.6 回归清单写入 [[存档系统流程]]；移除 A.4 `UIF_USE_LEGACY_MAIN_MENU` 回退 |
| 2026-06-29 | Phase 1 完成：Task 去 D3D、Handler Factory 注册、Component/Database include 清理、vcxproj 去掉 .h 误编译 |
| 2026-06-29 | Phase 2.2–2.4、2.6：`SceneTransitionService`、`switchToSolarSystem`、`ObjectFactory`、mapDenormalize SQL 迁移 |
| 2026-06-29 | Phase 7.1：`EntityArchetype` + `EntityComponentAssembler` 集中组件组装 |
| 2026-06-29 | 新增 **Phase 7** Entity/Component 模型优化清单（Archetype、System 层、脏标记、Task dispatch 等） |
