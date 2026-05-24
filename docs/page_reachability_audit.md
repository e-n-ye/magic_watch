# 当前可达页面与遗留页面清点

日期：2026-05-23

## 目的

这份文档只回答一件事：

- 当前模拟器主路径里，用户真的能走到哪些页面
- 仓库里虽然注册了、实现了，但当前主路径并不会走到哪些页面

这样后续讨论“某个页面要不要改”“某张卡是不是已经接了模型”时，先对齐我们说的是：

- 当前主路径页面
- 还是保留在仓库里的遗留/备用页面

## 术语

### 当前可达页面

指用户在当前版本里，能够通过以下方式进入的页面：

- 开机默认进入
- 主页面环滑动
- Launcher 点击
- Settings 点击
- Quick Settings 点击或长按
- 通知/熄屏/长续航等系统状态切换

### 遗留未接入主路径页面

指代码里已经 `register_page(...)`，甚至有完整 UI，但当前主路径没有稳定入口的页面。

这些页面不等于“应该删除”。
当前阶段它们更像：

- 旧实验资产
- 备用实现
- 未来可能重新接回主路径的候选页面

## 一、当前主路径可达页面

### 1. 根页面与系统壳层

这些页面由状态机直接驱动，属于当前产品主路径的一部分：

- `HomeRingHost`
- `Launcher`
- `Notifications`
- `NotificationWakePreview`
- `QuickSettings`
- `PowerMenu`
- `ScreenOff`
- `PoweredOff`
- `LongBatteryWatchface`
- `LongBatteryExit`

补充说明：

- 应用启动时，状态机默认根页面是 `HomeRingHost`
- `Watchface` 虽然也注册了，但当前不是默认根页面
- `SetRoot(Watchface)` 在状态机里也会被归一化到“回到 Home / boot_to_home”，本质上仍回到 `HomeRingHost`

证据：

- [Application.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Application.cpp)
- [AppStateMachine.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/State/AppStateMachine.cpp)

### 2. 主页面环中的内嵌 surface

`HomeRingHost` 自己不是单页列表，而是一个包含 5 个 surface 的宿主。

当前主路径真正可滑到的是：

1. `surface[0]`：主表盘中心页
2. `surface[1]`：支付快捷面
3. `surface[2]`：NFC 校园卡快捷面
4. `surface[3]`：健康快捷面
5. `surface[4]`：天气/睡眠/步数快捷面

注意：

- 这些 surface 不是独立 `PageId`
- 它们是 `HomeRingHostPage::build()` 里直接搭出来的
- 所以你现在平时看到的“主页环快捷卡”，本质上是 `HomeRingHostPage` 内部 UI，不是独立快捷页类

证据：

- [ShellPages.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/ShellPages.cpp)

### 3. 主页面环内嵌卡片当前可进入的目标页

#### `surface[1]` 支付快捷面

- `AppAlipay`
- `AppWeChatPay`

#### `surface[2]` NFC 快捷面

- `AppNfc`

#### `surface[3]` 健康快捷面

- `AppHeartRate`
- `AppBloodOxygen`
- `AppBreathing`
- `AppStress`

#### `surface[4]` 天气/日常快捷面

- `AppWeather`
- `AppSleep`
- `Pedometer`

### 4. Launcher 当前实际暴露的入口

当前 `Launcher` 暴露的是这一组入口：

- `SettingsHome`
- `AppWeather`
- `Pedometer`
- `AppSleep`
- `AppHeartRate`
- `AppBloodOxygen`
- `AppStress`
- `AppBreathing`
- `AppNfc`
- `AppAlipay`
- `AppWeChatPay`

也就是说，虽然仓库里还注册了很多别的页面，但当前 `Launcher` 主路径只对外开放这一小组。

证据：

- [ShellPages.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/ShellPages.cpp)

### 5. Settings 当前实际暴露的入口

`SettingsHomePage` 当前可见入口只有 8 个：

- `SettingDisplay`
- `SettingSound`
- `SettingBattery`
- `SettingDoNotDisturb`
- `SettingNotifications`
- `SettingAppLayout`
- `SettingSystemActions`
- `SettingAbout`

证据：

- [SettingsPages.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/SettingsPages.cpp)

### 6. 当前已接入主路径的二级/三级详情页

#### Steps

- `Pedometer`
- `PedometerDataInfo`

#### Battery

- `SettingBattery`
- `SettingBatteryLifeMode`
- `SettingBatteryOptimization`
- `SettingBatteryInfo`

#### Display

- `SettingDisplay`
- `SettingDisplayBrightness`
- `SettingDisplayManualBrightness`
- `SettingDisplayTimeout`
- `SettingDisplayRaiseToWake`
- `SettingDisplayRaiseToWakeStartTime`
- `SettingDisplayRaiseToWakeEndTime`
- `SettingDisplayKeepScreenOn`
- `SettingDisplayScreenOffDisplay`
- `SettingDisplayScreenOffDisplaySchedule`
- `SettingDisplayScreenOffDisplayStartTime`
- `SettingDisplayScreenOffDisplayEndTime`
- `SettingDisplayScreenOffStyle`

### 7. 当前可达但仍是 placeholder 的页面

这些页面当前用户可以走到，但内容仍然是占位页或轻量 mock：

- `AppHeartRate`
- `AppBloodOxygen`
- `AppBreathing`
- `AppStress`
- `AppSleep`
- `AppNfc`
- `AppAlipay`
- `AppWeChatPay`
- `SettingSound`
- `SettingDoNotDisturb`
- `SettingNotifications`
- `SettingSystemActions`
- `SettingAbout`

这类页面属于“主路径可达，但业务尚未展开”，不要和“完全不可达”混为一谈。

## 二、遗留但未接入当前主路径的页面

### 1. 已在 2026-05-24 收口中删除的独立 HomeShortcut 页面族

以下 4 个独立快捷页类曾经注册过：

- `HomeShortcutPayments`
- `HomeShortcutNfc`
- `HomeShortcutHealth`
- `HomeShortcutWeather`

它们都有自己的页面类和 UI，但当前主路径里用户实际看到的快捷卡并不是这 4 个独立页，而是 `HomeRingHost` 的 4 个内嵌 surface。

因此这 4 页在 2026-05-24 的清理轮次中已删除，原因是：

- 它们不在当前主路径中承担任何可见职责
- 它们与 `HomeRingHost` 内嵌快捷面形成双份实现，容易制造“改对类、改错页面”的混淆
- 当前阶段保留它们的认知成本高于继续持有的价值

### 2. `Watchface`

`Watchface` 页面仍然注册着，也有自己的实现。

但当前状态机默认根页面是 `HomeRingHost`，并且 `SetRoot(Watchface)` 也会被归一化为回主路径 Home。

所以当前阶段应把它标为：

- 保留实现
- 非当前主路径根页面

补充说明：

- 2026-05-24 已删除未参与构建的旧 `WatchfacePage.cpp/.h` 与 `QuickStatusPage.cpp/.h` 孤儿文件
- 当前有效的 `WatchfacePage` 实现在 [ShellPages.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/ShellPages.cpp) / [ShellPages.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/ShellPages.h)

### 3. 已在 2026-05-23 第一轮收口中删除的旧工具/能力页

以下页面属于围绕早期 LilyGo 板级能力规划产生的遗留资产，当前既不在主页环，也不在 Launcher / Settings 主路径中，已在第一轮收口中删除：

- `TimingToolsHome`
- `Stopwatch`
- `Timer`
- `Alarm`
- `GamesHome`
- `Game2048`
- `Gps`
- `Recorder`
- `AudioPlayer`
- `VideoPlayer`
- `Infrared`
- `Lora`
- `Calculator`

### 4. 已在 2026-05-23 第二轮收口中删除的遗留设置页

以下页面曾经注册过，但既不在 `SettingsHomePage` 中暴露，也不再保留独立价值，已在第二轮收口中删除：

- `SettingTimeDate`
- `SettingBluetooth`
- `SettingWifi`
- `SettingDeveloper`
- `SettingVersion`

补充说明：

- `SettingBluetooth` 原本还被 `QuickSettings` 的“寻找手机”按钮长按详情入口引用
- 本轮已同步取消这一路长按详情，不再为它保留独立设置页

## 三、当前最实用的讨论约定

为了避免后续再次“改对类、改错页面”，建议从现在开始统一这样说：

### 当你说“主页环外部显示的天气/步数快捷卡”

默认指：

- `HomeRingHostPage` 的 `surface[4]`

### 当你说“独立 WeatherShortcutPage”

默认指：

- 历史上存在过的 `PageId::HomeShortcutWeather`
- 它在 2026-05-24 清理后已不再保留于当前代码主线

### 当你说“当前主路径可达页面”

默认优先看：

- `HomeRingHost`
- `Launcher`
- `SettingsHome`
- `QuickSettings`
- `Notifications`
- `LongBatteryWatchface`

而不是先看所有 `register_page(...)`

## 四、下一步建议

这份清单完成后，后续页面改动建议先做一个动作：

1. 先确认目标页属于“当前主路径可达页面”还是“遗留未接入页面”
2. 如果是遗留页，先确认它是否仍需保留；若无当前主路径价值，优先删除而不是继续挂着
3. 只有确认要接回主路径后，再继续让它消费共享模型或扩业务

这样能明显减少：

- 误改页面
- 重复造轮子
- 一份业务数据接到两个平行 UI 资产上
