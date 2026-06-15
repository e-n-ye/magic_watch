# Agent 执行队列

本文件是目标模式执行入口，用于把当前要跑的卡片或卡片包排成队列。

`agent-plan.md` 仍可承载短小的内联卡片；较长任务建议放入 `cards/` 子目录，再由本文件引用。

## 当前队列

### V0.0-F411-DMA-CLOSE DMA 小闭环收口

- 状态：DONE
- 卡片来源：cards/f411-lvgl-perf-q3.md
- 执行范围：卡片 `F411-LVGL-DMA-3`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`F411-LVGL-DMA-2` 已由用户真机确认正常显示；本队列项只记录 DMA 前后指标和稳定性，不继续修改 DMA 代码。

### V0.1-F411-REFRESH-DIAG 刷新率诊断

- 状态：DONE
- 卡片来源：cards/v0-f411-refresh-q4.md
- 执行范围：卡片 `V0.1-A` / `V0.1-B` / `V0.1-C` / `V0.1-D` / `V0.1-E`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`V0.0-F411-DMA-CLOSE` 完成并验收；本队列项只做观测和瓶颈分类，禁止优化和重构。

### V0.2-F411-REFRESH-OPT 减少无效刷新

- 状态：DONE
- 卡片来源：cards/v0-f411-refresh-q4.md
- 执行范围：卡片 `V0.2-A` / `V0.2-B` / `V0.2-C`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`V0.1-F411-REFRESH-DIAG` 已输出可复现瓶颈判断；本队列项优先减少工作量，不追 DMA。

### V0.3-F411-XML-PROBE F411 XML 兼容性探针

- 状态：DONE
- 卡片来源：cards/v0-f411-xml-probe-q5.md
- 执行范围：卡片 `V0.3-A` / `V0.3-B` / `V0.3-C` / `V0.3-D`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`V0.2-F411-REFRESH-OPT` 完成或明确无需继续优化；PC 健康四卡闭环已验收；F411 LVGL 基线已稳定。
- 结论：命中 `runtime XML / PNG-FS / 大改生成代码` 停止条件；不得直接进入原定义 `V0.4 F411 XML 真机最小闭环`。

### V0.4R-F411-LITE-UI F411 Lite UI 垂直闭环

- 状态：DONE
- 卡片来源：cards/v0-f411-lite-ui-q6.md
- 执行范围：卡片 `V0.4R-B` / `V0.4R-C` / `V0.4R-D` / `V0.4R-D2` / `V0.4R-D2B` / `V0.4R-D3` / `V0.4R-E`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`V0.3-F411-XML-PROBE` 已完成并确认原定义 `V0.4` 被 `V0.4R` 替代；本队列项固定共享 `UiEvent`、Coordinator、`PageIntent`、`UiModelSnapshot` 和行为语义，不要求 F411 消费 PC 当前这份 LVGL 9.6 生成 C。

### V0.5-WATCH-CORE-GUARDRAILS 共享合同护栏与 F411 所有权收口

- 状态：DONE
- 卡片来源：cards/v0-watch-core-q7.md
- 执行范围：卡片 `V0.5-P0-A` / `V0.5-P0-B` / `V0.5-P0-C`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`V0.4R-F411-LITE-UI` 已完成；`V0.5-P0-A`、`V0.5-P0-B`、`V0.5-P0-C` 均已完成并各自验收；当前阶段结束于“共享合同护栏建立 + F411 所有权收口 + 旧主链退出当前 target”。

### V0.5-P1-NAV-CONTRACT 共享导航合同澄清

- 状态：DONE
- 卡片来源：cards/v0-navigation-contract-q8.md
- 执行范围：卡片 `V0.5-P1-A` / `V0.5-P1-B` / `V0.5-P1-C`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`V0.5-WATCH-CORE-GUARDRAILS` 已完成；进入本队列项时，必须先解决“当前页面状态、页面跳转意图、Adapter 消费规则”三者的共享合同，再决定是否需要 `ScreenId` 或其他后续语义。

### V0.5-P2-EVENT-DRAIN 共享事件处理完成语义

- 状态：DONE
- 卡片来源：cards/v0-event-drain-q9.md
- 执行范围：卡片 `V0.5-P2-A` / `V0.5-P2-B`
- 停止策略：当前队列连续执行
- 验收后处理：保留
- 执行前条件：`V0.5-P1-NAV-CONTRACT` 已完成；进入本队列项时，必须先修正 pending drain 的完成判定，再决定 V0.5 是否可以退出到 Power 语义规划。

### V0.5-P3-POWER-SEMANTICS Power / Wake / Screen On 共享语义决策

- 状态：DONE
- 卡片来源：cards/v0-power-semantics-q10.md
- 执行范围：卡片 `V0.5-P3-A`
- 停止策略：队列项完成后停止
- 验收后处理：保留
- 执行前条件：`V0.5-P2-EVENT-DRAIN` 已完成并经用户验收；本队列项只形成 Power 共享语义决策和下一实现卡边界，不修改代码，不进入 V0.6 平台执行。

### V0.5-P3-POWER-CONTRACT 共享 Power 合同实现

- 状态：DONE
- 卡片来源：cards/v0-power-contract-close-q11.md
- 执行范围：卡片 `V0.5-P3-B`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`V0.5-P3-POWER-SEMANTICS` 已完成并验收；本队列项只实现 `watch_core` 内的最小 Power 决策合同与纯 PC 测试，不接 Adapter、背光、输入门控或 V0.6 平台执行。

### V0.5-P3-CORE-AUDIT Core 所有权复核与退出门

- 状态：DONE
- 卡片来源：cards/v0-power-contract-close-q11.md
- 执行范围：卡片 `V0.5-P3-C`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`V0.5-P3-POWER-CONTRACT` 已完成并经用户回填 F411 编译与原 UI 冒烟；本队列项只做审计与退出门判断，不修改代码。

### V0.5-P4-LEARNING-CLOSE 学习沉淀与 V0.6 交接

- 状态：DONE
- 卡片来源：cards/v0-power-contract-close-q11.md
- 执行范围：卡片 `V0.5-P4-A`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`V0.5-P3-CORE-AUDIT` 已完成并验收；本队列项只收口学习沉淀、路线修正与 V0.6 交界，不进入实现。

### ARCH-CONTROL-LEARNING 架构掌控力恢复学习轮

- 状态：PAUSED
- 卡片来源：cards/architecture-control-learning-q12.md
- 执行范围：卡片 `LEARN-A` / `LEARN-B` / `LEARN-C` / `LEARN-D`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 暂停原因：逐函数学习最终架构不能提供“需求压力 -> 方案选择 -> 代价观察”的完整经验，当前方向改为独立 PC 架构冲突实验场。
- 执行门：不得继续自动执行 `LEARN-A~D`；后续只允许先按 `docs/20_guides/watch_arch_learning_lab.md` 单独规划 Stage 00，且不得提前创建后续 Stage。

### WATCH-ARCH-LAB-STAGE-00 直接调用基线

- 状态：DONE
- 卡片来源：cards/watch-arch-learning-stage00-q13.md
- 执行范围：卡片 `ARCH-LAB-00`
- 停止策略：队列项完成后停止
- 验收后处理：冻结并保留
- 执行前条件：实验场总纲已由提交 `cfca90c` 收口；只允许创建 Stage 00，不得自动推进后续 Stage。

### WATCH-ARCH-LAB-STAGE-01 Console 直接调用基线

- 状态：DONE
- 卡片来源：cards/watch-arch-learning-stage01-q14.md
- 执行范围：卡片 `ARCH-LAB-01`
- 停止策略：队列项完成后停止
- 验收后处理：冻结并保留
- 执行前条件：`ARCH-LAB-00` 已由提交 `c32957b` 冻结；本卡只建立 Console 直接调用骨架，不得自动引入系统抽象或推进 Stage 02。

### WATCH-ARCH-LAB-STAGE-02 上下文相关菜单导航

- 状态：DONE
- 卡片来源：cards/watch-arch-learning-stage02-q15.md
- 执行范围：卡片 `ARCH-LAB-02`
- 停止策略：队列项完成后停止
- 验收后处理：冻结并保留
- 执行前条件：`ARCH-LAB-01` 已由提交 `6ce2806` 冻结；先完成状态与导航预测，不得直接创建 Stage 02 代码。

### WATCH-ARCH-LAB-STAGE-03 第二种输入表示

- 状态：DONE
- 卡片来源：cards/watch-arch-learning-stage03-q16.md
- 执行范围：卡片 `ARCH-LAB-03`
- 停止策略：第一版实现后等待用户运行与复盘
- 验收后处理：冻结并保留
- 执行前条件：`ARCH-LAB-02` 已由提交 `ee08027` 冻结；先比较两种直接接法，不得提前引入 Intent、Input Adapter 或 EventQueue。

### WATCH-ARCH-LAB-STAGE-04 命令字符串压力

- 状态：DONE
- 卡片来源：cards/watch-arch-learning-stage04-q17.md
- 执行范围：卡片 `ARCH-LAB-04`
- 停止策略：第一版实现后等待用户运行与复盘
- 验收后处理：保留
- 执行前条件：`ARCH-LAB-03` 已冻结；本轮只攻击“命令字符串是否已变成隐式内部合同”，不得引入异步、队列、Power 或共享动作枚举。

### WATCH-ARCH-LAB-STAGE-05 共享动作语义规划

- 状态：DONE
- 卡片来源：cards/watch-arch-learning-stage05-q18.md
- 执行范围：卡片 `ARCH-LAB-05`
- 停止策略：第一版实现后等待用户运行与复盘
- 验收后处理：保留
- 执行前条件：`ARCH-LAB-04` 已冻结；本轮只验证“最小共享动作语义是否值得引入”，不得引入异步、队列、更多页面或 Power。

### WATCH-ARCH-LAB-STAGE-06 时间维度与系统事件压力

- 状态：DONE
- 卡片来源：cards/watch-arch-learning-stage06-q19.md
- 执行范围：卡片 `ARCH-LAB-06`
- 停止策略：队列项完成后停止
- 验收后处理：保留
- 执行前条件：`ARCH-LAB-05` 已冻结；本轮只验证“时间维度、系统事件与 dirty render”的最小闭环，不引入目标导航层、线程或队列。
- 结论：事件分层和 dirty 状态压力已经成立，但当前仍是阻塞输入驱动的同步解释器；不继续 Stage 06B，转入真实 PC 工程架构审查。

### CURRENT-WATCH-ARCH-AUDIT-PC PC XML 主链架构地图与必要性审查

- 状态：DONE
- 卡片来源：cards/current-watch-architecture-audit-q20.md
- 执行范围：卡片 `ARCH-AUDIT-PC-01`
- 停止策略：队列项完成后停止
- 验收后处理：保留
- 执行前条件：`ARCH-LAB-06` 已收口；只读审计当前 `magic_watch_xml_sim`，
  不修改产品代码，不把旧 C++ 模拟器混入当前主线。
- 结论：当前必要边界是 Platform Runtime / XML View / UI Adapter /
  `watch_core`；Service、EventBus 和独立 ScreenManager 尚无实现证据。

### MAINLINE-DATA-01 动态电池最小闭环

- 状态：TODO
- 卡片来源：cards/mainline-data-q21.md
- 执行范围：卡片 `DATA-01-A` / `DATA-01-B` / `DATA-01-C` / `DATA-01-D`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`CURRENT-WATCH-ARCH-AUDIT-PC` 已完成；当前唯一执行入口是
  本队列项。允许执行四张子卡，但不得登记或执行能力路线阶段 2-4 卡片。

## 队列项格式

```text
### <队列项 ID> <标题>
- 状态：TODO / IN_PROGRESS / DONE / BLOCKED
- 卡片来源：agent-plan.md / cards/<file>.md
- 执行范围：批次 N / 卡片 ID 列表 / 全文件
- 停止策略：队列项完成后停止 / 当前队列连续执行 / 每张卡后停止
- 验收后处理：保留 / 从队列移除 / 删除卡片文件
```

## 使用说明

- 执行层优先读取本文件。
- 若「当前队列」为空，则回退到 `agent-progress.md` 的当前批次和 `agent-plan.md`。
- 一个队列项可以指向单张卡片，也可以指向同一轮可连续执行的卡片包。
- 卡片包完成后是否停下，由队列项的「停止策略」决定。
- 删除卡片文件只能在用户验收后执行，且必须一次删除一个明确路径。
