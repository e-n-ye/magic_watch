# v0.8a 健康监测设置模型边界收口

日期：2026-05-24

## 阶段定位

`v0.8` 的目标不是马上把心率、血氧、睡眠、压力做成完整功能，而是先把“续航优化”页里的那组健康相关开关，从页面局部状态收成一个共享模型边界。

这一轮只做 `v0.8a`：

- 审计当前代码现状
- 定义共享模型的最小字段
- 明确生产者和未来消费者
- 约束下一轮最小代码实现范围

这一轮不改代码。

## 为什么不能直接一轮做完

如果现在直接把“健康监测设置模型闭环”完整做完，会同时碰到：

- `BatteryOptimizationPage` 当前页面内的 5 个开关
- `DataCenter` / `AppEvents` 里的共享模型定义
- 后续 `AppHeartRate` / `AppBloodOxygen` / `AppSleep` / `AppStress` placeholder 页的读取逻辑
- 未来传感器采样与业务策略之间的职责边界

这会让本轮从“一个模型”迅速膨胀成“半条健康线”，风险主要有：

- 范围失控，难以独立验收
- 很容易顺手把 placeholder 页一起改了
- 会过早把“设置共享状态”和“健康业务逻辑”耦在一轮里

所以这里按小闭环拆成三轮更稳：

1. `v0.8a`：模型边界文档收口
2. `v0.8b`：最小共享模型接入 `BatteryOptimizationPage`
3. `v0.8c`：让一个或少量健康 placeholder 页开始消费同一个模型

当前最适合先做的，就是 `v0.8a`。

## 当前事实

### 1. 续航优化页现在仍是页面局部状态

证据在 [SettingsPages.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/SettingsPages.h) 和 [SettingsPages.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/SettingsPages.cpp)：

- `BatteryOptimizationPage` 内部有 `OptionState`
- `OptionState` 里直接保存 `bool enabled`
- 点击开关时，当前页只是在本页 `options_[index].enabled` 上做翻转
- `refresh_options()` 也只刷新本页 switch 外观

这意味着当前这 5 个开关：

- 不在 `DataCenter`
- 没有统一模型
- 离开页面后是否持久生效并不清晰
- 后续健康页也读不到这组状态

### 2. 未来消费者已经存在，但还只是 placeholder

当前主路径里已经可达的健康相关页包括：

- `AppHeartRate`
- `AppBloodOxygen`
- `AppStress`
- `AppBreathing`
- `AppSleep`

它们目前仍是 placeholder，但已经是未来自然消费者。证据在：

- [Application.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Application.cpp)
- [ShellPages.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/UI/Pages/ShellPages.cpp)
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)

### 3. 当前最缺的不是新页面，而是共享设置语义

现在的问题不是“没有更多健康页面”，而是：

- 续航优化页里的开关没有共享归属
- 后续健康页即使要细化，也没有同一份设置模型可读
- 项目会重新落回“每个页面自己记状态”的散乱模式

## 这一轮要收口的共享对象

推荐把这 5 个开关归为同一个共享对象：

1. 睡眠呼吸质量检测
2. 心脏健康监测
3. 全天压力监测
4. 睡眠高精度监测
5. 全天血氧监测

它们的共同点是：

- 都是“是否启用某种长期监测能力”的设置
- 都会影响续航
- 都会在未来与健康类页面产生语义联系
- 当前还不需要引入真实传感器样本，就可以先沉淀成共享状态

## 推荐的最小模型

推荐在下一轮引入一个单独模型，例如：

```cpp
struct HealthMonitoringSettingsModel {
  bool sleep_breathing_quality_enabled {true};
  bool heart_health_monitoring_enabled {true};
  bool all_day_stress_monitoring_enabled {true};
  bool high_precision_sleep_enabled {true};
  bool all_day_blood_oxygen_enabled {true};
};
```

这一轮先不引入更复杂字段，例如：

- 采样频率
- 监测时间窗口
- 自动/手动模式
- 与低功耗模式联动的策略位
- 传感器可用性

原因很简单：这些都还没有两个真实消费者，过早抽象会进入过度设计。

## 推荐的事件与数据归属

下一轮最小实现时，建议采用和 `PowerModeModel`、`DisplayPolicyModel` 一致的归属方式：

- 在 `AppEvents.h` 中定义 `HealthMonitoringSettingsModel`
- 新增一个 `EventId`，例如 `HealthMonitoringSettingsChanged`
- 由 `DataCenter` 持有当前模型
- `BatteryOptimizationPage` 只读/只写 `DataCenter`

也就是说，下一轮的生产者和消费者关系应收成：

```text
BatteryOptimizationPage -> DataCenter(更新共享健康监测设置模型)
                                         ->
                                未来健康页 / 续航优化页 / 可能的策略页读取
```

注意这里还没有 platform sample，也没有 service。

这是有意的：

- 这组状态本质是“用户设置”
- 不是“平台采样结果”
- 因此它更像 `DisplayPolicyModel`，而不是 `StepsModel`

## 下一轮的最小代码闭环应该做什么

`v0.8b` 推荐只做这几件事：

1. 在 `AppEvents.h` 定义 `HealthMonitoringSettingsModel`
2. 在 `DataCenter` 中新增：
   - `publish_health_monitoring_settings(...)`
   - `set_xxx_enabled(...)`
   - `health_monitoring_settings()`
3. 让 `BatteryOptimizationPage` 从页面局部 `bool` 改成读写共享模型
4. 保持 5 个开关的 UI 和行为不变，只改变数据归属

这一轮不做：

- 不修改 `AppHeartRate` / `AppBloodOxygen` / `AppSleep` / `AppStress`
- 不做真实传感器接入
- 不把这些设置和长续航模式策略自动联动
- 不新增 service

## `v0.8c` 的自然消费者顺序

当 `v0.8b` 完成后，`v0.8c` 建议按下面顺序让页面开始消费这份共享状态：

1. `AppSleep`
   - 可自然读取 `睡眠呼吸质量检测`
   - 可自然读取 `睡眠高精度监测`
2. `AppBloodOxygen`
   - 可自然读取 `全天血氧监测`
3. `AppStress`
   - 可自然读取 `全天压力监测`
4. `AppHeartRate`
   - 可自然读取 `心脏健康监测`

建议一次只接一个页面，避免再次把范围做大。

## 当前明确不做的事

这一阶段先不做以下内容：

- 不引入真实心率、血氧、压力、睡眠算法
- 不决定最终传感器字段和采样频率
- 不让续航优化页去控制真实硬件电源策略
- 不把所有健康 placeholder 页一轮内全部接上
- 不把“健康监测设置模型”升级成完整 service framework

## 验收标准

`v0.8a` 文档闭环完成后，应能明确回答：

1. 续航优化页这 5 个开关是否属于同一个共享对象  
   答：属于

2. 这组状态下一轮应归谁持有  
   答：`DataCenter`

3. 它更像 platform sample 还是用户设置  
   答：用户设置

4. 下一轮最小代码实现应先改哪里  
   答：`AppEvents.h`、`DataCenter`、`BatteryOptimizationPage`

5. 下一轮暂时不该碰哪里  
   答：真实传感器、完整健康业务页、复杂低功耗联动

## 结论

`v0.8` 的第一步，不是继续加健康页面，而是先让“续航优化里的健康开关”脱离页面局部状态，拥有清晰的共享模型归属。

这样后续无论是：

- 接健康 placeholder 页
- 做真实传感器设置
- 还是评估这些监测项对续航和长续航模式的关系

都能建立在同一份系统状态上，而不是各页各记一套。
