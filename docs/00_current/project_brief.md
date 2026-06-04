# Magic Watch Current Project Brief

日期：2026-06-04

## 当前定位

Magic Watch 当前不是以继续堆智能手表页面为目标。

当前阶段目标从继续证明模拟器或参考板，转向抽取一份可迁移、可亲手实现的 F411 / C-first 架构骨架。

v0 的成功标准不是页面数量，而是：

1. 新增一个功能时，修改范围可预测。
2. 输入、服务、状态机、页面之间职责清楚。
3. 页面不直接触碰硬件。
4. `PageManager` 不做业务判断。
5. 真实硬件接入时，上层 UI、状态模型和服务边界尽量不重写。
6. 文档入口足够短，历史资料可检索但不污染默认上下文。

## 当前阶段

阶段 8 架构收口已经完成，阶段 9 的 T-Watch Battery 垂直切片留下了真实 MCU 上最小架构子集的参考价值。

当前新的主线是“F411 架构蓝图重启”：把 Magic Watch 中真正有价值的分层思想剥离出来，用 C-first 的方式重新想象一个未来可以亲手实现的 STM32F411 手表骨架。

本阶段优先证明一件事：

即使不搬当前 C++ 模拟器代码，也能清楚说出未来 F411 项目的层次、数据流、事件流、页面流和第一批 C 模块该如何生长。

当前蓝图入口：

- `docs/10_architecture/f411_future_watch_architecture.md`
- `docs/10_architecture/f411_future_watch_architecture_uml.html`

## 当前不做

- 不推倒重写模拟器。
- 不继续盲目填充页面。
- 不把 T-Watch / AXP2101 路线当作 F411 最终方案。
- 不把模拟器 C++ 架构代码原样搬进 F411。
- 不让 AI 一口气生成完整工程骨架。
- 不把 PC trace replay / log replay 当作阶段 9 硬件闭环验收。
- 不只读取 AXP2101 PMU log 就宣称硬件桥接完成。
- 不宣称完整 Magic Watch UI 已经移植到真机。
- 不在真实硬件选型未完成前绑定最终芯片、板卡或 RTOS。
- 不在 Controller 中直接操作 UI、`PageManager` 或 LVGL 对象。

## 当前优先级

1. 维护 F411 / C-first 架构蓝图入口。
2. 把 Magic Watch 中可保留的思想翻译成 C 模块边界：Input、Service、ModelStore、EventQueue、Coordinator、Screen Manager、UI Adapter。
3. 明确哪些东西不照搬：模拟器 LVGL 页面代码、C++ 通用 EventBus、动态容器、全量 DataCenter、T-Watch/AXP2101 绑定。
4. 后续用小卡片在 F411 项目中逐步长出最小 C 骨架，而不是一次性生成完整工程。
5. 阶段 9 资料只作为真实 MCU 下放经验参考，不作为当前默认执行主线。

## 本阶段完成标准

本阶段完成不以新增页面数量、文件变短或 AI 生成代码量为标准，而以“人能否掌控架构”为标准：

1. 新会话只读 `AGENTS.md`、`docs/document_map.md` 和 3-5 个当前文档即可理解项目方向。
2. 每轮开发都有明确 Scope Lock，实际 diff 不越过本轮允许文件。
3. 能用 F411 蓝图说清每一层负责什么、不负责什么。
4. 能判断一个新功能应该进入 Driver、Service、ModelStore、EventQueue、Coordinator、Screen 还是 UI Adapter。
5. 后续第一批 C 模块能按蓝图小步生长，而不是复制模拟器实现。
6. T-Watch / 阶段 9 经验被保留为参考，不重新夺走当前 F411 主线。
