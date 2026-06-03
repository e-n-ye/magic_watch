# v0.5 Power Status 页面闭环入口

日期：2026-05-22

## 阶段定位

v0.5 的第一目标不是扩展应用数量，而是让 v0.4 抽出的 `BatteryPowerService` 形成完整可见闭环。

当前推荐的小闭环是：

`Settings` 首页新增电池入口，并将 `PageId::SettingBattery` 从 placeholder 变成只读 `Power Status` 页面。

这不是回到“堆页面”，因为它服务于一个明确架构目标：验证 `Platform / HAL -> BatteryPowerService -> DataCenter -> UI` 的边界是否成立。

## 为什么做这个页面

当前已经具备：

- `hal::BatterySample`。
- `BatteryPowerService`。
- `BatteryModel`。
- 多个页面已经能订阅电池模型。
- `PageId::SettingBattery` 仍是 placeholder。
- `SettingsHomePage` 当前没有电池入口，因此用户无法从设置首页进入该页面。

因此，v0.5 第一轮不是单纯替换 placeholder，而是一个更完整的小闭环：

1. 在设置首页露出电池入口。
2. 把 `SettingBattery` 改成只读 Power Status 页面。

它能训练三件事：

- service 不直接碰 UI。
- 页面不解释平台样本。
- 一个真实设置页如何消费领域模型，而不是自带业务逻辑。

## 本轮范围

只做：

- 在 `SettingsHomePage` 中新增电池入口。
- 新增或复用一个设置页类，替换 `PageId::SettingBattery` 的 placeholder。
- 页面订阅 `BatteryChanged`。
- 页面显示现有 `BatteryModel` 中已经有的字段：
  - 电量百分比。
  - 是否有电池。
  - 是否充电。
  - 是否外部供电。
  - 电池电压 mV。
- 无电池样本时显示离线 / 等待数据状态。
- 新增电池说明页，说明续航预估和续航优化的含义。
- 电池主页展示“长续航模式”和“续航优化”两张入口卡片，但本轮不进入对应子页面。

不做：

- 不新增 `PowerStatusModel`。
- 不改 `HAL.h`。
- 不新增 PMU 字段，例如 VBUS、SYS、充电阶段、PMU IRQ。
- 不做电池曲线、耗电估算、续航预测或长续航模式。
- 不接 T-Watch 真机。
- 第一轮不改 Quick Settings 的长续航开关语义；第四轮已单独收口。
- 不新增第二个 service。
- 第一轮不处理 Quick Settings 中“长续航模式”的真实策略语义；第四轮已接入 `PowerModeModel`。

## 建议实现边界

优先复用现有设置页风格：

- 如果 `SettingsPages` 里已有合适的 settings page base，沿用它。
- 页面类可以暂命名为 `BatteryStatusPage` 或 `PowerStatusPage`。
- 页面只读取 `DataCenter::battery()` 和订阅 `EventId::BatteryChanged`。
- 页面不包含低电量阈值判断；阈值仍属于 `BatteryPowerService`。
- 页面不直接依赖 `hal::BatterySample`。

## 验收标准

- `Settings -> Battery` 不再是 placeholder。
- 设置首页能看到并进入 Battery / Power Status 页面。
- 页面初次进入时能显示当前 `BatteryModel`。
- 模拟器电池变化后页面能刷新。
- 电池主页的说明按钮能进入说明页，说明页能返回。
- 按 `B` 热键低电量通知仍出现。
- 构建通过。
- 不引入新的 service framework 或 HAL 字段。

## 轮次拆分

v0.5 按用户确认拆成以下小闭环：

1. 设置首页电池入口、电池主页、说明页。
2. 长续航模式页面和续航优化页面。
3. 进入长续航模式后的表盘页面和退出路径。
4. 快速设置页面中的长续航入口收口。

当前已完成第三轮的最小闭环。第四轮再处理快速设置页面中的长续航入口收口；长续航下的完整输入/低功耗策略回归可作为第四轮前置检查或单独小轮。

## 第一轮结果

已完成：

- `SettingsHomePage` 新增“电池”入口。
- `PageId::SettingBattery` 从 placeholder 改为 `BatteryStatusPage`。
- 新增 `PageId::SettingBatteryInfo` 和 `BatteryInfoPage`。
- 电池主页显示电量百分比、可用时长估算、供电/充电状态和电压。
- 电池主页展示“长续航模式”和“续航优化”卡片，但暂不跳转。
- 电池主页底部 `i` 按钮可进入说明页。
- 构建通过：`cmake --build build --config Debug`。

## 第二轮结果

已完成：

- 新增 `PageId::SettingBatteryLifeMode` 和 `BatteryLifeModePage`。
- 新增 `PageId::SettingBatteryOptimization` 和 `BatteryOptimizationPage`。
- 电池主页中的“长续航模式”和“续航优化”卡片已改为可点击导航。
- 长续航模式页包含主开关、可用时长提示和模式说明；本轮开关只保留本页交互状态，尚不进入真实长续航表盘。
- 续航优化页包含状态提示和 5 个可交互开关占位：睡眠呼吸质量检测、心脏健康监测、全天压力监测、睡眠高精度监测、全天血氧监测。
- 续航优化开关暂不对接心率、血氧、睡眠等 domain model；后续对应页面出现后再收口。
- 构建通过：`cmake --build build --config Debug`。

## 第三轮结果

已完成：

- 新增 `PowerModeModel` 和 `EventId::PowerModeChanged`，用于表达长续航模式是否开启。
- `BatteryLifeModePage` 的主开关从本地占位状态升级为写入 `DataCenter::set_long_battery_mode_enabled()`。
- 新增 `PageId::LongBatteryWatchface` 和 `LongBatteryWatchfacePage`。
- 长续航表盘只显示日期/星期、时间、电量、计步占位信息。
- 新增 `PageId::LongBatteryExit` 和 `LongBatteryExitPage`。
- 长续航表盘单击屏幕后进入退出提示页。
- 退出提示页通过旋转表冠累计进度，满一圈后关闭长续航模式并回到主线表盘。
- 长续航模式开启时会抑制翻腕亮屏、通知/快捷设置边缘入口和主页环切换等主线交互。
- 收到充电或外部供电电池样本时，会关闭长续航模式并回到主线表盘。
- 5 秒自动熄屏仍沿用现有 `DisplayPolicyModel::screen_off_timeout_ms` 和状态机定时器。
- 构建通过：`cmake --build build --config Debug`。

保留边界：

- 计步信息仍为占位 `0`，后续再接真实 Steps model。
- 旋转表冠退出使用模拟器 detent 累计阈值，真实硬件的一圈映射留到平台适配时校准。
- Quick Settings 的长续航开关已与 `PowerModeModel` 收口。
- 长续航下的全部输入屏蔽策略需要继续按手工矩阵验证，尤其是 screen off 后的唤醒路径。

第三轮 bugfix：

- 修复长续航表盘电量/计步前方图标因 CJK 字体缺失而显示乱码的问题，暂改为“电量/步数”文本。
- 修复长续航模式下按下表冠会回到普通主页面的问题；现在表冠按下会回到长续航表盘。
- 修复长续航退出提示页熄屏后唤醒仍停留在退出页的问题；现在长续航模式下从熄屏唤醒统一回到长续航表盘。
- 构建通过：`cmake --build build --config Debug`。

## 第四轮结果

已完成：

- Quick Settings 中的长续航按钮接入 `PowerModeModel`。
- 从 Quick Settings 进入长续航模式前会显示确认弹窗，提示开启后仅保留时间、计步、NFC，旋转表冠或充电可退出。
- 确认后进入长续航模式，后续行为与从设置页开启一致。
- Quick Settings 的长续航按钮会跟随 `PowerModeChanged` 刷新启用状态。
- 长续航退出页的表冠进度改为有方向：顺时针增加退出进度，逆时针减少进度。
- 构建通过：`cmake --build build --config Debug`。

第四轮 bugfix：

- 长续航表盘不再通过全局 `TouchActivity` 进入退出页。
- 进入退出页改由 `LongBatteryWatchfacePage` 根对象的 `LV_EVENT_CLICKED` 触发，并复用 click guard 过滤拖动。
- 因此只有确认是单击长续航表盘时才进入退出页，滑动不会误触。
- 构建通过：`cmake --build build --config Debug`。

保留边界：

- 确认弹窗是 Quick Settings 内局部 overlay，尚未抽成通用确认组件。
- 真实硬件表冠方向与 detent 阈值仍需后续在平台适配时校准。

## 回归建议

实现后至少做：

- 构建：`cmake --build build --config Debug`。
- 手工路径：进入 Settings，打开 Battery 页面。
- 手工路径：观察电量百分比、充电、外部供电、电压文本。
- 手工路径：按 `B`，确认低电量通知仍出现。
- 目标回归：从 Battery 页面返回 Settings，再回表盘。

## 后续再考虑

只有当只读页面稳定后，才考虑：

- 是否引入 `PowerStatusModel`。
- 是否把电池健康、电源来源、PMU 详细字段拆出来。
- 是否接入 T-Watch AXP2101 的更完整字段。
- 是否让 Quick Settings 的长续航模式进入真实策略。
