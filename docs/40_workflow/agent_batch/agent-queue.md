# Agent 执行队列

本文件是目标模式执行入口，用于把当前要跑的卡片或卡片包排成队列。

`agent-plan.md` 仍可承载短小的内联卡片；较长任务建议放入 `cards/` 子目录，再由本文件引用。

## 当前队列

### H9-Q2A T-Watch AXP2101 底层读数闭环

- 状态：DONE
- 卡片来源：cards/h9-hardware-bridge-q2.md
- 执行范围：卡片 `H9-BRIDGE-2A`
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除

### H9-Q2B 最小架构子集下放

- 状态：DONE
- 卡片来源：cards/h9-hardware-bridge-q2.md
- 执行范围：卡片 `H9-BRIDGE-2B`
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除

### H9-Q2C 真实 FreeRTOS Power_Task 闭环

- 状态：TODO
- 卡片来源：cards/h9-hardware-bridge-q2.md
- 执行范围：卡片 `H9-BRIDGE-2C`
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除

### H9-Q2D 串口 BatteryChanged 事件观测

- 状态：TODO
- 卡片来源：cards/h9-hardware-bridge-q2.md
- 执行范围：卡片 `H9-BRIDGE-2D`
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除

### H9-Q2E 复用边界记录

- 状态：TODO
- 卡片来源：cards/h9-hardware-bridge-q2.md
- 执行范围：卡片 `H9-BRIDGE-2E`
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除

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
