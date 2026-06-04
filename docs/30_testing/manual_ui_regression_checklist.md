# Manual UI Regression Checklist

日期：2026-06-03

本文档不是完整测试矩阵。

它的目标是提供一份更短、更适合当前文档入口的手动回归清单，回答三个问题：

1. 当前做完一轮 UI / 状态 / 页面结构改动后，默认至少该回归哪些路径。
2. 当前不同结构风险分别对应哪些回归包。
3. 什么时候应该只跑一个小包，什么时候必须回到完整矩阵。

完整行为矩阵仍看：

- [simulator_manual_regression_matrix.md](/D:/MY_Desk/watch/magic_watch/docs/30_testing/simulator_manual_regression_matrix.md)

## Scope Lock

Allowed files:

- `docs/30_testing/manual_ui_regression_checklist.md`

Forbidden files:

- `docs/document_map.md`
- `docs/00_current/**/*`
- `docs/10_architecture/**/*`
- `docs/20_refactor/**/*`
- `sim/lv_port_pc_vscode/src/**/*`

Forbidden changes:

- 禁止修改任何代码。
- 禁止重写完整回归矩阵。
- 禁止把本轮扩成自动化测试设计。
- 禁止把回归清单写成历史任务流水账。

## 与相邻文档的分工

- `docs/00_current/current_refactor_status.md`：
  - 说明当前重构事实与主要风险区域。
- `docs/10_architecture/ui_page_split_status.md`：
  - 说明当前页面拆分版图、剩余壳层和聚合头状态。
- `docs/30_testing/simulator_manual_regression_matrix.md`：
  - 保留完整行为检查项。
- 本文档：
  - 只给出当前默认应执行的手动回归包和触发条件。

## 当前为什么需要一份更短的回归清单

当前代码与文档已经进入一个中间态：

- `Display`、`Power`、`Daily`、`Health` 页面实现已经显著分域下沉。
- `ShellPages.cpp` 仍承载主壳层、高交互壳层与一部分高生命周期风险区域。
- `ShellPages.h` / `SettingsPages.h` 仍承担聚合头职责。

如果每次都直接要求通跑完整矩阵，会带来两个问题：

1. 小轮次文档或结构同步很容易失去节奏。
2. 真正应该重点关注的高风险路径，反而会在大而全清单里失焦。

所以当前更合适的做法是：

- 默认先按改动类型跑“小包回归”。
- 涉及壳层、电源、页面栈恢复、通知预览、Display policy 冲突时，再升到更完整的组合回归。

## 当前默认输入约定

当前默认使用模拟器输入：

- `Enter` / `Space`: crown press
- `Q / E`: crown rotate
- `N`: 注入消息通知
- `S`: 注入步数 `+100`
- `R`: 模拟抬腕亮屏
- `F`: 模拟抬腕结束
- `C`: 模拟遮盖息屏
- touch:
  - tap
  - drag
  - flick
  - edge swipe

## 执行前通用前置检查

每次手动回归前至少确认：

1. 可正常构建并启动模拟器。
2. 启动后能进入当前默认主页路径。
3. 当前时间、电量文本能正常刷新一次。
4. 当前轮改动没有越过 Scope Lock。

## 当前默认手动回归包

构建通过只说明当前代码至少还能编译，不等于手动 UI 回归通过。

未实际执行的手动 UI 项必须保持“未执行”或“待执行”，不能因为 `cmake --build` 通过就写成通过。

### Home

适用场景：

- 修改 `HomeRingHost`、`Watchface`、主页环预览或主页快捷卡相关逻辑后。
- 修改主页输入、返回、预览或壳层拉起逻辑后。

至少检查：

1. 启动进入主页路径。
2. 横向滑动与 `Q / E` 能切换 HomeRing。
3. 高风险路径：HomeRing crown/preview 的预览、确认和取消行为一致。
4. 主页下拉能进入通知壳层，上拉能进入快捷设置壳层。

对应完整矩阵：

- `A1`
- `A2`

### Launcher

适用场景：

- 修改 `LauncherPage`、Launcher 入口或主页到 App 页跳转后。

至少检查：

1. crown press 可进入 `Launcher`。
2. 三种 Launcher 布局进入目标应用路径一致。
3. 从 Launcher 进入二级页面后，左缘返回、左上角返回、crown 返回行为一致。

对应完整矩阵：

- `A3`
- `A4`

### Notifications

适用场景：

- 修改 `NotificationsPage`、`NotificationWakePage`、通知预览、详情、清空或删除逻辑后。

至少检查：

1. 注入通知后列表可刷新，toast 正常。
2. 高风险路径：screen-off 下 `N` 可进入 notification wake preview。
3. 高风险路径：preview 点击或 crown 可进入完整通知列表。
4. 高风险路径：通知列表支持 swipe/detail/clear，且 unread -> detail -> read 状态变化正常。
5. `清空` 需要确认，取消不清空，确认后进入空态。
6. 单卡右滑删除、短滑回弹、详情点击、列表纵向滚动互不干扰。

对应完整矩阵：

- `G1`
- `G2`
- `G3`
- `K1`
- `K2`
- `K3`
- `L1`
- `L2`
- `L3`
- `M1`
- `M2`
- `M3`
- `N1`
- `N2`
- `N3`
- `N4`

### QuickSettings

适用场景：

- 修改 `QuickSettingsPage`、tile、drag close、toast、backdrop 或 timer 相关逻辑后。

至少检查：

1. Quick Settings tile 点击、长按与状态刷新正常。
2. 高风险路径：drag close 手势可关闭且不误触其他交互。
3. 高风险路径：toggle / timer / backdrop 状态变化正常。
4. `Quick Settings -> 长续航模式` 的确认弹层与状态联动正常。

对应完整矩阵：

- `O1`
- `O2`
- `O3`
- `O4`
- `P1`
- `P2`
- `P3`

### Power

适用场景：

- 修改 `PowerController`、`Shell/Power/*`、screen-off、wake、long-battery、页面栈保存/恢复逻辑后。
- 修改 Display policy 与 screen-off restore 的交界逻辑后。

至少检查：

1. 自动熄屏后能按当前允许路径恢复。
2. 普通页面自动熄屏后可恢复到原页面。
3. prompt / overlay 自动熄屏后可恢复到原状态。
4. 高风险路径：Power screen-off/restore 行为正确。
5. 进入长续航模式后，主页环、通知下拉、快捷设置上拉均被抑制。
6. 长续航退出页 `Q / E` 进度、自动熄屏恢复、完成退出行为正常。
7. 充电或外接电源触发退出长续航时路径正常。

对应完整矩阵：

- `H1`
- `H2`
- `I1`
- `I2`
- `I3`
- `I4`
- `I5`
- `I6`
- `I7`
- `I8`

### Daily

适用场景：

- 修改 `Daily/WeatherPages.cpp`
- 修改 `Daily/StepsPages.cpp`
- 修改 `StepsPagePrimitives.*` / `StepsMainPrimitives.*`
- 修改 Launcher 入口或主页卡片到 Daily 页的导航后

至少检查：

1. Weather 从主页快捷卡和三种 Launcher 布局进入一致。
2. Weather 纵向 section、横向 forecast 浏览、`Q / E`、返回行为正常。
3. Steps 从主页快捷卡和三种 Launcher 布局进入一致。
4. Steps 主页滚动、`Q / E`、`数据说明` 跳转与返回正常。
5. `S` 注入后，主页步数卡、Steps 主页、长续航步数显示保持一致。

对应完整矩阵：

- `A5`
- `A6`
- `A7`
- `J1`
- `J3`

### Health

适用场景：

- 修改 `Health/SleepPages.cpp`
- 修改 `Health/BloodOxygenPages.cpp`
- 修改 `Health/HeartRatePages.cpp`
- 修改 `HealthInfoPagePrimitives.*`
- 修改 `HealthIconPrimitives.*`
- 修改 `HealthSwitchPrimitives.*`
- 修改 `BloodOxygenPageHelpers.*`
- 修改健康监测共享设置模型与 `BatteryOptimizationPage` 联动后

至少检查：

1. `设置 -> 电池 -> 续航优化` 中 5 个健康监测开关可切换并持久化。
2. Sleep 首页、设置页、说明页与共享模型联动正常。
3. Blood Oxygen 首页、设置页、低血氧提醒、说明页正常。
4. HeartRate 首页、近 30 天静息、设置页、说明页路径正常。
5. HeartRate 的 entry measurement、wear prompt、结果态切换正常。
6. HeartRate 设置页中的共享状态与 `BatteryOptimizationPage` 一致。

对应完整矩阵：

- `Q1`
- `Q2`
- `Q3`
- `R1`
- `R2`
- `R3`
- `R4`
- `S1`
- `S2`
- `S3`
- `S4`
- `S5`
- `T1`
- `T2`
- `T3`
- `T4`
- `T5`
- `T6`
- `T7`
- `T8`
- `T9`

### Settings

适用场景：

- 修改 `Settings/Display/*` 文档或代码后。
- 修改亮度、息屏时间、抬腕亮屏、遮盖息屏、息屏显示、持续亮屏相关逻辑后。
- 修改与 Display policy 联动的设置入口或 Quick Settings tile 后。

至少检查：

1. `设置 -> 显示与亮度` 入口与可见项顺序正确。
2. 亮度模式切换、手动亮度页返回与摘要文本更新正常。
3. 息屏时间切换后摘要文本更新正常。
4. `抬腕亮屏` 三种模式下 `R / F` 行为正确。
5. `单击亮屏`、`遮盖息屏` 的开关行为和 `C` 事件路径正确。
6. `持续亮屏` 的选择、确认、运行时策略正确。
7. `息屏显示` 模式、默认样式、冲突弹层与摘要文本正确。

对应完整矩阵：

- `B1`
- `B2`
- `B3`
- `C1`
- `C2`
- `C3`
- `C4`
- `D1`
- `D2`
- `E1`
- `E2`
- `E3`
- `E4`
- `F1`

## 当前推荐组合回归

### 组合 1：Display + Power

当改动涉及以下任一情况时，至少跑：

- 包 `B`
- 包 `C`

典型场景：

- 抬腕亮屏
- 息屏显示
- 持续亮屏
- screen-off restore
- long battery

### 组合 2：Home / Launcher / Notifications / QuickSettings

当改动涉及以下任一情况时，至少跑：

- `Home`
- `Launcher`
- `Notifications`
- `QuickSettings`

典型场景：

- 临时壳层 drag close
- HomeRing preview
- 通知唤醒
- backdrop / toast

### 组合 3：Health shared settings

当改动涉及以下任一情况时，至少跑：

- `Health`
- 再补 `Power` 中与电池/长续航有关的最小路径

典型场景：

- `BatteryOptimizationPage`
- Sleep / BloodOxygen / HeartRate 设置联动
- 健康共享模型字段变动

### 组合 4：完整高风险关闭包

当改动同时触及以下两类以上区域时，不建议只跑小包，应回到完整矩阵：

- `Home` / `Launcher`
- `Shell/Power/*`
- `Settings/Display/*`
- `Notifications / QuickSettings`
- `HeartRateAppPage` 生命周期相关逻辑

## 当前最需要优先盯住的风险点

### 风险 1：Home / Launcher / Notifications / QuickSettings 高交互区域

当前最容易回归的仍是：

- `HomeRingHost`
- `Launcher`
- `Notifications`
- `QuickSettings`

这些区域往往伴随：

- drag / preview
- timer
- backdrop
- 临时壳层开合
- crown / touch 混合输入

### 风险 2：Power 与 screen-off restore

文件虽然已迁域，但风险依旧集中在：

- `ScreenOff`
- `LongBattery`
- 页面栈保存/恢复
- 自动熄屏 / 抬腕唤醒 / 遮盖息屏 / 通知唤醒交界

### 风险 3：HeartRate 生命周期边界

当前 HeartRate 最需要盯住的不是页面位置，而是：

- entry measurement
- measurement timer
- wear prompt
- result stage
- crown release timer

## 什么情况下必须回到完整矩阵

出现以下任一情况时，不要只跑本文档的小包，应回到完整矩阵：

1. 改动同时跨两个以上高风险域。
2. 修改了 `PageManager`、`AppStateMachine`、输入路由或共享模型发布路径。
3. 修改影响到壳层开合、screen-off restore、notification wake、long-battery 其中两项以上。
4. 需要作为阶段关闭依据，而不是单轮文档或小步结构同步关闭依据。

## 本轮结论

当前最合适的回归策略，不是每轮都全量通跑，也不是完全凭感觉抽查。

更稳的做法是：

1. 先根据改动域选择对应页面结构对应的回归包。
2. 涉及 `Display + Power`、`Home / Launcher / Notifications / QuickSettings`、`Health shared settings` 这类交界时，升级为组合回归。
3. 触及多个高风险域时，回到完整矩阵。

这样可以在保持节奏的同时，把当前真正容易回归的结构风险盯住。
