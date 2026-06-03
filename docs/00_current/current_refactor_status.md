# Magic Watch Current Refactor Status

日期：2026-06-02

本文档只回答一件事：

- 当前代码重构已经真实推进到哪里

它不是长期架构原则文档，也不是历史流水账。
它的目标是为接下来的文档同步轮提供一个稳定事实基线，避免继续用早期“待拆巨石”叙事描述已经变化的代码结构。

## Scope Lock

Allowed files:

- `docs/00_current/current_refactor_status.md`

Forbidden files:

- `sim/lv_port_pc_vscode/src/**/*`
- `sim/lv_port_pc_vscode/CMakeLists.txt`
- `docs/document_map.md`
- `docs/10_architecture/**/*`
- `docs/20_refactor/**/*`
- 其他所有代码和文档

Forbidden changes:

- 禁止修改任何 C++ 代码。
- 禁止修改构建登记。
- 禁止顺手同步 `document_map.md`。
- 禁止把本轮扩成“全量文档重写”。
- 禁止提前进入页面继续拆分或 UI bug 修复。

## 为什么不能再只用旧叙事

此前很多入口文档曾把 `SettingsPages.cpp` 和 `ShellPages.cpp` 作为“未来待拆巨石”的主要事实背景。

这个判断并不完全错，但已经不够准确：

- `Settings / Display` 已经拆到二级目录。
- `Power` 域页面已经从 `ShellPages.cpp` 下沉。
- `Daily` 的 `Weather` 与 `Steps` 已经下沉。
- `Health` 的 `Sleep`、`BloodOxygen`、`HeartRate` 已经形成独立实现文件。
- 一批 Shell shared helpers 和 Health shared helpers 已经出现。

如果文档只写“不要机械拆 `ShellPages.cpp` / `SettingsPages.cpp`”，团队能知道大方向，却看不清当前代码真实落点，也很难判断下一轮文档应更新哪里。

## 当前代码真实结构总览

### Settings

- `SettingsPages.h` 仍是当前外部聚合头，`Application.cpp` 继续通过它注册 Settings 相关页面。
- `Settings/` 目录已经承载：
  - `SettingsHomePages.cpp`
  - `SettingsBasePages.cpp`
  - `SettingsBatteryPages.cpp`
  - `SettingsLayoutPages.cpp`
  - `SettingsPlaceholderPages.cpp`
  - `SettingsPagePrimitives.*`
- 这说明 Settings 已不再是“只有一个大实现文件”的状态，但外部入口还没有彻底按域分头治理。

当前判断：

- 已从旧单文件实现阶段明显前进。
- 聚合头仍保留，属于有意控制 `Application.cpp` 改动面的过渡结构。

### Settings / Display

- `SettingsDisplayPages.cpp` 已不存在。
- Display 已下沉到 `Settings/Display/`：
  - `SettingsDisplayBasicPages.cpp`
  - `SettingsDisplayWakePages.cpp`
  - `SettingsDisplayKeepScreenOnPages.cpp`
  - `SettingsDisplayScreenOffPages.cpp`
  - `SettingsDisplayHelpers.*`
- `CMakeLists.txt` 已登记上述文件。

当前判断：

- `Display` 是目前最明确完成二级子域化的 Settings 样本。
- 文档若仍假设 `SettingsDisplayPages.cpp` 存在，已经与代码事实冲突。

### Shell shared helpers

当前已存在的 Shell shared helpers：

- `Shell/ShellPagePrimitives.*`
- `Shell/ShellFontHelpers.*`
- `Shell/ShellClickGuard.*`
- `Shell/ShellCrownScrollHelpers.*`
- `Shell/ShellAssetHelpers.*`
- `Shell/ShellImagePrimitives.*`
- `Shell/ShellAppVisualRegistry.*`
- `Shell/ShellAppIconPrimitives.*`

其中：

- `ShellAppVisualRegistry.*` 当前承载 `AppVisualSpec`、app visual specs 表和 `find_app_visual_spec(...)`。
- `ShellAppIconPrimitives.*` 当前承载 `create_app_round_icon(...)` 这类 Shell app 圆形图标 helper。

当前判断：

- 这些 helper 已经是多个页面域的真实依赖，而不是未来规划。
- 它们构成了“页面已拆，但共享 UI 构件开始沉淀”的第二阶段事实。

### Shell / HomeRing single-domain helpers

- `WatchfacePage` 已迁入 `Shell/Home/ShellHomePages.cpp`。
- `HomeRingHostPage` 已迁入 `Shell/Home/ShellHomePages.cpp`。
- `WatchfacePage` 与 `HomeRingHostPage` 声明已下沉到 `Shell/Home/ShellHomePages.h`，`ShellPages.h` 仅通过过渡 include 暴露 Home 声明。
- `Shell/Home/ShellHomeLayoutPrimitives.*` 已新增并登记到 `CMakeLists.txt`。
- `Shell/Home/ShellHomePaymentCardPrimitives.*` 已新增并登记到 `CMakeLists.txt`。
- `Shell/Home/ShellHomeHealthCardPrimitives.*` 已新增并登记到 `CMakeLists.txt`。
- `Shell/Home/ShellHomeDailyCardPrimitives.*` 已新增并登记到 `CMakeLists.txt`。
- 当前承载的边界包括：
  - `SurfaceLayout`
  - `make_surface_layout()`
  - `style_surface_stage()`
  - `create_stage_root()`
  - `style_pager_root()`
  - `create_pager_dot()`
  - `set_single_line_label()`
- HomeRing payment / NFC 卡片纯 UI 构建 helper
- HomeRing health 类卡片纯 UI 构建 helper
- HomeRing daily 类卡片纯 UI 构建 helper

当前判断：

- `ShellPages.cpp` 中不再保留 `WatchfacePage::` 实现。
- 这不只是 helper 下沉事实；`HomeRingHostPage` 页面实现本体也已经迁入 `Shell/Home/`。
- `ShellPages.cpp` 中不再保留 `HomeRingHostPage::` 实现。
- payment / NFC 的点击绑定、`PageId` 选择与导航行为仍留在 `HomeRingHostPage`。
- health 卡片的点击绑定、`PageId` 选择与导航行为仍留在 `HomeRingHostPage`。
- daily 卡片的动态数据、点击绑定、`PageId` 选择与导航行为仍留在 `HomeRingHostPage`。
- 这不代表 Wallet / Pay / NFC 已有独立页面实现。
- 这也不代表 HomeRing 整页已迁出。
- 后续文档若继续把 HomeRing 描述成“只有页面未迁出，尚无单域 helper 边界”，会低估当前拆分进度。

### Shell / Notifications shared visual helpers

- `Shell/Notifications/ShellNotificationPrimitives.*` 已新增并登记到 `CMakeLists.txt`。
- `Shell/Notifications/ShellNotificationPagePrimitives.*` 已新增并登记到 `CMakeLists.txt`。
- 它承载 Notifications 域共享视觉 helper，包括：
  - `notification_card_color(...)`
  - `notification_read_card_color(...)`
  - `notification_primary_text_color(...)`
  - `notification_secondary_text_color(...)`
  - `notification_accent_color(...)`
  - `create_notification_icon(...)`

当前判断：

- `ShellNotificationPagePrimitives.*` 只承载 `NotificationsPage` 单域纯 UI 构建 helper。
- `NotificationWakePage` 已迁入 `Shell/Notifications/ShellNotificationPages.cpp`。
- `NotificationsPage` 已迁入 `Shell/Notifications/ShellNotificationPages.cpp`。
- `NotificationWakePage` 与 `NotificationsPage` 声明已下沉到 `Shell/Notifications/ShellNotificationPages.h`，`ShellPages.h` 仅通过过渡 include 兼容通知声明。
- `ShellNotificationPrimitives.*` 仍是通知视觉共享 helper 边界。
- `Application.cpp` 内的 `NotificationToastOverlay::hide_timer_` 已迁入 `LvglTimerGuard`。
- `NotificationWakePage::auto_close_timer_` 已迁入 `LvglTimerGuard`。
- `NotificationsPage::preview_close_timer_` 已迁入 `LvglTimerGuard`。
- `ShellPages.h` 仍是过渡聚合 include 头，`Application.cpp` 注册未变。
- 这不代表通知复杂行为已经低风险；swipe / sheet / clear confirm 仍需手动回归。

### Shell / QuickSettings single-domain UI primitives

- `Shell/QuickSettings/ShellQuickSettingsPrimitives.*` 已新增并登记到 `CMakeLists.txt`。
- 它只承载 `QuickSettingsPage` 单域纯 UI 构建 helper，包括 backdrop shell、sheet shell、toast、long-battery confirm overlay、toggle grid 与 drag handle。
- `QuickSettingsPage` 已迁入 `Shell/QuickSettings/ShellQuickSettingsPages.cpp`。
- `QuickSettingsPage` 声明已下沉到 `Shell/QuickSettings/ShellQuickSettingsPages.h`，`ShellPages.h` 仅通过过渡 include 兼容快捷设置声明。
- `ShellPages.cpp` 中不再保留 `QuickSettingsPage::` 实现。
- `QuickSettingsPage::toast_timer_` 与 `preview_close_timer_` 已迁入 `LvglTimerGuard`。
- DataCenter 读写、EventBus 订阅、toggle / slider 行为、NavigationCommand、timer 与 shell surface 行为仍留在 `QuickSettingsPage`。
- `ShellPages.h` 仍是过渡聚合 include 头，`Application.cpp` 注册未变。

### Shell / Launcher

- `LauncherPage` 已从 `ShellPages.cpp` 下沉到 `Shell/Launcher/ShellLauncherPages.cpp`。
- `LauncherPage` 声明已下沉到 `Shell/Launcher/ShellLauncherPages.h`，`ShellPages.h` 仅通过过渡 include 兼容 Launcher 声明。
- `CMakeLists.txt` 已登记 `Shell/Launcher/ShellLauncherPages.cpp`。
- `LauncherPage::crown_release_timer_` 已迁入 `LvglTimerGuard`。
- `Application.cpp` 注册结构未改，仍显式注册 `LauncherPage`。
- `Application.cpp` 注册结构未改，仍通过现有聚合头注册页面。

当前判断：

- 这说明 Shell 已开始从“共享 helper 抽离”继续推进到“单个高交互页面独立下沉”阶段。
- 这不代表 `HomeRingHost`、`Notifications`、`QuickSettings` 已经迁出。
- 这也不代表 Wallet / Pay / NFC 已有独立页面实现；它们当前仍是入口卡片或 placeholder 路径。

### Power

- Power 域页面已下沉到 `Shell/Power/ShellPowerPages.cpp`。
- Power 域页面声明已下沉到 `Shell/Power/ShellPowerPages.h`，`ShellPages.h` 仅通过过渡 include 兼容 Power 声明。
- 当前承载页面包括：
  - `PowerMenuPage`
  - `ScreenOffPage`
  - `LongBatteryWatchfacePage`
  - `LongBatteryExitPage`
  - `PassiveShellPage`
- `CMakeLists.txt` 已登记 `Shell/Power/ShellPowerPages.cpp`。

当前判断：

- Power 已不应再被描述为 `ShellPages.cpp` 的内部子块。
- 相关文档若仍把电源页实现位置写成 `ShellPages.cpp`，会误导后续代码检索。

### Daily / Weather

- Weather 页面已下沉到 `Daily/WeatherPages.cpp`。
- `WeatherAppPage` 声明已下沉到 `Daily/WeatherPages.h`，`ShellPages.h` 仅通过过渡 include 兼容 Weather 声明。
- 当前仍由 `Application.cpp` 注册。

当前判断：

- Weather 已完成实现文件迁移。
- 但对外头文件仍是聚合式暴露，属于“实现域已分，注册入口未分”的现状。

### Daily / Steps

- Steps 页面已下沉到：
  - `Daily/StepsPages.cpp`
  - `Daily/StepsPagePrimitives.*`
  - `Daily/StepsMainPrimitives.*`
- `StepsAppPage` 与 `StepsDataInfoPage` 声明已下沉到 `Daily/StepsPages.h`，`ShellPages.h` 仅通过过渡 include 兼容 Steps 声明。
- `StepsAppPage` 与 `StepsDataInfoPage` 已形成独立 Daily 域实现。
- `StepsAppPage` 的 crown release timer 已迁入 `LvglTimerGuard`，与 `StepsDataInfoPage` 采用同一低风险生命周期模式。

当前判断：

- Steps 不应再被描述为 `ShellPages.cpp` 内部应用页面。
- 它已经是带本域 primitive 的独立 Daily 样本。
- 这也说明 `LvglTimerGuard` 已开始从单页样本推进到同域复用，而不是只停留在 helper 存在但未落地的状态。

### Health / Sleep

- Sleep 页面已下沉到 `Health/SleepPages.cpp`。
- Sleep 页面声明已下沉到 `Health/SleepPages.h`，`ShellPages.h` 仅通过过渡 include 兼容 Sleep 声明。
- `SleepAppPage` 与 `SleepSettingsPage` 的 crown release timer 已迁入 `LvglTimerGuard`；`SleepMonitoringDetailPage` 的同类 timer 仍待后续逐页迁移。
- 当前页面包括：
  - `SleepAppPage`
  - `SleepSettingsPage`
  - `SleepMonitoringDetailPage`
  - `SleepInfoPage`

当前判断：

- Sleep 已从“主路径 placeholder”推进为独立 Health 实现域。
- 这说明 `LvglTimerGuard` 已开始进入 Sleep 域，但当前仍只是单页试点，不应误写成整个 Sleep 文件生命周期风险已消失。

### Health / BloodOxygen

- Blood Oxygen 页面已下沉到 `Health/BloodOxygenPages.cpp`。
- Blood Oxygen 页面声明已下沉到 `Health/BloodOxygenPages.h`，`ShellPages.h` 仅通过过渡 include 兼容 Blood Oxygen 声明。
- `BloodOxygenAppPage`、`BloodOxygenInfoPage`、`BloodOxygenSettingsPage` 与 `BloodOxygenLowOxygenReminderPage` 的 crown release timer 已迁入 `LvglTimerGuard`。
- 共享 helper 已出现：
  - `Health/HealthInfoPagePrimitives.*`
  - `Health/HealthIconPrimitives.*`
  - `Health/HealthSwitchPrimitives.*`
  - `Health/BloodOxygenPageHelpers.*`
- 当前页面包括：
  - `BloodOxygenAppPage`
  - `BloodOxygenSettingsPage`
  - `BloodOxygenLowOxygenReminderPage`
  - `BloodOxygenInfoPage`

当前判断：

- Blood Oxygen 已形成“独立页面实现 + Health shared helpers + domain helper”结构。
- 这说明 `LvglTimerGuard` 已在 BloodOxygen 域内完成简单 crown timer 的同域复用；后续剩余高风险点应转向更复杂的生命周期或测量 timer，而不是继续把简单裸 timer 留在该域。

### Health / HeartRate

- Heart Rate 页面已下沉到 `Health/HeartRatePages.cpp`。
- Heart Rate 页面声明已下沉到 `Health/HeartRatePages.h`，`ShellPages.h` 仅通过过渡 include 兼容 Heart Rate 声明。
- `HeartRateAppPage` 的 entry measurement timer 与 crown release timer 已迁入 `LvglTimerGuard`；回页时已显式恢复 result stage，离页时会隐藏 wear prompt overlay，但同页的更细粒度 wear prompt 显示策略仍待后续单独审计。
- 当前非测量域页面包括：
  - `HeartRateInfoPage`
  - `HeartRateAppPage`
  - `HeartRateSettingsPage`
  - `HeartRateAllDayMonitoringPage`
  - `HeartRateHeartHealthMonitoringPage`
  - `HeartRateHighReminderPage`
  - `HeartRateLowReminderPage`
  - `HeartRateResting30DaysPage`

当前判断：

- `ShellPages.cpp` 已无 HeartRate 非测量页面实现残留。
- 当前更集中的风险仍在 `HeartRatePages.cpp` 内部，但已经从“measurement timer + stage + wear prompt 全部混在一起”进一步收窄到“wear prompt 显示策略与少量 stage 生命周期边界”。

### ShellPages.cpp 退场状态

当前 `ShellPages.cpp` 已不再承载任何页面实现。

退场前的剩余内容曾主要是：

- 已清理前的零引用 `using`
- 已清理前的零引用文本常量
- 已清理前的零引用 QuickSettings 日志 helper
- 已清理前的零引用 generic helper / residue

当前判断：

- `ShellPages.cpp` 当前已无任何 `ClassName::` 页面实现。
- `ShellPages.cpp` 中的零引用 dead residue 已清理，空壳文件也已删除。
- `CMakeLists.txt` 已移除 `ShellPages.cpp` 的编译登记。
- 这说明 `ShellPages.cpp` 已从构建中退场，而不是继续作为空壳源文件保留。
- `WatchfacePage` 与 `HomeRingHostPage` 已共同迁入 `Shell/Home/ShellHomePages.cpp`。
- `NotificationWakePage` 与 `NotificationsPage` 已迁入 `Shell/Notifications/ShellNotificationPages.cpp`；`Shell/Notifications/ShellNotificationPrimitives.*` 继续承载通知域共享视觉 helper。
- `Shell/Notifications/ShellNotificationPagePrimitives.*` 已承载 `NotificationsPage` 单域纯 UI 构建 helper。
- `Shell/QuickSettings/ShellQuickSettingsPrimitives.*` 继续承载 `QuickSettingsPage` 单域纯 UI 构建 helper；`QuickSettingsPage` 页面实现已迁入 `Shell/QuickSettings/ShellQuickSettingsPages.cpp`。
- `HomeRingHostPage` 已迁入 `Shell/Home/ShellHomePages.cpp`；其 layout / stage / pager 纯 UI 构件已下沉到 `Shell/Home/ShellHomeLayoutPrimitives.*`，payment / NFC 卡片纯 UI 构件已下沉到 `Shell/Home/ShellHomePaymentCardPrimitives.*`，health 类卡片纯 UI 构件已下沉到 `Shell/Home/ShellHomeHealthCardPrimitives.*`，daily 类卡片纯 UI 构件已下沉到 `Shell/Home/ShellHomeDailyCardPrimitives.*`。
- `ShellPages.h` 已转为过渡聚合 include 头，不再直接承载页面类声明；`Application.cpp` 注册语义未变。
- 下一步代码收口更可能转向聚合头拆分或注册结构重组，而不是继续迁页面。

### ShellPages.h 聚合头状态

- `ShellPages.h` 已转为过渡聚合 include 头，页面类声明已按 Shell、Daily、Health、Power 等领域下沉到对应目录头文件。
- 这意味着：
  - 实现文件已明显分域；
  - 头文件边界已跟上实现边界；
  - 后续风险转向注册结构分域化与手动 UI 回归。

当前判断：

- 这是当前结构最明显的“已拆一半”特征之一。
- 后续文档需要把“实现迁移完成”和“头文件治理未完成”分开描述，不能混成一句“还没拆完”。

### Application.cpp 注册状态

- `Application.cpp` 仍是页面注册组合根。
- 顶部 include 仍以：
  - `CommonPages.h`
  - `SettingsPages.h`
  - 各 Shell / Daily / Health / Power 域页面头
  为主。
- `register_pages()` 已开始按领域组合；当前 Home 域已通过 `register_shell_home_pages(PageManager&, DataCenter&)` 分流，Notifications 域已通过 `register_shell_notification_pages(PageManager&, DataCenter&)` 分流，QuickSettings 域已通过 `register_shell_quick_settings_pages(PageManager&, DataCenter&)` 分流，Power 域已通过 `register_shell_power_pages(PageManager&, DataCenter&)` 分流，Daily 域已通过 `register_daily_pages(PageManager&, DataCenter&)` 分流，Health 域已通过 `register_health_pages(PageManager&, DataCenter&)` 分流。
- 这说明当前注册层已不再依赖 `ShellPages.h` 的声明主体，但注册结构仍处于“组合根逐域收口”的过渡阶段。

当前判断：

- 这是刻意保守的注册策略，不等于页面实现还在原地。
- 当前已完成 Home、Notifications、QuickSettings、Power、Daily 与 Health 领域注册分流；`register_pages()` 已开始从页面注册长列表收口为领域组合根。
- `ShellPages.h` 仍作为过渡兼容 include 头存在，`Application.cpp` 注册语义未变；这不妨碍各页面声明与实现已迁入对应领域目录。

### CMake 登记状态

- `CMakeLists.txt` 已登记本轮审计涉及的分域文件：
  - `Settings/Display/*`
  - `Daily/StepsPagePrimitives.cpp`
  - `Daily/StepsMainPrimitives.cpp`
  - `Daily/StepsPages.cpp`
  - `Daily/WeatherPages.cpp`
  - `Health/BloodOxygenPageHelpers.cpp`
  - `Health/HeartRatePages.cpp`
  - `Health/BloodOxygenPages.cpp`
  - `Health/SleepPages.cpp`
  - `Shell/ShellAppVisualRegistry.cpp`
  - `Shell/ShellAppIconPrimitives.cpp`
  - `Shell/Home/ShellHomeDailyCardPrimitives.cpp`
  - `Shell/Home/ShellHomePages.cpp`
  - `Shell/Home/ShellHomeLayoutPrimitives.cpp`
  - `Shell/Home/ShellHomeHealthCardPrimitives.cpp`
  - `Shell/Home/ShellHomePaymentCardPrimitives.cpp`
  - `Shell/Notifications/ShellNotificationPagePrimitives.cpp`
  - `Shell/Notifications/ShellNotificationPrimitives.cpp`
  - `Shell/Notifications/ShellNotificationPages.cpp`
  - `Shell/QuickSettings/ShellQuickSettingsPrimitives.cpp`
  - `Shell/QuickSettings/ShellQuickSettingsPages.cpp`
  - `Shell/Launcher/ShellLauncherPages.cpp`
  - `Shell/Power/ShellPowerPages.cpp`

当前判断：

- 这些拆分不是“未接线的文件整理”，而是已进入真实构建路径的结构变化。

## 当前剩余风险

### 风险 1：文档仍大量沿用“待拆巨石”叙事

风险不在于这个叙事完全错误，而在于它会遮蔽已经发生的结构迁移。

直接后果：

- 新轮次容易重复审计已经落地的拆分。
- 读代码的人会去旧文件里找已经迁出的页面。

### 风险 2：聚合头状态与实现落点容易被混淆

当前最容易误判的是：

- 因为 `ShellPages.h` 仍作为兼容 include 头存在，或 `Application.cpp` 仍集中注册页面
- 就误以为 Daily / Health / Power / Display 仍在老文件里声明或实现

这会直接影响文档准确性与后续拆分判断。

### 风险 3：HeartRate 风险位置已经转移

当前 HeartRate 的主要风险已不再是“还残留在 `ShellPages.cpp`”。

更真实的风险点是：

- `HeartRateAppPage`
- `measurement_timer_`
- 页面测量阶段切换
- wear prompt 的显示与关闭边界

后续文档若继续把 HeartRate 风险写成“ShellPages 残留”，会把问题定位到错误位置。

## 当前文档同步建议

后续同步应按以下顺序推进：

1. 保持 `docs/document_map.md` 指向当前工作树，并把长历史文档留作按需检索。
2. 继续维护 UI 页面归属与拆分状态文档，明确哪些域已下沉、哪些仍受聚合头和注册结构影响。
3. 再整理手动回归清单与当前结构之间的映射，避免“行为回归文档知道页面，但结构文档不知道页面在哪”。

## 本轮结论

截至 2026-06-03，项目代码现状已经从“SettingsPages / ShellPages 双巨石”前进到“ShellPages.cpp 已退场、实现文件显著分域下沉、聚合头与注册层仍保守维持”的过渡阶段。

因此当前更准确的描述应是：

- `SettingsPages.cpp`、聚合头和注册结构的风险依然存在；
- 但它们已经不再代表全部页面实现现状；
- 文档必须先承认 `Display`、`Power`、`Daily`、`Health` 的真实下沉事实，后续拆分与风险讨论才不会失真。
