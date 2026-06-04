# UI Page Split Status

日期：2026-06-04

本文档只回答当前一个问题：

- UI 页面拆分现在已经走到哪里

它不是页面归属规则文档，也不是逐轮实施日志。
它的职责是把“哪些页面域已经迁出旧巨石、哪些仍通过聚合头暴露、哪些风险已经转向生命周期 / 注册 / 回归”固定下来，作为后续文档同步和 UI 拆分讨论的结构基线。

## Scope Lock

Allowed files:

- `docs/10_architecture/ui_page_split_status.md`

Forbidden files:

- `docs/document_map.md`
- `docs/00_current/**/*`
- `docs/20_refactor/**/*`
- `sim/lv_port_pc_vscode/src/**/*`
- `sim/lv_port_pc_vscode/CMakeLists.txt`

Forbidden changes:

- 禁止修改任何 C++ 代码。
- 禁止修改页面注册与构建登记。
- 禁止把本轮扩成手动回归清单整理。
- 禁止顺手重写 `ui_page_ownership.md`。
- 禁止把“页面归属规则”和“当前拆分状态”混成一份文档。

## 与相邻文档的分工

- `docs/10_architecture/ui_page_ownership.md`：
  - 解决“一个页面应该属于哪个域”。
  - 防止未来继续把页面默认塞回 `ShellPages.cpp`。
- `docs/00_current/current_refactor_status.md`：
  - 解决“当前代码重构事实是什么”。
  - 覆盖 helper、聚合头、构建登记、当前整体风险。
- 本文档：
  - 只解决“页面拆分版图当前长什么样”。
  - 强调页面域、实现文件、聚合头状态和剩余高风险区域。

## 当前拆分阶段判断

截至 2026-06-02，当前 UI 结构已经不再是“`SettingsPages.cpp` 和 `ShellPages.cpp` 承载几乎全部页面实现”的阶段。

更准确的判断是：

1. 页面实现文件已经显著分域下沉。
2. 聚合头已收口为过渡兼容层，注册入口已开始分域收口。
3. `ShellPages.cpp` 已从构建退场，剩余高风险主要转向注册尾巴、复杂生命周期与手动回归。
4. 后续文档和讨论必须区分：
   - 页面实现是否已经迁出
   - 对外头文件是否仍聚合暴露
   - 剩余风险是否还在旧巨石里

## 页面拆分状态总览

### A. Settings 域

当前实现文件主要位于：

- `Pages/Settings/SettingsHomePages.cpp`
- `Pages/Settings/SettingsBasePages.cpp`
- `Pages/Settings/SettingsBatteryPages.cpp`
- `Pages/Settings/SettingsLayoutPages.cpp`
- `Pages/Settings/SettingsPlaceholderPages.cpp`
- `Pages/Settings/SettingsPagePrimitives.*`

当前状态：

- 已从单一大实现文件状态进入域内拆分状态。
- 但对外仍通过 `SettingsPages.h` 暴露。

拆分判断：

- Settings 主域：已迁出。
- Settings 外部聚合入口：未拆。

风险等级：

- 中。

主要风险：

- 风险已不再是“页面还没迁出”，而是“聚合头仍然承载过多声明、外部仍无法按子域理解结构”。

### B. Settings / Display 子域

当前实现文件位于：

- `Pages/Settings/Display/SettingsDisplayBasicPages.cpp`
- `Pages/Settings/Display/SettingsDisplayWakePages.cpp`
- `Pages/Settings/Display/SettingsDisplayKeepScreenOnPages.cpp`
- `Pages/Settings/Display/SettingsDisplayScreenOffPages.cpp`
- `Pages/Settings/Display/SettingsDisplayHelpers.*`

当前状态：

- `SettingsDisplayPages.cpp` 已不存在。
- Display 已成为当前最清晰的二级子域拆分页样本。

拆分判断：

- Display 页面实现：已迁出。
- Display helper：已域内独立。
- Display 外部暴露：仍通过 `SettingsPages.h`。

风险等级：

- 低到中。

主要风险：

- 文档若继续把 Display 理解成旧 `SettingsPages.cpp` 的子块，会直接误导代码检索。

### C. Shell shared primitives / helpers

当前共享文件包括：

- `Shell/ShellPagePrimitives.*`
- `Shell/ShellFontHelpers.*`
- `Shell/ShellClickGuard.*`
- `Shell/ShellCrownScrollHelpers.*`
- `Shell/ShellAssetHelpers.*`
- `Shell/ShellImagePrimitives.*`
- `Shell/ShellAppVisualRegistry.*`
- `Shell/ShellAppIconPrimitives.*`

当前状态：

- Shell 共享 UI 构件已经形成稳定文件组。
- `ShellAppVisualRegistry.*` 已形成 Shell app 视觉注册表边界，当前承载 `AppVisualSpec`、app visual specs 表和 `find_app_visual_spec(...)`。
- `ShellAppIconPrimitives.*` 已形成 Shell app 圆形图标 helper 边界，当前承载 `create_app_round_icon(...)`。

拆分判断：

- 共享 UI 构件：已开始独立。
- Shell 主页面实现：已完成分域下沉。

风险等级：

- 中。

主要风险：

- 若后续文档仍只描述页面迁移，不描述 shared primitives，会看不清壳层代码为什么还能继续复用旧 helper。

### D. Shell / HomeRing helper boundary

当前实现文件位于：

- `Shell/Home/ShellHomePages.cpp`
- `Shell/Home/ShellHomePages.h`
- `Shell/Home/ShellHomeLayoutPrimitives.*`
- `Shell/Home/ShellHomePaymentCardPrimitives.*`
- `Shell/Home/ShellHomeHealthCardPrimitives.*`
- `Shell/Home/ShellHomeDailyCardPrimitives.*`

当前状态：

- `WatchfacePage` 已从 `ShellPages.cpp` 下沉到 `Shell/Home/ShellHomePages.cpp`。
- `HomeRingHostPage` 已从 `ShellPages.cpp` 下沉到 `Shell/Home/ShellHomePages.cpp`。
- Home 的 `WatchfacePage` / `HomeRingHostPage` 声明已下沉到 `Shell/Home/ShellHomePages.h`，`ShellPages.h` 仅通过过渡 include 继续兼容旧入口。
- 但 HomeRing 的纯 UI layout / stage / pager 构件已经开始形成单域 helper 边界。
- HomeRing payment / NFC card primitives 也已经开始形成单域 helper 边界。
- HomeRing health card primitives 也已经开始形成单域 helper 边界。
- HomeRing daily card primitives 也已经开始形成单域 helper 边界。
- 当前 helper 包括：
  - `SurfaceLayout`
  - `make_surface_layout()`
  - `style_surface_stage()`
  - `create_stage_root()`
  - `style_pager_root()`
  - `create_pager_dot()`
  - `set_single_line_label()`
- payment / NFC 卡片的纯 UI 构建 helper
- health 类卡片的纯 UI 构建 helper
- daily 类卡片的纯 UI 构建 helper

拆分判断：

- HomeRing 页面实现：已迁入 `Shell/Home/`。
- HomeRing 单域 helper：已开始独立。
- HomeRing 外部暴露：已具备 Home 域声明头，`ShellPages.h` 仍通过过渡 include 兼容旧入口。

风险等级：

- 中。

主要风险：

- 若后续文档只按“页面是否迁出”来描述 HomeRing，会把这轮已经成立的单域 helper 边界忽略掉。
- 但这也不应被误写成“HomeRing 行为已经低风险”；当前只是实现落点和纯 UI 构件边界完成下沉。
- 后续仍需谨慎处理 HomeRing 的 EventBus、preview、crown、卡片跳转等行为边界；这些行为当前仍留在 `Shell/Home/ShellHomePages.cpp` 的页面实现内。

### E. Shell / Launcher

当前实现文件位于：

- `Shell/Launcher/ShellLauncherPages.cpp`
- `Shell/Launcher/ShellLauncherPages.h`

当前状态：

- `LauncherPage` 已从 `ShellPages.cpp` 下沉到独立 Launcher 域实现文件。
- `LauncherPage` 声明已下沉到 `Shell/Launcher/ShellLauncherPages.h`，`ShellPages.h` 仅通过过渡 include 兼容 Launcher 入口。
- `LauncherPage` 当前仍直接在 `Application.cpp` 中注册，尚未单独抽出 Launcher 域注册函数。
- 当前判断：这是可接受的阶段性偏离，因为 Launcher 只剩单页入口，继续抽独立注册函数的收益有限。

拆分判断：

- Launcher 页面实现：已迁出。
- Launcher 外部暴露：已具备 Launcher 域声明头，`ShellPages.h` 仍通过过渡 include 兼容旧入口。

风险等级：

- 中。

主要风险：

- 这说明 Shell/Home 当前已经承载 `WatchfacePage` 与 `HomeRingHostPage`，但不代表 `Notifications`、`QuickSettings` 已完成同级迁移。
- 如果后续文档把“Launcher 已迁出”误写成“整个主壳层已拆开”，会直接高估当前壳层边界清晰度。

### E. Shell / Notifications shared visual boundary

当前实现文件位于：

- `Shell/Notifications/ShellNotificationPagePrimitives.*`
- `Shell/Notifications/ShellNotificationPrimitives.*`
- `Shell/Notifications/ShellNotificationPages.cpp`
- `Shell/Notifications/ShellNotificationPages.h`

当前状态：

- Notifications 域已经开始形成共享视觉 helper 边界。
- `NotificationsPage` 单域 UI primitives 已开始独立。
- 当前承载的 helper 包括：
  - `notification_card_color(...)`
  - `notification_read_card_color(...)`
  - `notification_primary_text_color(...)`
  - `notification_secondary_text_color(...)`
  - `notification_accent_color(...)`
  - `create_notification_icon(...)`
- `NotificationWakePage` 已从 `ShellPages.cpp` 下沉到 `Shell/Notifications/ShellNotificationPages.cpp`。
- `NotificationsPage` 已从 `ShellPages.cpp` 下沉到 `Shell/Notifications/ShellNotificationPages.cpp`。
- `NotificationWakePage` / `NotificationsPage` 声明已下沉到 `Shell/Notifications/ShellNotificationPages.h`，`ShellPages.h` 仅通过过渡 include 兼容通知入口。

拆分判断：

- 现在 Shell/Notifications 已同时承载通知视觉 shared primitives、`NotificationWakePage`、`NotificationsPage`，以及 `NotificationsPage` 单域 UI primitives。
- 这不代表通知页复杂行为已经低风险；`NotificationsPage` 的 swipe / sheet / clear confirm / preview close timer 仍需手动回归。

风险等级：

- 中。

主要风险：

- `NotificationsPage` 已迁出，但仍承载 swipe / sheet / clear confirm / preview close timer 等复杂行为。
- `QuickSettings` 仍是独立的高风险剩余区域，不能因为通知 visual helper 下沉就误判主壳层已接近拆完。

### F. Shell / QuickSettings UI primitive boundary

当前实现文件位于：

- `Shell/QuickSettings/ShellQuickSettingsPrimitives.*`
- `Shell/QuickSettings/ShellQuickSettingsPages.cpp`
- `Shell/QuickSettings/ShellQuickSettingsPages.h`

当前状态：

- Shell/QuickSettings 已开始形成单域 UI primitives 边界。
- `ShellQuickSettingsPrimitives.*` 只承载 `QuickSettingsPage` 的 backdrop shell、sheet shell、toast、long-battery confirm overlay、toggle grid 与 drag handle 等纯 UI 构建。
- `QuickSettingsPage` 已从 `ShellPages.cpp` 下沉到 `Shell/QuickSettings/ShellQuickSettingsPages.cpp`。
- `QuickSettingsPage` 声明已下沉到 `Shell/QuickSettings/ShellQuickSettingsPages.h`，`ShellPages.h` 仅通过过渡 include 兼容快捷设置入口。
- `ShellPages.h` 已收口为兼容 include 头；`Application.cpp` 已完成 Home、Notifications、QuickSettings、Power、Daily、Health 的注册分流。

拆分判断：

- 这一步延续了前一轮 UI primitives 下沉边界，并在不触碰 DataCenter、EventBus、timer、NavigationCommand、toggle 状态刷新和 shell surface 行为语义的前提下，下沉 `QuickSettingsPage` 页面本体。

风险等级：

- 高。

主要风险：

- 完整 `QuickSettingsPage` 仍承载 toggle / long press / toast / preview close timer / shell drag 等复杂行为。
- 当前风险已经不再是“QuickSettings 仍留在 `ShellPages.cpp`”，而是 QuickSettings 本页复杂行为仍需手动回归。

### F. Power 域

当前实现文件位于：

- `Shell/Power/ShellPowerPages.cpp`
- `Shell/Power/ShellPowerPages.h`

当前页面包括：

- `PowerMenuPage`
- `ScreenOffPage`
- `LongBatteryWatchfacePage`
- `LongBatteryExitPage`
- `PassiveShellPage`

当前状态：

- Power 域页面已整体从 `ShellPages.cpp` 下沉。

拆分判断：

- Power 页面实现：已迁出。
- Power 头部声明：已具备 Power 域声明头，`ShellPages.h` 仍通过过渡 include 兼容旧入口。

风险等级：

- 中。

主要风险：

- 主要风险不再是“Power 还在 `ShellPages.cpp`”，而是：
  - 电源页面虽然迁出，但仍与 Coordinator、页面栈恢复、screen-off 状态切换强耦合；
  - 文档若只看文件位置，会低估其交互复杂度。

### G. Daily / Weather

当前实现文件位于：

- `Daily/WeatherPages.cpp`
- `Daily/WeatherPages.h`

当前状态：

- Weather 已从旧壳层实现中迁出。

拆分判断：

- 页面实现：已迁出。
- 外部暴露：已具备 Weather 域声明头，`ShellPages.h` 仍通过过渡 include 兼容旧入口。

风险等级：

- 低。

主要风险：

- 结构风险较低，当前更像是“文档未同步”问题，而不是“代码仍混放”问题。

### H. Daily / Steps

当前实现文件位于：

- `Daily/StepsPages.cpp`
- `Daily/StepsPages.h`
- `Daily/StepsPagePrimitives.*`
- `Daily/StepsMainPrimitives.*`

当前状态：

- Steps 已形成“页面实现 + 域内 primitives”的独立 Daily 子域。

拆分判断：

- 页面实现：已迁出。
- 域内共享 UI：已抽出。
- 外部暴露：已具备 Steps 域声明头，`ShellPages.h` 仍通过过渡 include 兼容旧入口。

风险等级：

- 低到中。

主要风险：

- 当前风险主要在文档解释层，而不是结构边界本身。

### H. Health / Sleep

当前实现文件位于：

- `Health/SleepPages.cpp`
- `Health/SleepPages.h`

当前页面包括：

- `SleepAppPage`
- `SleepSettingsPage`
- `SleepMonitoringDetailPage`
- `SleepInfoPage`

拆分判断：

- 页面实现：已迁出。
- 页面声明：已下沉到 `Health/SleepPages.h`，`ShellPages.h` 仅通过过渡 include 兼容旧入口。
- 子域继续细分：暂未需要。

风险等级：

- 中。

主要风险：

- Sleep 已脱离 placeholder 阶段，但文档若仍把它当“Shell 应用页子块”，会遮蔽它与其它 Health 子域的并列关系。

### I. Health / BloodOxygen

当前实现文件位于：

- `Health/BloodOxygenPages.cpp`
- `Health/BloodOxygenPages.h`

共享/辅助文件包括：

- `Health/HealthInfoPagePrimitives.*`
- `Health/HealthIconPrimitives.*`
- `Health/HealthSwitchPrimitives.*`
- `Health/BloodOxygenPageHelpers.*`

当前页面包括：

- `BloodOxygenAppPage`
- `BloodOxygenSettingsPage`
- `BloodOxygenLowOxygenReminderPage`
- `BloodOxygenInfoPage`

拆分判断：

- 页面实现：已迁出。
- 页面声明：已下沉到 `Health/BloodOxygenPages.h`，`ShellPages.h` 仅通过过渡 include 兼容旧入口。
- Health shared helper：已出现。
- domain helper：已出现。

风险等级：

- 中。

主要风险：

- 当前风险在于文档若不同时描述“Health shared helper”和“领域页实现”，会误以为 BloodOxygen 只是单文件迁移。

### J. Health / HeartRate

当前实现文件位于：

- `Health/HeartRatePages.cpp`
- `Health/HeartRatePages.h`

当前页面包括：

- `HeartRateAppPage`
- `HeartRateResting30DaysPage`
- `HeartRateSettingsPage`
- `HeartRateAllDayMonitoringPage`
- `HeartRateHeartHealthMonitoringPage`
- `HeartRateHighReminderPage`
- `HeartRateLowReminderPage`
- `HeartRateInfoPage`

拆分判断：

- HeartRate 非测量域页面：已迁出。
- 页面声明：已下沉到 `Health/HeartRatePages.h`，`ShellPages.h` 仅通过过渡 include 兼容旧入口。
- 旧 `ShellPages.cpp` 对 HeartRate 的实现承载：已基本清空。

风险等级：

- 高。

主要风险：

- 当前风险集中在 `HeartRatePages.cpp` 内部，而不是旧 `ShellPages.cpp` 残留。
- 关键风险点包括：
  - `HeartRateAppPage` 的 measurement timer
  - measurement stage / result stage 切换
  - wear prompt overlay
  - crown release timer

这意味着后续若写“HeartRate 仍未拆完”，必须明确是“生命周期风险未收口”，不是“页面仍未迁出”。

## `ShellPages.cpp` 退场状态

截至当前，`ShellPages.cpp` 已不再承载任何页面实现。

退场前的剩余内容曾主要是：

- 零引用 `using`
- 零引用文本常量
- 零引用 QuickSettings 日志 helper
- 零引用 generic helper / dead residue

当前判断：

- `ShellPages.cpp` 当前 0 页面实现。
- `ShellPages.cpp` 中的零引用 dead residue 已清理，空壳文件也已删除，`CMakeLists.txt` 已移除其编译登记。
- `WatchfacePage` 与 `HomeRingHostPage` 已迁入 `Shell/Home/ShellHomePages.cpp`，但 Watchface 的时间、电量、输入路径，以及 HomeRing 的 EventBus、preview、crown、卡片跳转和 daily 动态数据边界仍需谨慎手动回归。
- `NotificationWakePage` 与 `NotificationsPage` 已迁入 `Shell/Notifications/ShellNotificationPages.cpp`，QuickSettings 页面实现也已迁入 `Shell/QuickSettings/ShellQuickSettingsPages.cpp`。
- 剩余风险已从“页面迁移”转为“兼容聚合头维护、Launcher / Settings 注册尾巴、手动 UI 回归”。
- 手动 UI 回归仍未执行，不应写成已通过。

## 聚合头状态

### SettingsPages.h

当前状态：

- 仍承担 Settings 对外聚合头职责。

优点：

- 保持 `Application.cpp` 注册入口稳定。

风险：

- 对外阅读时容易看不出 Display 与其它 Settings 子域已经拆开。

### ShellPages.h

当前状态：

- 已转为跨 Shell、Power、Daily、Health 的过渡聚合 include 头，不再直接承载页面类声明。

优点：

- 迁移实现文件时减少了组合根改动。

风险：

- 仍可能让读者误以为相关页面需要从兼容聚合头进入。
- 真正剩余风险已转向 `Application.cpp` 中 Launcher / Settings 注册尾巴、以及手动 UI 回归。

## 拆分状态矩阵

| 领域 | 当前实现位置 | 是否已从旧巨石迁出 | 是否仍依赖聚合头 | 当前主要风险 |
| --- | --- | --- | --- | --- |
| Settings 主域 | `Pages/Settings/*` | 是 | 是，`SettingsPages.h` | 聚合头仍厚 |
| Settings / Display | `Pages/Settings/Display/*` | 是 | 是，`SettingsPages.h` | 文档易滞后 |
| Launcher | `Shell/Launcher/ShellLauncherPages.cpp` | 是 | 是，`ShellPages.h` | 高交互页已迁出，但主壳层未整体拆开 |
| Power | `Shell/Power/ShellPowerPages.cpp` | 是 | 是，`ShellPages.h` | 与 screen-off / restore 强耦合 |
| Daily / Weather | `Daily/WeatherPages.cpp` | 是 | 是，`ShellPages.h` | 主要是文档解释滞后 |
| Daily / Steps | `Daily/StepsPages.cpp` + primitives | 是 | 是，`ShellPages.h` | 文档解释滞后 |
| Health / Sleep | `Health/SleepPages.cpp/.h` | 是 | 过渡 include 兼容 | 容易仍被误写成 Shell 子块 |
| Health / BloodOxygen | `Health/BloodOxygenPages.cpp/.h` + helpers | 是 | 过渡 include 兼容 | shared helper 与页面关系不易看清 |
| Health / HeartRate | `Health/HeartRatePages.cpp/.h` | 是 | 过渡 include 兼容 | 生命周期风险高 |
| 剩余 Shell 收口层 | `ShellPages.h` + `Application.cpp` | 是 | `ShellPages.h` 仅过渡 include | Launcher / Settings 注册尾巴与手动回归 |

## 当前最需要避免的误判

### 误判 1：只要 `ShellPages.h` 还在，就说明页面还没拆

不准确。

更准确的说法是：

- 许多页面实现已经迁出；
- 但头文件与注册层仍保守维持聚合暴露。

### 误判 2：HeartRate 仍然主要是 `ShellPages.cpp` 风险

不准确。

更准确的说法是：

- HeartRate 页面位置问题已基本解决；
- 剩下的是 `HeartRatePages.cpp` 内的生命周期和交互状态风险。

### 误判 3：Power 已迁出，所以 Power 风险已经很低

也不准确。

文件位置迁出不等于系统交互复杂度已经降低。
Power 仍与：

- `ScreenOff`
- `LongBattery`
- 页面栈保存/恢复
- `PageManager`
- `AppStateMachine`

保持高耦合，只是它不再混在 `ShellPages.cpp` 里实现。

## 本轮结论

当前 UI 页面拆分已经形成一个比较清晰的中间态：

1. `Display`、`Power`、`Daily`、`Health` 页面实现已显著分域下沉。
2. `SettingsPages.h` 仍承担 Settings 对外聚合入口，`ShellPages.h` 已收口为兼容 include 头。
3. `Application.cpp` 已完成 Home、Notifications、QuickSettings、Power、Daily、Health 的注册分流；当前接受两处阶段性偏离：Launcher 继续直接注册，Settings 继续保留长列表，待后续 Settings 专项审计时再决定是否继续收口。
4. `ShellPages.cpp` 当前已从构建退场；后续更像澄清聚合边界与注册结构，而不是继续迁页面。
5. 后续任何关于“ShellPages 拆分是否继续”的讨论，都不应再基于早期“所有页面都还在壳层巨石里”的旧事实。

## 后续建议

在文档线的下一步，更适合继续做：

- 手动回归清单与当前结构映射整理；

而不是继续重复描述“哪些文件已经迁出”。

因为结构事实、入口事实和页面归属事实三者现在已经有了基本落点，下一阶段更需要把这些结构变化映射到验证闭环上。
