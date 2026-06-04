# Agent 执行队列

本文件是目标模式执行入口，用于把当前要跑的卡片或卡片包排成队列。

`agent-plan.md` 仍可承载短小的内联卡片；较长任务建议放入 `cards/` 子目录，再由本文件引用。

## 当前队列

### DOCROOT-Q8-GUIDES docs 根目录第二轮开发指南迁移

- 状态：DONE
- 卡片来源：cards/docroot-reorg-wave2-guides.md
- 执行范围：全文件
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除；用户确认后可删除卡片文件

### DOCROOT-Q9-HISTORY docs 根目录第二轮历史归档迁移

- 状态：DONE
- 卡片来源：cards/docroot-reorg-wave2-history.md
- 执行范围：全文件
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除；用户确认后可删除卡片文件

### DOCROOT-Q12-REGRESSION-MATRIX docs 测试矩阵目录归属审计

- 状态：DONE
- 卡片来源：cards/docroot-reorg-wave3-decisions.md
- 执行范围：卡片 `DOCROOT-B10-C03`
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除；用户确认后可删除卡片文件

### DOCROOT-Q10-CURRENT-SIM-ARCH docs current_sim_architecture 根目录归属审计

- 状态：TODO
- 卡片来源：cards/docroot-reorg-wave3-decisions.md
- 执行范围：卡片 `DOCROOT-B10-C01`
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除；用户确认后可删除卡片文件

### DOCROOT-Q11-DECISION-LOG docs decision_log 根目录归属审计

- 状态：TODO
- 卡片来源：cards/docroot-reorg-wave3-decisions.md
- 执行范围：卡片 `DOCROOT-B10-C02`
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除；用户确认后可删除卡片文件

### DOCROOT-Q13-DOCS-ROOT-REVIEW docs 根目录清洁度最终复核

- 状态：TODO
- 卡片来源：cards/docroot-reorg-wave3-decisions.md
- 执行范围：卡片 `DOCROOT-B10-C04`
- 停止策略：队列项完成后停止
- 验收后处理：从队列移除；用户确认后可删除卡片文件

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
