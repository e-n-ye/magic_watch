# Magic Watch 文档任务路由器

日期：2026-06-02

本文件不再是“推荐阅读清单”，而是任务路由器。默认上下文必须短；历史文档可以检索，但不能污染每次新会话。

## 默认新会话

默认只读：

1. `AGENTS.md`
2. [document_map.md](/D:/MY_Desk/watch/magic_watch/docs/document_map.md)
3. [project_brief.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/project_brief.md)
4. [current_decisions.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_decisions.md)

如果时间很紧，最少读：

- [document_map.md](/D:/MY_Desk/watch/magic_watch/docs/document_map.md)
- [project_brief.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/project_brief.md)
- [current_decisions.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_decisions.md)

默认入口只回答三件事：

- 项目当前为什么继续做
- 当前哪些决策仍有效
- 遇到具体任务时下一步该补读什么

默认新会话不要一开始通读长事实文档、长拆分状态文档或长回归清单。

## 当前默认不读

以下文档不再作为默认入口：

- [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/project_charter.md): 旧阶段项目宪章，已由 `00_current/project_brief.md` 接管当前定位。
- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_refactor_status.md): 当前代码事实基线，内容较长，只在需要确认“代码实际拆到哪”时补读。
- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md): 架构主线入口，不是每次新会话都必须先读。
- [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/v0_scope.md): 当前范围摘要，在需要确认阶段边界时补读。
- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md): 历史决策流水账，只作历史检索。
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/page_reachability_audit.md): 历史页面可达性审计，只在查旧入口、旧页面或历史判断时选读。

## 当前代码事实补读

- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_refactor_status.md): 当前代码重构落点总览。

读取时机：

- 需要确认各页面域、helper、聚合头和构建登记的真实现状
- 需要确认某个页面是否已迁出 `ShellPages.cpp`
- 需要确认当前文档是否反映已提交代码事实

定位说明：

- 它是当前代码事实基线，不是默认必读。
- 它记录已提交事实，不记录未提交中间态。
- 读完它后，再按任务进入页面拆分、测试回归或架构专题文档。

## 按任务选读

### 我想知道当前项目是什么

先读：

- [project_brief.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/project_brief.md)
- [current_decisions.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_decisions.md)
- [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/v0_scope.md)，仅在需要确认当前阶段边界时补读。

### 我想知道当前重构到哪了

先读：

- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_refactor_status.md)
- [ui_page_split_status.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/ui_page_split_status.md)

### 我想继续收口 Shell 聚合头 / 注册结构

- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)
- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_refactor_status.md)
- [ui_page_split_status.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/ui_page_split_status.md)

### 我想新增 UI 页面

先读：

- [ui_page_ownership.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/ui_page_ownership.md)
- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)
- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_refactor_status.md)，仅在需要确认当前实现文件归属时补读。

### 我想判断页面归属

先读：

- [ui_page_ownership.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/ui_page_ownership.md)
- [ui_page_split_status.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/ui_page_split_status.md)

### 我想继续 HeartRate 测量流

先读：

- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_refactor_status.md)
- [ui_page_split_status.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/ui_page_split_status.md)

说明：

- HeartRate 后续风险重点已经不是继续从 `ShellPages.cpp` 迁页面。
- 当前更需要关注 `HeartRateAppPage` 内部 measurement timer、stage 切换和 wear prompt 生命周期边界。

### 我想做手动 UI 回归

先读：

- [manual_ui_regression_checklist.md](/D:/MY_Desk/watch/magic_watch/docs/30_testing/manual_ui_regression_checklist.md)
- [simulator_manual_regression_matrix.md](/D:/MY_Desk/watch/magic_watch/docs/30_testing/simulator_manual_regression_matrix.md)，仅在需要完整行为矩阵时补读。
- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_refactor_status.md)，仅在需要把回归包映射到当前结构风险时补读。

说明：

- 构建通过不等于手动 UI 通过。
- 未执行的手动 UI 不能写成通过。
- 完整矩阵与更短回归清单都归入 `docs/30_testing/`，避免测试入口继续散落在 docs 根目录。

### 我想查视觉探索资料

先读：

- [60_visual_exploration/](/D:/MY_Desk/watch/magic_watch/docs/60_visual_exploration)
- [settings_visual_reference.md](/D:/MY_Desk/watch/magic_watch/docs/60_visual_exploration/settings_visual_reference.md)

说明：

- 视觉方向稿、视觉参考和视觉流程文档已迁入 `docs/60_visual_exploration/`，属于按需视觉资料，不回到默认新会话入口。
- `watchface_style_architecture.md` 仍按架构专题保留在根目录，不并入视觉探索目录。

### 我想使用卡片批次 / 目标模式执行

先读：

- [agent-rules.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-rules.md)
- [agent-queue.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-queue.md)
- [agent-plan.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-plan.md)
- [agent-progress.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-progress.md)
- [agent-inbox.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-inbox.md)，仅在需要并行规划或追加新需求卡片时读取。

说明：

- 这是按需执行流程，不是默认新会话入口。
- `agent-queue.md` 是目标模式执行入口；为空时回退到 `agent-progress.md` 当前批次和 `agent-plan.md`。
- `agent-plan.md` 是内联卡片状态权威源；卡片文件中的卡片以对应文件为权威源；`agent-progress.md` 只记录当前批次、自动提交策略、不要重复做和会话历史。
- 执行窗口只跑当前批次；并行规划窗口只能追加未来批次或写入 `agent-inbox.md`。

### 我想写简历项目描述

先读：

- [project_story_notes.md](/D:/MY_Desk/watch/magic_watch/docs/90_resume/project_story_notes.md)

说明：

- 这里只整理项目叙事素材，不影响工程决策。
- 它不是默认工程必读。
- 后续如需继续整理简历，可再结合 `D:/MY_Desk/watch/jianli`，但这不属于当前工程默认上下文。

### 我想理解 `DataCenter / EventBus / AppStateMachine / PageManager`

先读：

- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)
- [current_decisions.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_decisions.md)

### 我想规划未来真实硬件接入，但不想提前绑定芯片或 RTOS

先读：

- [hardware_boundary.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/hardware_boundary.md)
- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)，仅在需要确认当前协调层与页面边界时补读。

说明：

- 这是按需专题文档，不加入默认新会话必读。
- 它只定义 ISR、Service、DataCenter、EventBus、Coordinator、UI 与 Driver/BSP/HAL 的职责边界。
- 它不绑定芯片、板卡、RTOS 或硬件代码骨架。

### 我想查历史硬件参考

先读：

- [70_hardware_reference/](/D:/MY_Desk/watch/magic_watch/docs/70_hardware_reference)
- [hardware_frontier_entry.md](/D:/MY_Desk/watch/magic_watch/docs/70_hardware_reference/hardware_frontier_entry.md)

说明：

- 硬件候选、选型策略和 TWatch 参考验证资料已迁入 `docs/70_hardware_reference/`，属于未来硬件接入前的按需参考材料。
- 当前硬件边界与系统职责约束仍以 [hardware_boundary.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/hardware_boundary.md) 为准。

### 我想查 v0 历史阶段记录

先读：

- [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/50_stage_records/v0_scope.md)
- [50_stage_records/](/D:/MY_Desk/watch/magic_watch/docs/50_stage_records)

说明：

- 这批 `v0_*` 文档已迁入 `docs/50_stage_records/`，属于历史阶段记录，不回到默认新会话入口。
- 需要看当前范围时，优先读 [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/v0_scope.md)。

### 我想查历史决策

先读：

- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)

说明：

- 它是历史检索材料，不应回到默认入口。
- 读取时只检索相关日期或关键词，不默认通读全文。

### 我想看 docs 根目录剩余文档的二次评审

先读：

- [docs_root_reorg_inventory.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/docs_root_reorg_inventory.md)

说明：

- `DOCROOT-B6-C01` 已把 Wave 1 迁移后的剩余根目录文档分成三类：明确保留根目录、第二轮候选迁移、仍需单独决策。
- `cpp_minimal_glossary.md`、`learning_plan.md`、`simulator_reading_guide.md` 已迁入 [20_guides/](/D:/MY_Desk/watch/magic_watch/docs/20_guides)；`project_charter.md`、`prototype_progress.md`、`page_reachability_audit.md`、`simulator_manual_regression_run_2026_05_19.md` 已迁入 [80_history/](/D:/MY_Desk/watch/magic_watch/docs/80_history)。

### 我想查历史归档文档

先读：

- [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/project_charter.md)
- [prototype_progress.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/prototype_progress.md)
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/page_reachability_audit.md)
- [simulator_manual_regression_run_2026_05_19.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/simulator_manual_regression_run_2026_05_19.md)

说明：

- 这四份文档属于历史归档/历史检索入口，不回到默认新会话入口。
- 它们分别服务于旧阶段定位、原型推进记录、历史页面可达性判断和一次性手动回归运行记录的追溯。

### 我想补读开发指南

先读：

- [cpp_minimal_glossary.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/cpp_minimal_glossary.md)
- [learning_plan.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/learning_plan.md)
- [simulator_reading_guide.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/simulator_reading_guide.md)

说明：

- 这三份文档属于按需阅读的开发指南，不回到默认新会话入口。
- 它们分别服务于 C++ 代码阅读补盲、学习路径规划和模拟器主线代码导读。

## 文档维护规则

- 新增文档必须说明它属于当前入口、架构专题、开发指南、审计资料还是历史检索。
- 当前入口文档应短、稳定、面向新会话。
- 历史文档保留价值，但不得默认进入上下文。
- 中文文档修改后，对本轮实际改动的中文文件做乱码哨兵检查。
