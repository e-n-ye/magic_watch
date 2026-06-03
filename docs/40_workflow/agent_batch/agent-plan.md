# Agent 任务计划

本文件是任务卡片清单，卡片状态以本文件为权威源。

执行规则见：`docs/40_workflow/agent_batch/agent-rules.md`。
运行时进度见：`docs/40_workflow/agent_batch/agent-progress.md`。
并行规划草稿见：`docs/40_workflow/agent_batch/agent-inbox.md`。

---

## 8E-B1-C01 硬件接入边界文档契约

- 批次：1
- 状态：DONE
- 依赖：无
- 自检：`git diff --check`；对本卡改动中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查
- 建议提交信息：`docs: define hardware boundary contract`
- Doc Impact：required

### 问题定位

阶段 8E 需要把未来真实硬件接入的上层边界写成文档契约，避免 UI、EventBus、DataCenter、Service、Driver/BSP/HAL 的职责在后续硬件化时重新混在一起。当前项目还未做真实硬件选型，因此本卡只收口文档，不新增硬件代码骨架。

### 实施方案

1. 新增或更新硬件边界专题文档，明确 ISR、Service、EventBus、DataCenter、UI 页面和 Driver/BSP/HAL 的职责边界。
2. 在当前架构或范围文档中补充最小必要引用，使新会话能按需找到该契约。
3. 在 `docs/document_map.md` 中把硬件边界文档登记为按需路由，不加入默认必读。

### 涉及位置

Allowed files:

- `docs/00_current/current_architecture.md`
- `docs/00_current/v0_scope.md`
- `docs/10_architecture/state_machine.md`
- `docs/10_architecture/hardware_boundary.md`
- `docs/document_map.md`

Read-only files:

- `docs/00_current/current_decisions.md`
- `docs/00_current/current_refactor_status.md`

Forbidden changes:

- 不新增硬件代码骨架。
- 不绑定芯片、RTOS 或具体板卡。
- 不修改 `sim/**`。
- 不把手动 UI 回归写成已通过。

### 风险

- 边界写得过细会提前绑定未定硬件路线；对策是只写职责和约束，不写芯片、板卡或 RTOS 细节。
- EventBus 当前是同步分发；对策是明确接 RTOS 前必须定义队列、线程归属和 snapshot 边界。
- UI 页面可能被误写成可访问 HAL；对策是明确 UI 只消费模型和事件，不访问 Driver/BSP/HAL。

### 验收标准

- [ ] 文档明确 ISR 不直接改 UI，不直接 publish UI 事件。
- [ ] 文档明确硬件采样进入 Service，由 Service 聚合、去抖、限频后写入 DataCenter 或发布事件。
- [ ] 文档明确 EventBus 当前同步分发，接 RTOS 前必须定义队列、线程归属和 snapshot 边界。
- [ ] 文档明确 DataCenter 不暴露可变裸引用。
- [ ] 文档明确 UI 页面只消费模型和事件，不访问 Driver/BSP/HAL。
- [ ] 文档能回答“未来换真实硬件时哪些上层代码不应重写”。
- [ ] 未新增硬件代码骨架，未绑定芯片、RTOS 或具体板卡。

### 执行记录

- 完成时间：2026-06-03（本轮会话）
- 实际改动文件：
  - `docs/00_current/current_architecture.md`
  - `docs/00_current/v0_scope.md`
  - `docs/10_architecture/state_machine.md`
  - `docs/10_architecture/hardware_boundary.md`
  - `docs/document_map.md`
- 自检结果：
  - `git diff --check` 通过
  - 中文文档乱码哨兵检查通过，未命中本轮改动文件
- 风险回应：
  - 文档只定义职责边界与同步约束，未绑定芯片、板卡、RTOS 或硬件代码骨架
  - 明确 UI 不访问 `Driver/BSP/HAL`，并补充同步 `EventBus` 在真实硬件化前必须先定义队列、线程归属与 snapshot 边界

---

## 8F-B1-C02 手动 UI 回归清单重排

- 批次：1
- 状态：DONE
- 依赖：8E-B1-C01
- 自检：`git diff --check`；对本卡改动中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查
- 建议提交信息：`docs: refresh manual ui regression checklist`
- Doc Impact：required

### 问题定位

阶段 8F 需要把手动 UI 回归从“构建通过”的影子里拆出来，按当前页面结构重排回归包，并明确高风险路径。当前阶段不得把未执行的手动 UI 写成通过。

### 实施方案

1. 更新手动 UI 回归清单，按 Home、Launcher、Notifications、QuickSettings、Power、Daily、Health、Settings 重排。
2. 标记必须手动点的高风险路径，包括 HomeRing crown/preview、Notifications swipe/detail/clear、QuickSettings drag/toggle/timer、Power screen-off/restore。
3. 在当前状态文档中只补充“回归清单已重排 / 手动 UI 未执行”的最小事实。

### 涉及位置

Allowed files:

- `docs/30_testing/manual_ui_regression_checklist.md`
- `docs/00_current/current_refactor_status.md`

Read-only files:

- `docs/document_map.md`
- `docs/10_architecture/ui_page_split_status.md`

Forbidden changes:

- 不宣称手动 UI 已通过。
- 不修改 `sim/**`。
- 不新增自动化测试框架。
- 不把构建通过当作 UI 回归通过。

### 风险

- 清单重排可能遗漏高交互页；对策是按结构域和高风险路径双重覆盖。
- 未执行项可能被误写成通过；对策是所有未执行项保持“未执行”或“待执行”。
- 回归清单过长会降低可执行性；对策是区分必须路径和补充路径。

### 验收标准

- [ ] 回归包按 Home、Launcher、Notifications、QuickSettings、Power、Daily、Health、Settings 重排。
- [ ] 高风险路径至少覆盖 HomeRing crown/preview。
- [ ] 高风险路径至少覆盖 Notifications swipe/detail/clear。
- [ ] 高风险路径至少覆盖 QuickSettings drag/toggle/timer。
- [ ] 高风险路径至少覆盖 Power screen-off/restore。
- [ ] 文档明确构建通过不等于 UI 回归通过。
- [ ] 未执行的手动 UI 项保持未执行状态，没有写成通过。

### 执行记录

- 完成时间：2026-06-03（本轮会话）
- 实际改动文件：
  - `docs/30_testing/manual_ui_regression_checklist.md`
  - `docs/00_current/current_refactor_status.md`
- 自检结果：
  - `git diff --check` 通过
  - 中文文档乱码哨兵检查通过，未命中本轮改动文件
- 风险回应：
  - 清单仅完成结构重排与高风险路径标记，未宣称手动 UI 已执行或已通过
  - 当前状态文档只补充“清单已重排 / 手动 UI 未执行”的最小事实，没有把构建通过写成 UI 回归通过
