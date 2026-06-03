# v0.4 Battery / PowerStatus service 边界闭环

日期：2026-05-22

## 目的

本文是 v0.4 的第四个小闭环：选择并落地第一个真实 service 边界。

本轮只做 `Battery / PowerStatus service` 的最小边界，不做天气、计步、通知真实化、IMU 姿态 service、触摸手势真实化或 deep sleep 生命周期。

## 为什么先选电池

电池是当前最合适的第一批 service：

- 平台侧已有 `hal::BatterySample`。
- 模拟器已有电池样本生成。
- T-Watch 已验证 AXP2101 基础供电、电池电压、充电和外部供电状态。
- 业务复杂度低，但已经出现了超过模型直通的解释逻辑：低电量通知 latch。
- 页面已经通过 `DataCenter` 订阅 `BatteryModel`，不需要改 UI。

## 本轮代码边界

新增：

- `sim/lv_port_pc_vscode/src/App/Services/BatteryPowerService.h`
- `sim/lv_port_pc_vscode/src/App/Services/BatteryPowerService.cpp`

更新：

- `sim/lv_port_pc_vscode/src/App/Application.h`
- `sim/lv_port_pc_vscode/src/App/Application.cpp`
- `sim/lv_port_pc_vscode/CMakeLists.txt`

## 当前职责

| 层 | 当前职责 |
| --- | --- |
| Platform / HAL | 继续只产生 `hal::BatterySample`。 |
| `Application` | 组合 service，并把 `BatteryChanged` HAL event 转交给 `BatteryPowerService`。 |
| `BatteryPowerService` | 将 `hal::BatterySample` 解释为 `BatteryModel`；发布电池模型；处理低电量阈值和去重 latch；生成低电量通知。 |
| `DataCenter` | 保存并发布 UI 可订阅的 `BatteryModel`；保存通知列表。 |
| UI 页面 | 继续订阅 `BatteryChanged` 并渲染，不解释 PMU 或低电量策略。 |
| `AppStateMachine` | 不读电池样本，不解释 PMU 细节。 |

## 保留边界

本轮没有引入：

- 通用 service framework。
- service registry。
- 异步任务或 RTOS 任务。
- 新的 `PowerStatusModel`。
- PMU 详细字段，例如 VBUS、SYS、充电阶段和 PMU IRQ。
- 真实硬件 BSP。

这些都等第二个真实消费者或更明确的迁移压力出现后再判断。

## 行为说明

`BatteryPowerService` 当前只做三件事：

1. 接收 `hal::BatterySample`。
2. 规范化百分比到 0 到 100，并发布现有 `BatteryModel`。
3. 当电量小于等于 20% 时生成一次低电量通知；电量回到阈值以上后解除 latch。

模拟器热键注入的低电量通知仍然保留，但入口改为调用 service 的注入方法。它仍是 debug 行为，不代表真实平台事件。

## 验收标准

- 页面仍只订阅 `BatteryModel`。
- 平台层仍只提供 `BatterySample`。
- 低电量 latch 不再留在 `Application`。
- 状态机不消费电池原始样本。
- 构建通过。

## 回归记录

2026-05-22：

- 构建通过：`cmake --build build --config Debug`。
- 手工验证通过：启动模拟器后按 `B` 热键，低电量通知仍出现。

## 下一步建议

v0.4 的下一轮不建议继续扩展第二个 service。更稳的下一步是对本轮做一次小回归：

- 启动模拟器确认电池显示仍更新。
- 用 `B` 热键确认低电量通知仍出现。
- 等模拟器电量下降到阈值附近时观察自动低电量通知只触发一次。

如果这条线稳定，再考虑是否把 `Time` 或 `Motion` 的 service 边界写成文档，不急着实现。
