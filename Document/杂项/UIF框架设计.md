上级目录：[[文档]]

# UIF 框架设计

> UI Framework（`include/UIF/`）— 节点树 + JSON 源文件，与旧 `UIBase` 并存。

## 与旧 UI 边界

| 旧 UI | UIF |
|-------|-----|
| `UIBase` / `UIButton` / `UIWindow*` | `UINode` 树 + `UISystem` |
| 每控件自建 Effect | `UIRenderContext` 共享设备与 D2D |
| `#include SolarSystemMgr` | **禁止**依赖模拟层 |
| C++ 硬编码布局 | `resource/ui/*.ui.json` |

**采用策略**：MainScene / Phase C 主菜单用 UIF；SpaceScene 等暂不改。

## 三层架构

```
JSON → UIDocumentLoader → UINode 树
                              ↓
                    UISystem (Update / Draw / HitTest)
                              ↓
              UINavigationStack + IUIScreenController（Phase C）
                              ↓
              UIRenderContext (D3D11 Sprite + D2D Text)
                              ↓
                    UIActionRegistry → GameApp 回调
```

## 坐标系

- 原点：窗口客户区左上角
- 单位：逻辑像素（与 `Scene::m_ClientWidth/Height` 一致）
- `rect`：相对**父节点**的 `{x, y, w, h}`；根节点相对屏幕

## 生命周期

1. `UIDocumentLoader::LoadFromFile`
2. `UISystem::SetDocument` → `Build(renderContext)`
3. 每帧：`Update(input)` → `Draw()`
4. `Measure/Arrange`：v1 为 pass-through，预留 LayoutNode

## JSON Schema v1

见 [`resource/ui/dev_main_menu.ui.json`](../../resource/ui/dev_main_menu.ui.json)。

| 字段 | 说明 |
|------|------|
| `version` | 固定 `1` |
| `id` | 文档 id |
| `root` | 根节点 |
| `type` | `Panel` / `Text` / `Sprite` / `Button` |
| `id` | 节点唯一 id（文档内） |
| `rect` | `{x,y,w,h}` |
| `visible` | 可选，默认 true |
| `children` | Panel 子节点数组 |
| `text` | Text / Button 文案 |
| `texture` | Sprite / Button 贴图路径 |
| `action` | Button 点击时 `UIActionRegistry` 的 id |

## 节点类型 v1

| 类型 | 职责 |
|------|------|
| `Panel` | 容器，递归 Draw/HitTest |
| `Text` | D2D 文本 |
| `Sprite` | Effect 贴图四边形 |
| `Button` | Sprite + Text + hover/pressed + action |

**v1 不做**：LayoutNode、九宫格、动画、声明式数据绑定（列表由 Screen 控制器动态填充）。

## 导航栈（Phase C，可复用）

| 组件 | 职责 |
|------|------|
| `UINavigationStack` | Push/Pop JSON 屏幕、Esc 返回、busy overlay |
| `IUIScreenController` | `OnShow` / `OnHide` 生命周期 + action 注册 |
| `UIActionScope` | 屏幕级 action 批量 Unregister |
| `UIGameplayHost` | Scene 注入的 gameplay 回调（不污染 UIF） |
| `UIScreenTextInput` | 弹窗文本输入（WM_CHAR，无需新节点类型） |

新功能界面：**写 JSON + 实现 Controller + Push**，不必改 MainScene 核心逻辑。

存档相关 Screen 在 `include/UIF/screens/`，源文件在 `resource/ui/`。

## 输入

- HitTest：子节点**逆序**（后绘制者优先）
- Capture：鼠标在 Button 上按下至释放
- `UISystem::Update` 返回是否消费输入

## 编辑器（不自举）

- `UIEditorSession` + `UIEditorRenderer`（纯 D2D 线框）
- MainScene **F11** 切换；**方向键**微调 rect；**Ctrl+S** 保存 JSON
- 编辑器 UI 不通过 UIF 加载自身

## 目录

```
include/UIF/          头文件
src/UIF/              实现
resource/ui/          源文件
```

## 实现状态（2026-06-29）

已实现：`UINode` 树、`UISystem`、`UIRenderContext`、`UIDocumentLoader`、`Panel/Text/Sprite/Button` 节点、`UIActionRegistry`、`UIEditorSession`/`UIEditorRenderer`。

MainScene 默认加载 `resource/ui/main_menu.ui.json`（`UINavigationStack` 根屏）；开发基准 `dev_main_menu.ui.json` 仍可用于 F11 编辑。

| 操作 | 按键 |
|------|------|
| 开始新游戏 | UIF 按钮或 Enter（回退模式） |
| 返回主菜单 | SpaceScene **F10** |
| UI 编辑器 | MainScene **F11** |
| 保存 UI JSON | **Ctrl+S**（编辑模式，保存并重载） |
| 添加 Panel 子节点 | **Insert** |
| 删除选中节点 | **Delete**（不可删 root） |
| 移动节点 rect | 方向键（Shift 加速） |

## 渲染职责边界（与 Effect / D3DManager）

UIF **只负责布局与交互**；**图元绕序、Pass 状态、Draw 细节**属于渲染中间层，不应由节点自行选择 `GenerateVertices` 等 API。

### 三层分工

| 层 | 职责 | 现有代码 |
|----|------|----------|
| **控件层**（UIF / UIButton） | rect、纹理路径、文案、HitTest、action | `UINode`、`UISystem` |
| **Pass / 图元层**（待抽取） | UI 四边形顶点（绕序）、正交 MVP、Clear/Present、UI 用 RS/DS | 暂分散在 `UIRenderContext`、`GenerateUIVertices`、`UISkyBox` 手写 |
| **Effect 中间层** | VS/PS/VB/纹理/Blend/Sampler、`apply()` + `Draw` | `Effect`、`D3DManager` |

```
UIF 节点  →  DrawSprite(rect, texture)     // 只传布局，不选 Generate*
UIRenderPass  →  GenerateUIVertices + OrthoCamera + Clear/Present
Effect  →  apply()（不变）
D3DManager  →  设备 / RTV / D2D 单例
```

### 绕序约定（2026-06-29 踩坑记录）

- **2D 正交 UI** 必须使用 `GenerateUIVertices`（绕序对齐 `UIBase::GenerateRectVertex` / `UIButton`）。
- **`GenerateVertices`** 保留给 **3D 场景**（如 MainScene 背景 + `FirstPersonCamera`）；UIF **禁止**直接调用。
- 默认 `CullMode=BACK` 未在 Effect 中覆盖；绕序错误时 VS 有输出、PS 不运行。

### 与旧 UI 的渲染差异

| | 旧 MainScene 背景 | UIButton / UIF Sprite |
|--|-------------------|------------------------|
| 顶点 | `GenerateVertices` | `GenerateUIVertices` |
| 相机 | `FirstPersonCamera` | `OrthographicCamera` |
| 设备 | `D3DManager`（Effect 内部） | 同左（`UIRenderContext` 委托 `D3DManager`） |

文本绘制对齐 `UIText::DrawUI`：经 `D3DManager` 取 D2D，**每次 Draw 独立 `BeginDraw`/`EndDraw`**，不与 D3D Sprite 包在同一 batch 内。

## 后续升级路线（低风险）

1. **B.7** 抽取 `UIRenderPass` / `DrawUIQuad(rect, tex)`，`GenerateUIVertices` 收进 Pass 层。
2. UIF 节点只调 Pass API；JSON schema 不变。
3. `UIBase::GenerateRectVertex` 内部转调 `GenerateUIVertices`（行为不变 PR）。
4. 旧 `UIWindow*` 按 Phase 4 逐个迁移；**SpaceScene 3D 渲染不碰**。

**原则**：只加不拆（Strangler）；每项独立 PR + MainScene 冒烟（UIF 主菜单 → 新建 → 进 SpaceScene → F10 回主菜单）。
