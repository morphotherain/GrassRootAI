**项目背景**

本宇宙沙盒类游戏的开发源于个人对宇宙探索题材的浓厚兴趣。在众多宇宙游戏中，《Eve Online》凭借其丰富的玩法和宏大的宇宙设定，展现出巨大的魅力。然而，目前市场上的宇宙沙盒游戏仍存在一些可提升的空间，如性能优化、玩法的创新性等。因此，我们希望开发一款既具有高度自由和丰富玩法，又在性能和创新性上有所突破的宇宙沙盒游戏。

**功能介绍**

目前，游戏已实现了多个基础功能。玩家可以查看飞船仓库，管理自己的飞船资源，了解每一艘飞船的状态和属性。星图系统为玩家提供了广阔宇宙的概览，玩家能够在星图上规划航行路线，进行跃迁操作，利用星门快速穿越不同星系，探索宇宙的各个角落。飞船装配和总览功能允许玩家根据自己的战术需求，对飞船进行个性化的装配和调整。此外，玩家还可以进行进站出站操作，在空间站中补充资源、修理飞船等。游戏中也实现了基本的挖矿和战斗功能，玩家可以在宇宙中采集资源，也能与其他玩家或 NPC 进行战斗。

**仓库**


展示当前舰船的仓库情况，可以拖动物品到其它窗口来进行物品移动和配装等操作


![仓库截图](media/Pasted_image_20250522225141.png)


**装配**

为当前的舰船调整装备并预览装配后的性能


![装配截图](media/Pasted_image_20250522225208.png)

**市场**

进行买入和卖出等操作

![市场截图](media/Pasted_image_20250522225241.png)


**总览**


展示处于同一空间中的其它实体，右键可以选择进行接近，跃迁至，锁定，对空间站可以选择停靠，对星门可以选择跳跃等交互和操作
其中跃迁至需要目标处于150km外，锁定需要目标处于舰船的锁定范围内（如10km），停靠等交互需要接近可交互实体2500m内。


![总览截图](media/Pasted_image_20250522225355.png)


**舰船UI**


已锁定的目标显示在屏幕中间偏上，同时空间中对应被锁定物体的图标会套上十字锁定框
下方的白色半圆弧代表护盾，装甲，结构血量，
黄色光点代表舰船电容电量
右方是舰船装备的主动装备（从上往下为高能量槽，中能量槽，低能量槽），启动的装备会有白色进度条指示运转进度


![舰船UI截图](media/Pasted_image_20250522225543.png)


**开发环境与技术栈**

我们的开发环境基于 Windows 11 操作系统，使用 Visual Studio 2022 作为集成开发环境。游戏引擎完全使用 C++ 配合 DirectX 11 进行开发，DirectX 11 提供了强大的图形渲染和硬件加速功能，确保游戏能够在较低硬件要求下实现高质量的画面效果。此外，项目还集成了 nlohmann 的 JSON 库，用于加载生成处理数据的职责链的配置文件。天空盒纹理使用 Blender 进行生成。


**系统设计：**


本游戏系统围绕基于 DirectX 11 及 C++ 开发的核心框架构建，从依赖图中可看出各模块间紧密关联，以实现丰富的宇宙沙盒游戏功能。整体架构涵盖图形渲染、输入处理、任务管理、组件系统、数据库管理以及用户界面等多个关键部分，各部分协同运作，支撑起游戏的运行。

**使用文档**

软件：需确保系统安装了必要的运行库和驱动程序，如 Visual Studio 2022 相关运行库、显卡最新驱动等。若在游戏启动时提示缺少某些 dll 文件，需根据提示安装对应的运行库。

需要在网盘上下载数据库和贴图资源（约2.3G），解压并放到游戏根目录下

（目前还在开发中暂无法提供最新的资源，请等待第一个稳定版上线）


游戏启动与关闭

启动：在游戏安装目录中，找到游戏可执行文件，双击即可启动游戏。

关闭：在游戏过程中，若要关闭游戏，可使用快捷键 alt + F4 。

**游戏操作指引**

视角与移动：使用 WASD 键控制飞船视角移动。

资源收集：锁定小行星后打开对应采掘装备开始收集矿物。

飞船装备：利用 F1 - F8、shift + F1 - F8、ctrl + shift + F1 - F8 快捷键组合，启停飞船各类装备。在战斗或任务中，可根据实际情况灵活切换装备状态。

界面操作：在总览界面，右键点击唤出右键菜单进行目标相关操作。在其他界面，依据界面提示和上述提到的操作方式，使用鼠标和快捷键完成相应功能操作。

快捷键：


c - 仓库界面


x - 装配界面


m - 星图界面


num1 - 市场界面

