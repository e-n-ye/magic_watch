# Magic Watch Current Project Brief

日期：2026-06-04

## 当前定位

Magic Watch 当前不是以继续堆智能手表页面为目标。

当前阶段目标是把已经收口的可维护、可扩展、事件驱动、可模拟架构，推进到真实硬件上的最小架构子集验证。

v0 的成功标准不是页面数量，而是：

1. 新增一个功能时，修改范围可预测。
2. 输入、服务、状态机、页面之间职责清楚。
3. 页面不直接触碰硬件。
4. `PageManager` 不做业务判断。
5. 真实硬件接入时，上层 UI、状态模型和服务边界尽量不重写。
6. 文档入口足够短，历史资料可检索但不污染默认上下文。

## 当前阶段

阶段 8 架构收口已经完成。当前进入“阶段 9 真实硬件桥接”，暂停继续扩张健康、通知、设置等功能页。

本阶段优先证明一件事：

Magic Watch 的最小架构子集能在 T-Watch S3 Plus 真机上交叉编译并运行，而不是只在 PC 模拟器里处理回放数据。

第一条真实硬件切片固定为 Battery / PowerStatus：

- 以 AXP2101 作为第一真实数据源。
- 以 `hal::BatterySample`、`BatteryPowerService`、`DataCenter` 和简化 `EventBus` 作为第一批下放架构子集。
- 以真实 FreeRTOS `Power_Task`、串口 `BatteryChanged` 事件、`free_heap` 和 task high water mark 作为最小观测闭环。

## 当前不做

- 不推倒重写模拟器。
- 不继续盲目填充页面。
- 不把 PC trace replay / log replay 当作阶段 9 硬件闭环验收。
- 不只读取 AXP2101 PMU log 就宣称硬件桥接完成。
- 不宣称完整 Magic Watch UI 已经移植到真机。
- 不在真实硬件选型未完成前绑定最终芯片、板卡或 RTOS。
- 不在 Controller 中直接操作 UI、`PageManager` 或 LVGL 对象。

## 当前优先级

1. 阶段 9 真实硬件桥接方向落档。
2. T-Watch S3 Plus 上稳定读取 AXP2101 / BatteryPowerStatus。
3. 将最小 `hal::BatterySample`、`BatteryPowerService`、`DataCenter`、简化 `EventBus` 下放到 T-Watch prototype。
4. 在真实 FreeRTOS `Power_Task` 中 1Hz 采样并推入板载 `DataCenter`。
5. 通过串口观察 `BatteryChanged`、`free_heap` 与 task high water mark。
6. 记录哪些上层代码可复用，哪些因动态分配、同步事件或 UI 依赖暂时不能直接下放。

## 本阶段完成标准

本阶段完成不以新增页面数量或文件变短为标准，而以真实硬件桥接证据为标准：

1. 新会话只读 `AGENTS.md`、`docs/document_map.md` 和 3-5 个当前文档即可理解项目方向。
2. 每轮开发都有明确 Scope Lock，实际 diff 不越过本轮允许文件。
3. 阶段 9 卡片不能把 PC replay 写成硬件闭环完成。
4. 至少一轮代码在 T-Watch S3 Plus 上交叉编译并运行 Magic Watch 最小架构子集。
5. 串口能观察到由板载 `EventBus` 发出的 `BatteryChanged` 事件。
6. 真机日志记录 `free_heap` 和 `Power_Task` high water mark，并明确未验证项。
