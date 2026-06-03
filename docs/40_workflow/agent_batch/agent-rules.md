# Agent 批次执行规则

本文件定义 Magic Watch 卡片批次工作流的固定执行规则。

配套文件：

- `docs/40_workflow/agent_batch/agent-plan.md`：任务卡片清单，卡片状态以此为权威源。
- `docs/40_workflow/agent_batch/agent-progress.md`：运行时进度，只记录当前批次、自动提交策略、不要重复做和会话历史。
- `docs/40_workflow/agent_batch/agent-inbox.md`：规划窗口或并行窗口的新卡片草稿池。

## 基本原则

- 只执行 `agent-progress.md` 中「当前批次」对应的 TODO 卡片。
- 按 `agent-plan.md` 中的卡片顺序执行。
- 不重新设计方案，不重构无关代码，不改变接口、数据结构和现有业务逻辑。
- 不删除现有功能，不引入第三方依赖，除非卡片明确要求。
- 卡片状态以 `agent-plan.md` 为权威源，`agent-progress.md` 不镜像状态表。
- 新需求可以进入 `agent-inbox.md`，但不得干扰正在执行的当前批次。

## 启动协议

每次执行会话开始前必须按顺序完成：

1. 运行 `git status --short -uall`。
2. 如果工作区不干净，且脏文件不在当前批次明确允许范围内，立即停止。
3. 完整读取 `agent-rules.md`、`agent-plan.md`、`agent-progress.md`。
4. 读取 `agent-progress.md` 中「不要重复做」清单。
5. 以「当前批次」为本轮范围，输出本轮将执行的 TODO 卡片列表。
6. 在「会话历史」末尾追加新的 `### 会话 YYYY-MM-DD HH:mm` 块，本轮记录只写入该块。

## 批次推进协议

当当前批次的所有卡片状态都为 DONE，或 BLOCKED 已被用户确认：

1. 将 `agent-progress.md` 中「当前批次」的值加 1。
2. 在当前会话块中记录批次推进确认。
3. 停止执行，等待用户决定是否进入下一批次。

不得自动开始下一批次。

如果「每批最多自动完成卡片数」已达到，即使当前批次仍有 TODO，也必须停止等待用户验收。

## 卡片状态

每张卡片只能使用以下状态之一：

- TODO：未开始。
- IN_PROGRESS：正在执行。
- DONE：已完成。
- BLOCKED：无法安全继续。

## 卡片字段规范

每张卡片必须包含：

- 批次
- 状态
- 依赖
- 自检
- 建议提交信息
- Doc Impact
- 问题定位
- 实施方案
- 涉及位置
- 风险
- 验收标准
- 执行记录

`涉及位置` 必须包含：

- Allowed files
- Read-only files
- Forbidden changes

字段缺失视为卡片不合格，标记 BLOCKED 并提示用户补全。

## 每张卡片的执行流程

1. 阅读卡片所有字段，特别注意依赖、风险、Allowed files 和 Forbidden changes。
2. 若依赖卡片不是 DONE，标记 BLOCKED 或跳过并说明原因。
3. 在 `agent-plan.md` 中将卡片状态改为 IN_PROGRESS。
4. 只在 Allowed files 范围内做最小改动。
5. 如果必须超出 Allowed files，停止并标记 BLOCKED。
6. 执行卡片自检。
7. 代码卡默认至少执行 `git status --short -uall`、`cmake --build sim/lv_port_pc_vscode/build --config Debug`、`git diff --check`，除非卡片明确写明不构建。
8. 文档卡默认执行 `git diff --check`。
9. 修改中文文档后，对本轮实际改动过的中文文件执行 `AGENTS.md` 中规定的乱码哨兵检查。
10. 验收通过后将状态改为 DONE；无法在最小范围内修复则改为 BLOCKED。
11. 在执行记录中填写完成时间、实际改动文件、自检结果和风险回应。
12. 在当前会话块追加卡片摘要。
13. 若 `agent-progress.md` 中「允许自动提交」为「是」，每张 DONE 卡片单独提交一次。

## 并行规划规则

当执行窗口正在运行时，另一个窗口可以做规划，但必须遵守：

- 只向 `agent-inbox.md` 追加新需求或草稿卡片。
- 或只向 `agent-plan.md` 追加未来批次卡片。
- 不修改当前批次卡片。
- 不修改 `agent-progress.md` 的当前批次、当前会话块或历史会话块。
- 若必须调整当前批次，先让执行窗口停止并重新读取三份运行文件。

## 停止条件

遇到以下任一情况立即停止：

- 当前批次完成并已按协议推进。
- 达到每批最多自动完成卡片数。
- 卡片自检失败且无法在 Allowed files 内修复。
- 需要修改接口、数据结构或核心业务逻辑，但卡片未明确要求。
- 卡片字段不合格。
- 依赖卡片未完成。
- 涉及位置不存在。

## 输出要求

每次执行会话结束时必须输出：

- 本次完成或 BLOCKED 的卡片 ID。
- 实际修改的文件列表。
- 自检命令及结果。
- 手动验证摘要，若未执行则明确写未执行。
- 阻塞原因，若有。
- 下一步建议：继续当前批次、触发批次推进或等待用户决策。
