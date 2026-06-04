# Stage 9 Hardware Bridge Plan

日期：2026-06-04

## 目标

阶段 9 的目标不是继续在 PC 模拟器里证明架构“看起来能跑”，而是把 Magic Watch 的最小架构子集交叉编译进真实 MCU，并在 T-Watch S3 Plus 真机上跑通一条低频、低风险、可观测的数据流。

第一条垂直切片固定为 Battery / PowerStatus：

```text
AXP2101 -> hal::BatterySample -> BatteryPowerService -> DataCenter -> simplified EventBus -> Serial observer
```

这条切片只证明真实硬件桥接与架构下放能力，不代表完整 Magic Watch UI 已移植到真机。

## 当前依据

- T-Watch S3 Plus 参考板已经完成最小显示、触摸、AXP2101 基础供电状态、BMA423 基础姿态、screen off/on、deep sleep 与 PMU 侧键唤醒的 bring-up 观察。
- Magic Watch 模拟器侧已有 `hal::BatterySample`、`BatteryPowerService`、`DataCenter` 和同步 `EventBus`。
- F411 旧板可作为 STM32 + FreeRTOS + 多传感器经验对照，但不作为阶段 9 第一桥接对象。

## 硬约束

阶段 9 硬件桥接闭环必须至少有一轮在真实 MCU 上交叉编译并运行 Magic Watch 最小架构子集。

以下内容不能算作阶段 9 硬件闭环完成：

- 只读取 AXP2101 并打印 PMU log。
- 只把硬件 log 回放到 PC 模拟器。
- 只在 PC 上验证 `BatteryPowerService`。
- 只写硬件边界文档但不交叉编译架构子集。
- 宣称完整 Magic Watch UI 已移植到真机。

PC trace replay / log replay 只能作为辅助测试，用来对比模型转换结果或复现实验样本，不能作为硬件落地证明。

## H9-Q2 后续代码方向

### H9-BRIDGE-2A：底层读数闭环

在 T-Watch S3 Plus 上继续使用 PlatformIO / Arduino 工程，稳定读取 AXP2101 电池状态，并从串口输出：

- 电池电压。
- 电量百分比。
- 充电状态。
- 外部供电状态。
- `free_heap`。

当前收口要求：

- 这一轮只证明板级读数与基础资源观测链路稳定存在。
- 这一轮不证明 `hal::BatterySample`、`BatteryPowerService`、`DataCenter` 或 `EventBus` 已经下放到真机。

### H9-BRIDGE-2B：最小架构子集下放

把最小化的 `hal::BatterySample`、`BatteryPowerService`、`DataCenter` 和简化 `EventBus` 移植进 T-Watch 工程，解决 Xtensa / Arduino / FreeRTOS 编译差异。

本轮不追求完整复用 PC 模拟器代码，也不移植完整 UI。

### H9-BRIDGE-2C：真实 FreeRTOS 任务闭环

在 T-Watch 上创建真实 FreeRTOS `Power_Task`，以 1Hz 频率轮询 AXP2101，把读数封装为 `BatterySample` 并推入板载 `DataCenter`。

### H9-BRIDGE-2D：串口事件观测

在 T-Watch 串口订阅 `BatteryChanged` 事件，打印：

- 当前电池模型。
- `free_heap`。
- `Power_Task` high water mark。

### H9-BRIDGE-2E：复用边界记录

记录哪些 Magic Watch 上层代码可以直接复用，哪些因为 `std::function`、`std::vector`、`std::string`、同步 `EventBus` 或 UI 依赖暂时不能直接下放。

## 阶段 9 第一验收定义

第一轮真正通过的标准是：

- T-Watch prototype 使用 PlatformIO 交叉编译通过。
- 真机串口能看到由板载 `EventBus` 发出的 `BatteryChanged` 事件。
- 串口记录包含电池模型、`free_heap` 与 `Power_Task` high water mark。
- 文档如实记录未验证项：完整 UI 真机移植、真实低功耗电流、触摸 / BMA / 心率 / 血氧接入。

## 和 PC 模拟器的关系

PC LVGL 模拟器仍是 UI 行为、页面回归和快速开发的主环境。

但从阶段 9 开始，凡是宣称“硬件桥接完成”的卡片，必须提供真机交叉编译和运行证据。PC replay 只能作为辅助对照，不能替代真实 MCU 上的 Flash / SRAM / FreeRTOS 任务栈验证。

## 后续窗口阅读入口

进入阶段 9 时优先读：

- `docs/70_hardware_reference/stage9_hardware_bridge_plan.md`
- `docs/10_architecture/hardware_boundary.md`
- `docs/70_hardware_reference/twatch_s3_plus_reference_validation_run.md`
