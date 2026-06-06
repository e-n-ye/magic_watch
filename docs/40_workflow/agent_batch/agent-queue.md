# Agent 执行队列

本文件是目标模式执行入口，用于把当前要跑的卡片或卡片包排成队列。

`agent-plan.md` 仍可承载短小的内联卡片；较长任务建议放入 `cards/` 子目录，再由本文件引用。

## 当前队列

### LVGL-XML-Q1 健康四卡 XML 与 PC 垂直闭环

- 状态：DONE
- 卡片来源：cards/lvgl-xml-watch-core-q1.md
- 执行范围：卡片 `LVGL-XML-Q1-1` / `LVGL-XML-Q1-2` / `LVGL-XML-Q1-3` / `LVGL-XML-Q1-4` / `LVGL-XML-Q1-5`
- 停止策略：每张卡后停止
- 验收后处理：保留

### F411-Q3 LVGL 性能基线与 SPI DMA 准备

- 状态：IN_PROGRESS
- 卡片来源：cards/f411-lvgl-perf-q3.md
- 执行范围：卡片 `F411-LVGL-PERF-2` / `F411-LVGL-DMA-PREP` / `F411-LVGL-DMA-1` / `F411-LVGL-DMA-2` / `F411-LVGL-DMA-3`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`LVGL-XML-Q1` 已完成并验收；`F411-LVGL-PERF-1` 与 `F411-LVGL-PERF-1B` 已在卡片文件中记录为 DONE。

### F411-XML-Q2 同一套 XML 生成 UI 上 F411

- 状态：TODO
- 卡片来源：cards/lvgl-xml-watch-core-q1.md
- 执行范围：卡片 `F411-XML-Q2-1`
- 停止策略：每张卡后停止
- 验收后处理：保留
- 执行前条件：`LVGL-XML-Q1` 已完成并验收；`F411-Q3` 已完成并验收。

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
