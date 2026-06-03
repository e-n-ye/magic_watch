# v0.8b 健康监测设置模型最小接入

日期：2026-05-24

## 本轮目标

把 `BatteryOptimizationPage` 里的 5 个健康相关开关，从“页面自己记状态”改成“读写共享模型”。

本轮只做：

- 新增 `HealthMonitoringSettingsModel`
- 让 `DataCenter` 持有并发布这组设置
- 让 `BatteryOptimizationPage` 改为读写共享模型

本轮不做：

- 不改 `AppHeartRate`
- 不改 `AppBloodOxygen`
- 不改 `AppSleep`
- 不改 `AppStress`
- 不接真实传感器
- 不引入新的 service

## 改动点

### 1. 共享模型进入 `AppEvents`

在 [AppEvents.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Common/AppEvents.h) 中新增：

- `EventId::HealthMonitoringSettingsChanged`
- `HealthMonitoringSettingsModel`

字段保持最小集合：

- `sleep_breathing_quality_enabled`
- `heart_health_monitoring_enabled`
- `all_day_stress_monitoring_enabled`
- `high_precision_sleep_enabled`
- `all_day_blood_oxygen_enabled`

## 2. `DataCenter` 持有这组设置

在 [DataCenter.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Common/DataCenter.h) 和 [DataCenter.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Common/DataCenter.cpp) 中新增：

- `publish_health_monitoring_settings(...)`
- `health_monitoring_settings()`
- 5 个最小 setter

这样下一轮无论是谁要消费这组状态，都不再需要依赖 `BatteryOptimizationPage` 的局部变量。

## 3. `BatteryOptimizationPage` 改成共享模型编辑器

在 [SettingsPages.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/SettingsPages.h) 和 [SettingsPages.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/SettingsPages.cpp) 中：

- `OptionState` 不再持有 `enabled`
- 改为通过 `OptionKey` 映射共享模型字段
- 页面订阅 `HealthMonitoringSettingsChanged`
- 页面出现时先读 `DataCenter`
- 点击开关时不再翻本地 `bool`，而是写回 `DataCenter`

这意味着当前页现在的角色是：

```text
共享健康监测设置模型的一个编辑器
```

而不是：

```text
自己持有 5 个真实状态的孤立页面
```

## 为什么这轮是正确的边界

这组状态本质上是“用户设置”，不是“平台采样结果”。

因此它更像：

- `DisplayPolicyModel`

而不是：

- `StepsModel`

本轮没有引入 service，是刻意保持克制：

- 现在先统一设置归属
- 以后再决定哪些健康页消费它
- 再以后才可能碰真实传感器与业务逻辑

## 验收标准

本轮完成后，应满足：

1. 进入 `设置 -> 电池 -> 续航优化`
2. 5 个开关都仍可点击
3. 点击后 UI 立即变化
4. 返回上一级，再重新进入 `续航优化`
5. 刚才切换过的状态仍保留
6. 构建通过：`cmake --build build --config Debug`

## 下一轮建议

`v0.8c` 不建议一口气把所有健康页都接上。

更稳的顺序是：

1. 先让 `AppSleep` 读取睡眠相关设置
2. 再让 `AppBloodOxygen` / `AppStress` / `AppHeartRate` 逐个接入

这样能继续保持“小闭环、低风险、可独立回归”的节奏。
