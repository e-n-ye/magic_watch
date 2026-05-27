# UI Page Ownership

日期：2026-05-27

本文定义 Magic Watch UI 页面归属与防复发规则。它不是代码风格指南，而是架构治理入口：新增页面、拆分页面、扩展壳层或继续推进健康/日常应用前，必须先判断页面属于哪个领域。

## 为什么会长出巨石文件

`ShellPages.cpp` 长到 9000+ 行，核心原因不是页面数量本身，而是缺少页面归属规则。

早期 `ShellPages.cpp` 的名字看起来合理：它承载表盘、Launcher、通知、快捷设置等壳层页面。但随着项目推进，所有“不是 Settings 的 UI”都被默认塞进这里，包括健康应用、日常应用、通知唤醒、电源页、息屏页、长续航页、Launcher helper、HomeRing helper、字体 helper、图标 helper、拖拽逻辑、timer 和 EventBus 订阅。

这不是单纯“文件没拆”的问题。真正的问题是：

- 文件名按技术壳层命名，但后续内容按“方便追加”增长。
- 新增页面前没有先回答“它属于哪个域”。
- `ShellPages.h` 同时声明大量无关页面，公共头也变成聚合压力点。
- `Application.cpp` 注册页面很方便，导致新增页面自然流向同一个头和同一个实现文件。
- shared primitive、domain helper、页面状态和业务写入混在一起，使后续拆分越来越难。

如果只把巨石文件切成多个 `.cpp`，但不建立归属规则，后续仍会长出新的 `ShellPages.cpp`，只是换一个名字。

## Settings 拆分后的经验

Settings 拆分后的结构可以作为一个可复用经验：

- `SettingsPages.h` 保留为外部聚合头，减少 `Application.cpp` 和外部 include 改动。
- `Settings/` 作为 Settings 内部实现域，承载 Base、Home、Battery、Layout、Placeholder 等实现。
- `Settings/Display/` 作为复杂子域，承载 Display 的 Basic、Wake、KeepScreenOn、ScreenOff 和 Helpers。
- `SettingsPagePrimitives.*` 只放通用 UI primitive。
- `SettingsDisplayHelpers.*` 只放 Display 域 helper，不放页面实现、不放导航语义、不写 DataCenter。
- 每轮拆分后做构建和手动 UI 回归，避免“结构变清楚但行为悄悄变了”。

值得复用的是：外部聚合头、内部领域目录、复杂子域二级目录、primitive/helper/page implementation 分离、小轮次回归。

不要机械照搬的是：不是所有领域都需要一个聚合头；不是所有子域都要立即二级拆；不是所有 helper 都应该抽到 shared primitive。只有多个页面稳定复用、且不携带业务语义的 UI 构件，才适合进入 shared primitive。

## UI 页面归属分类规则

新增或迁移页面时，优先归到以下类别之一：

### Settings

系统设置页面。典型特征是修改用户偏好、显示配置、电池策略、布局偏好、通知偏好等。应放在 `Pages/Settings/` 或其子目录。

### Shell surfaces

手表主壳层页面，通常从表盘边缘手势、按键、全局状态进入，例如 Launcher、Notifications、QuickSettings。它们可以和 `PageManager` 交互，但具体页面实现不应默认放回 `ShellPages.cpp`。

### Temporary shell overlays

临时壳层、确认弹层、预览层、toast、通知唤醒预览等。必须明确是否常驻、谁创建、谁销毁、是否有 timer、是否订阅 EventBus。

### HomeRing / Watchface host

默认主页、表盘宿主、HomeRing surface、表盘预览和风格渲染。此类页面应独立于 Launcher / Notifications / Settings。

### Launcher

应用列表和快捷入口集合。Launcher 的布局、图标、滚动和表冠处理属于 Launcher 域；应用页面本身不属于 Launcher。

### Notifications

通知列表、通知详情、通知清空确认、通知唤醒预览。通知数据来自 Service / DataCenter，页面不直接读取硬件或 BLE。

### QuickSettings

快捷设置面板、快捷 toggle、toast、快捷操作确认。QuickSettings 可以发起导航请求或写设置模型，但不应承载被打开的详情页实现。

### Power / ScreenOff / LongBattery

电源菜单、息屏页、PoweredOff 占位、长续航表盘、长续航退出页。该类页面要特别标注是否与 `PowerController`、timer、页面栈保存/恢复有关。

### Health apps

Steps、Sleep、HeartRate、BloodOxygen、Stress、Breathing 等健康应用及其设置页、详情页。健康应用不应继续默认进入 Shell 域；应逐步形成 `Pages/Health/` 或按具体健康域分目录。

### Daily apps

Weather、Timer、Alarm、Music、Calendar 等日常应用。它们是应用页面，不是 Shell surface。

### Pay / NFC / Wallet-like shortcuts

NFC、Alipay、WeChat Pay、钱包类快捷入口。它们可能从 Launcher 进入，但实现应属于 Wallet / Pay 域，而不是 Launcher 或 Shell。

### Shared UI primitives

通用 UI 构件，例如滚动列、卡片壳、开关轨道、选择点、返回按钮、通用文字卡。它们不得携带业务语义。

### Domain helpers

领域 helper，例如 Display 文案格式化、电池状态文案、通知卡片格式化。它们可以理解领域模型，但不得创建完整页面，不得混入通用 primitive。

## 新增页面准入规则

新增页面前必须回答：

| 问题 | 目的 |
| --- | --- |
| 这个页面属于哪个域？ | 防止默认塞进 `ShellPages.cpp`。 |
| 它是常驻页面还是临时壳层？ | 决定生命周期、内存和 timer 责任。 |
| 是否需要 `PageManager` 栈？ | 区分 push/pop 页面和临时 surface。 |
| 是否订阅 EventBus？ | 明确订阅取消责任和高频事件风险。 |
| 是否写 DataCenter？ | 判断是否是设置页、应用页还是领域逻辑泄漏。 |
| 是否持有 `lv_timer_t*`？ | 必须遵守 UI 生命周期契约。 |
| 是否应该使用已有 primitive？ | 避免重复造 UI 构件。 |
| 是否应该新建子域目录？ | 防止复杂域继续平铺膨胀。 |
| 是否会让某个文件超过预警线？ | 触发拆分讨论，而不是继续堆功能。 |

如果这些问题无法回答，不应进入实现轮。

## 文件大小预警规则

这些数字不是绝对硬性代码规范，而是架构治理触发器：

- 300 行：关注是否已有多个职责。
- 500 行：新增页面前必须评估是否拆分。
- 800 行：不应继续往里加新页面，必须提出拆分计划。
- 1000 行以上：视为架构债务，不得继续堆功能。

例外需要说明原因，例如单个复杂 watchface 或外部生成/移植文件。但页面集合、壳层集合、设置集合不应以“暂时方便”为理由越过预警线。

## 聚合头规则

可以保留聚合头的情况：

- 外部注册需要稳定 include 入口。
- 拆分实现文件时希望减少 `Application.cpp` 或其它组合根改动。
- 当前阶段仍需要统一暴露一组页面声明。

聚合头的限制：

- 只声明外部注册所需页面。
- 不放页面实现。
- 不放大型 inline helper。
- 不把跨域 helper、领域模型、controller 细节塞进去。
- 当聚合头超过 500 行，应评估是否拆成域内 header，再由聚合头短暂转发。

聚合头是外部入口，不是跨域依赖垃圾桶。

## Application 注册规则

`Application.cpp` 可以作为组合根注册页面。它负责把 `PageId` 绑定到页面工厂，这是合理职责。

但新增注册前必须确认页面归属：

- 页面实现应在所属域目录中。
- 不得因为注册方便就把实现塞进 `ShellPages.cpp`。
- 大量注册持续增长时，应考虑按域提供注册 helper 或 factory。
- 注册 helper 只负责注册，不应创建业务状态或做导航决策。

组合根可以知道页面类型，但不应迫使所有页面实现共享同一个巨型头和巨型 `.cpp`。

## Shared primitive 规则

Shared primitive 只放通用 UI 构件：

- 可以创建 LVGL 基础控件和稳定视觉构件。
- 可以提供字体、颜色、卡片、开关、选择点等通用表现。
- 不写 DataCenter。
- 不订阅 EventBus。
- 不发起导航。
- 不保存页面内部状态。
- 不读取具体领域模型。
- 不把 domain helper 误塞进 primitive。

如果 helper 知道 `DisplayPolicyModel`、`BatteryModel`、`NotificationItem`、健康模型等，它属于 domain helper，不属于 shared primitive。

## ShellPages 拆分前置原则

下一轮 Shell 拆分必须先做归属盘点，而不是直接移动代码。

必须回答：

- 哪些是真 Shell？
- 哪些是 App？
- 哪些是 Health？
- 哪些是 Daily？
- 哪些是 System surface？
- 哪些是 temporary overlay？
- 哪些是 shared primitive？
- 哪些声明必须留在聚合头？
- 哪些可以进入域内 header？
- 哪些 timer、EventBus subscription、DataCenter 写入需要生命周期标注？

拆分顺序应优先低风险、边界清晰的领域。不得在同一轮同时迁移多个高耦合壳层。

## 防复发验收标准

新增 UI 或拆分 UI 后，应满足：

- 新增页面不能默认进入 `ShellPages.cpp`。
- 新增功能必须先有归属判断。
- 超过文件大小预警线必须触发拆分讨论。
- helper 不得混入业务语义。
- 页面不直接触碰硬件。
- Controller 不直接创建 UI。
- `PageManager` 对外语义不因页面归属调整而变化。
- 临时壳层、timer、EventBus subscription 有明确生命周期责任。
- 手动 UI 回归覆盖新增或移动的入口、返回路径和关键交互。
