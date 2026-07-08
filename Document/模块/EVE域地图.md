上级目录：[[文档]]

# EVE 玩法域地图

> **目的**：建立 EVE Online 体量的**基础认知**，作为模块划分的上位地图。  
> **读者**：开发者、AI — 实现新玩法前先对号入座，不要按「当前仓库里有什么文件」倒推域边界。

---

## 0. 术语：两套「Task」

| 名称 | EVE 里 | 本项目 |
|------|--------|--------|
| **Command / Task** | 无直接对应（玩家操作指令） | `Task` + `TaskMgr` + `TaskParams` |
| **Mission / Quest** | Agent 任务、剧情、远征 | 将来 **`Domain/Mission`**，与 Command 无关 |

文档与代码评论中：**Mission = EVE 任务**，**Task = 命令总线**。

---

## 1. EVE 不是功能列表，是几层叠加

```text
┌─────────────────────────────────────────────────────────────┐
│  L4 社交 / 经济 / 主权（慢变量，跨星系、跨玩家）              │
│      军团、联盟、主权、市场、合同、声望、派系战争               │
├─────────────────────────────────────────────────────────────┤
│  L3 空间类型（在哪玩、规则包不同）                            │
│      高安 K-space / 低安 / 零安 / 虫洞 J-space / 实例 / 站内  │
├─────────────────────────────────────────────────────────────┤
│  L2 玩法循环（玩家重复做的事）                                │
│      导航、战斗、采集、工业、市场、技能、任务、PI、探索         │
├─────────────────────────────────────────────────────────────┤
│  L1 模拟内核（所有内容共享）                                  │
│      实体、空间、时间片、Dogma 属性、容器、伤害、锁定           │
├─────────────────────────────────────────────────────────────┤
│  L0 数据与持久化                                              │
│      静态表 inv/dogma/map、dyn 存档、订单、sov 索引            │
└─────────────────────────────────────────────────────────────┘
```

**模块划分顺序**：先认 L3 空间类型 + L2 玩法域，再写代码。  
虫洞、副本、主权不是「多几个 Scene」，而是 **SpaceKind + ContentRules**。

---

## 2. 空间类型（L3）— `Universe/*`

| SpaceKind | EVE | 持久性 | 本地 | 主权 | 典型内容 |
|-----------|-----|--------|------|------|----------|
| **K-space 高安** | 0.5–1.0 | 永久 | 有 | CONCORD | rat、agent、贸易 |
| **K-space 低安/零安** | 0.4–0.0 | 永久 | 有 | 玩家 Sov | rat、建筑、sov 结构 |
| **Wormhole (J-space)** | 虫洞星系 | 连接临时 | 无 | 无 | Sleeper、质量/坍缩 |
| **Instance** | 任务空间、深渊等 | 实例生命周期 | 隔离 | 无 | 任务怪、倒计时 |
| **Structure** | 站内、机库、停靠 | 绑建筑/站 | 站内规则 | 视空间 | 装配、仓储 UI |

**模块归宿**：

| 子模块 | 职责 |
|--------|------|
| `Universe/KSpace` | 星域/星座/星系、星门网、安等 |
| `Universe/Wormhole` | 虫洞连接、质量、J-class 规则 |
| `Universe/Instance` | 副本/深渊/任务空间进出与销毁 |
| `Universe/Structure` | 停靠、站内、机库视图 |

**禁止**：为虫洞复制 `SolarSystem` 类再堆 3000 行 `if (isWH)`；应扩展 SpaceKind + Rules。

---

## 3. 玩法域（L2）全景

### 3.1 导航与探索

| 域 | EVE | 本项目 MVP |
|----|-----|------------|
| **Navigation** | 跃迁、接近、环绕、停靠、跳门 | 部分 Task 已有 |
| **Exploration** | 探针、扫描、异常信号 | 未做 |

探索发现签名 → 打开 **Instance** 入口；与 Navigation 分域。

### 3.2 战斗与 PvE 内容

| 层 | 职责 | 本项目 |
|----|------|--------|
| **Combat**（机制） | 锁定、武器、伤害、EWAR、电容… | Locking、Weapon、Equipment |
| **PvEContent**（内容） | 异常、复合体、Incursion、世界级事件 | 仅 belt rat 级 |

**缝**：Combat 管「怎么打」；PvE 管「刷什么、掉什么、入口在哪」。

### 3.3 Mission（EVE 任务，非 Command Task）

- Agent 类型、目标检查、奖励、声望
- 与 **Instance** 联动（任务空间）
- **独立域** `Domain/Mission`，禁止塞进 UI 或 `TaskMgr`

### 3.4 Harvesting（采集）

| 子域 | EVE | 本项目 |
|------|-----|--------|
| Asteroid | 激光采矿 | Miner 链已有 |
| Ice / Gas | 特殊矿 | 未做 |
| **PlanetaryInteraction** | 行星工厂、路由 | 未做（独立小游戏） |
| Moon / Sov 矿 | 与主权绑定 | 远期 |

**禁止**：PI 逻辑塞进 `StorageComponent`。

### 3.5 Fitting / Inventory / Industry / Market

| 域 | EVE | 本项目 |
|----|-----|--------|
| **Fitting** | 高/中/低/rig、CPU/PG | Equipments + 槽位 Container |
| **Inventory** | 资产位置、堆叠、转移 | dynGameObjects + transfer/create |
| **Industry** | 精炼、制造、发明、反应 | Refining + industry 表 |
| **Market** | 区域订单簿 | dynMarket 表 + UI 壳 |
| **Contracts** | 玩家合同 | 可砍，模块名保留 |

### 3.6 Character / Social / Sovereignty（L4）

| 域 | EVE | 本项目 |
|----|-----|--------|
| **Character** | 技能、克隆、植入体、Standings | Pilot + Skill |
| **Social** | 军团、联盟、权限 | 未做 |
| **Sovereignty** | TCU、IHUB、ADM、计时 | 未做 |

**禁止**：主权/军团逻辑进 `SolarSystemMgr`。

### 3.7 NPC 生态

```text
NPC/Core           — 刷新、表驱动、loot、AI tick 调度
NPC/Archetype/*    — Rat、Agent、Sleeper、StationService、Hauler…
```

Rat 与 Agent 共享「存在+属性+伤害」，**不共享**决策树与刷新规则。

---

## 4. SpaceKind × 玩法矩阵（速查）

| | K-space | WH | Instance | Structure |
|--|---------|-----|----------|-----------|
| Navigation | ✓ | ✓（无本地） | ✓（隔离） | 停靠规则 |
| Combat | ✓ | Sleeper 包 | Mission 怪 | 有限 |
| Mission | Agent | 少见 | **主战场** | — |
| Sov | Null | ✗ | ✗ | 建筑 claim |
| Market | ✓ | 无 NPC 市场 | ✗ | 站内 |

---

## 5. MVP / 占位 / 不做（路线图语义）

| 状态 | 含义 | 示例 |
|------|------|------|
| **MVP 核心** | 已有或近期必做 | K-space、导航、战斗、装配、库存、技能、精炼 |
| **占位** | 文档+模块名存在，实现可空 | Mission、Market 逻辑、WH、Instance、Exploration |
| **L4 预留** | 只写依赖与 SpaceKind，不实现 | Sov、Corp、PI、Contracts |
| **明确不做** | 文档标注，防 AI 乱塞 | （由项目负责人填写） |

---

## 6. 相关文档

- 模块树与 AI 规则：[[模块划分]]
- 数据表与 Container/Owner 评估：[[数据与对象模型评估]]
- Command 总线：[[任务系统]]、[[TaskParams]]
- 架构原则：[[架构重构指南]]
