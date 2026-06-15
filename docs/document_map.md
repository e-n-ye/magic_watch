# Magic Watch 文档任务路由器

日期：2026-06-07

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
- [magicwatch_long_term_roadmap.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/magicwatch_long_term_roadmap.md): V0-V3 执行路线图，内容较长，只在规划阶段、拆执行卡或判断停止条件时补读。
- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md): 历史决策流水账，只作历史检索；保留根目录稳定路径，避免跨会话检索入口漂移。
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/page_reachability_audit.md): 历史页面可达性审计，只在查旧入口、旧页面或历史判断时选读。
- [current_sim_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/current_sim_architecture.md): 旧版模拟器现状地图，只在追溯早期骨架或核对 `00_current/current_architecture.md` 之前的现状判断时补读。

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
- [lvgl_xml_watch_core_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/lvgl_xml_watch_core_architecture.md)，仅在需要继续 XML / `watch_core` / PC / F411 主线时补读。
- [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/v0_scope.md)，仅在需要确认当前阶段边界时补读。
- [magicwatch_long_term_roadmap.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/magicwatch_long_term_roadmap.md)，仅在需要 V0-V3 执行路线、阶段依赖、量化门槛或停止条件时补读。

### 我想规划 V0-V3 长期路线或拆分下一批执行卡

先读：

- [magicwatch_long_term_roadmap.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/magicwatch_long_term_roadmap.md)
- [current_decisions.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_decisions.md)
- [lvgl_xml_watch_core_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/lvgl_xml_watch_core_architecture.md)，仅在计划涉及 XML / `watch_core` / PC / F411 主线时补读。

说明：

- `magicwatch_long_term_roadmap.md` 是 V0-V3 的当前执行地图，包含阶段依赖图、V0 执行门、量化门槛、任务卡拆分和跨阶段复用规则。
- 它是按需当前计划，不加入默认新会话入口。
- 如果只是问项目当前定位，优先读更短的 `project_brief.md` 和 `current_decisions.md`。

### 我想继续 LVGL XML / watch_core / F411 新主线

先读：

- [lvgl_xml_watch_core_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/lvgl_xml_watch_core_architecture.md)
- [lvgl_xml_watch_core_architecture_uml.html](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/lvgl_xml_watch_core_architecture_uml.html)
- [magicwatch_long_term_roadmap.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/magicwatch_long_term_roadmap.md)，仅在需要确认 V0 阶段依赖、量化指标或停止条件时补读。
- [agent-queue.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-queue.md)
- [lvgl-xml-watch-core-q1.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/cards/lvgl-xml-watch-core-q1.md)

说明：

- 这是当前默认工程主线：XML 作为 PC UI 源，`watch_core` 作为纯 C 产品核心，PC 与 F411 固定共享语义合同，但允许 UI View、LVGL 主版本和资源实现分叉。
- 第一阶段只做主页健康四卡、PC 可交互闭环和 F411 显示链路验证，不迁 Zephyr、不绑定最终芯片、不接完整传感器业务。
- 后续执行顺序以 `agent-queue.md` 为准：`V0.0-F411-DMA-CLOSE` -> `V0.1-F411-REFRESH-DIAG` -> `V0.2-F411-REFRESH-OPT` -> `V0.3-F411-XML-PROBE` -> `V0.4R-F411-LITE-UI`；原定义 `V0.4` 已被 `V0.4R` 替代。
- 执行前如需要锁定 AI 边界，补读 [magicwatch_ai_collaboration_rules.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/magicwatch_ai_collaboration_rules.md)。

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

- [lvgl_xml_watch_core_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/lvgl_xml_watch_core_architecture.md)，如果页面属于新 XML 主线。
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

- [magicwatch_ai_collaboration_rules.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/magicwatch_ai_collaboration_rules.md)
- [agent-rules.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-rules.md)
- [agent-queue.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-queue.md)
- [agent-plan.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-plan.md)
- [agent-progress.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-progress.md)
- [agent-inbox.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/agent_batch/agent-inbox.md)，仅在需要并行规划或追加新需求卡片时读取。

说明：

- 这是按需执行流程，不是默认新会话入口。
- `magicwatch_ai_collaboration_rules.md` 提供 Scope Lock、默认禁止事项、人工验收 checklist 和停止条件；复杂任务或多模块任务执行前应先读。
- `agent-queue.md` 是目标模式执行入口；为空时回退到 `agent-progress.md` 当前批次和 `agent-plan.md`。
- `agent-plan.md` 是内联卡片状态权威源；卡片文件中的卡片以对应文件为权威源；`agent-progress.md` 只记录当前批次、自动提交策略、不要重复做和会话历史。
- 执行窗口只跑当前批次；并行规划窗口只能追加未来批次或写入 `agent-inbox.md`。
- 当前 V0 执行队列已经按长期规划收敛为 DMA 收口、刷新诊断、无效刷新优化和 F411 XML 兼容性探针；旧 `F411-XML-Q2-1` 保留为历史卡片，但不得直接从队列执行。

### 我想让 AI 执行复杂任务前锁边界

先读：

- [magicwatch_ai_collaboration_rules.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/magicwatch_ai_collaboration_rules.md)
- [magicwatch_long_term_roadmap.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/magicwatch_long_term_roadmap.md)，仅在需要确认当前阶段门槛或停止条件时补读。

说明：

- 该规则文档用于每次实现前声明 `Allowed files / Forbidden files / Forbidden changes`，并在收尾时检查是否越界、是否破坏已验收行为、是否把未验证项写成通过。
- 它补充卡片批次规则，不替代 `agent-queue.md` 和具体卡片。

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

### 我想审查当前 PC XML 主链每一层是否必要

先读：

- [current_watch_architecture_map_and_necessity_audit.md](/D:/MY_Desk/watch/magic_watch/docs/30_testing/current_watch_architecture_map_and_necessity_audit.md)
- [lvgl_xml_watch_core_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/lvgl_xml_watch_core_architecture.md)，仅在需要对照目标蓝图时补读。

说明：

- 该审计只描述当前 `magic_watch_xml_sim` 的真实运行链。
- 它区分活跃路径、未接通路径和未来候选层，不把旧 C++ 模拟器混入当前主线。
- 下一步应先审系统事件如何从 PC HAL 进入共享状态，不直接扩建 EventBus 或 ScreenManager。

### 我想回到 F411 重新掌控架构蓝图

先读：

- [lvgl_xml_watch_core_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/lvgl_xml_watch_core_architecture.md)，如果目标是继续当前 XML/F411 主线。
- [f411_future_watch_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/f411_future_watch_architecture.md)
- [f411_future_watch_architecture_uml.html](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/f411_future_watch_architecture_uml.html)

说明：

- F411 / C-first 蓝图仍是当前真机落地参考，但当前执行主线已经升级为 `LVGL XML + watch_core + PC/F411 双后端`。
- 它只抽取 Magic Watch 中值得保留的分层思想：输入语义、Service、ModelStore、EventQueue、Coordinator、Screen Manager 和 UI Adapter。
- 它不要求当前 API 已存在，也不搬模拟器 C++ 实现；后续代码应按小卡片让骨架逐步长出来。

### 我想进入阶段 9 真实硬件桥接

先读：

- [stage9_hardware_bridge_plan.md](/D:/MY_Desk/watch/magic_watch/docs/70_hardware_reference/stage9_hardware_bridge_plan.md)
- [hardware_boundary.md](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/hardware_boundary.md)
- [twatch_s3_plus_reference_validation_run.md](/D:/MY_Desk/watch/magic_watch/docs/70_hardware_reference/twatch_s3_plus_reference_validation_run.md)，仅在需要核对 T-Watch S3 Plus 既有 bring-up 事实时补读。

说明：

- 阶段 9 第一真实硬件对象默认是现有 T-Watch S3 Plus。
- 第一真实数据源默认是 AXP2101 / BatteryPowerStatus。
- 硬件闭环必须在真实 MCU 上交叉编译并运行 Magic Watch 最小架构子集。
- PC trace replay / log replay 只能作为辅助测试，不能作为阶段 9 硬件落地证明。
- F411 旧板作为 STM32 + FreeRTOS + 多传感器经验对照，不作为第一桥接对象。

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
- [v0_5_core_contract_learning_closure.md](/D:/MY_Desk/watch/magic_watch/docs/50_stage_records/v0_5_core_contract_learning_closure.md)，仅在需要理解为什么旧 `V0.5-A~G` 已过时、当前 `V0.5` 真正完成了什么、以及 `V0.6` 从哪里开始时补读。
- [50_stage_records/](/D:/MY_Desk/watch/magic_watch/docs/50_stage_records)

说明：

- 这批 `v0_*` 文档已迁入 `docs/50_stage_records/`，属于历史阶段记录，不回到默认新会话入口。
- 需要看当前范围时，优先读 [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/v0_scope.md)。

### 我想查历史决策

先读：

- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)

说明：

- 它是历史检索材料，不应回到默认入口。
- 它继续保留在 docs 根目录，目的是给跨会话追溯和旧文档链接一个稳定、可预期的位置。
- 读取时只检索相关日期或关键词，不默认通读全文。

### 我想看 docs 根目录剩余文档的二次评审

先读：

- [docs_root_reorg_inventory.md](/D:/MY_Desk/watch/magic_watch/docs/40_workflow/docs_root_reorg_inventory.md)

说明：

- `DOCROOT-B6-C01` 已把 Wave 1 迁移后的剩余根目录文档分成三类：明确保留根目录、第二轮候选迁移、仍需单独决策。
- `cpp_minimal_glossary.md`、`learning_plan.md`、`simulator_reading_guide.md` 已迁入 [20_guides/](/D:/MY_Desk/watch/magic_watch/docs/20_guides)；`project_charter.md`、`prototype_progress.md`、`page_reachability_audit.md`、`simulator_manual_regression_run_2026_05_19.md`、`current_sim_architecture.md` 已迁入 [80_history/](/D:/MY_Desk/watch/magic_watch/docs/80_history)。

### 我想查历史归档文档

先读：

- [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/project_charter.md)
- [prototype_progress.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/prototype_progress.md)
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/page_reachability_audit.md)
- [simulator_manual_regression_run_2026_05_19.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/simulator_manual_regression_run_2026_05_19.md)
- [current_sim_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/current_sim_architecture.md)

说明：

- 这五份文档属于历史归档/历史检索入口，不回到默认新会话入口。
- 它们分别服务于旧阶段定位、原型推进记录、历史页面可达性判断、一次性手动回归运行记录，以及旧版模拟器现状骨架的追溯。

### 我想补读开发指南

先读：

- [magicwatch_personal_growth_plan.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/magicwatch_personal_growth_plan.md)
- [watch_arch_learning_lab.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/watch_arch_learning_lab.md)，仅在规划或执行独立 PC 架构冲突实验场时读取。
- [architecture_control_learning_workbook.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/architecture_control_learning_workbook.md)，历史学习现场；只在具体实验暴露 LVGL、FreeRTOS 或当前主链知识缺口时按需回看。
- [cpp_minimal_glossary.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/cpp_minimal_glossary.md)
- [learning_plan.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/learning_plan.md)
- [simulator_reading_guide.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/simulator_reading_guide.md)

说明：

- 这些文档属于按需阅读的开发指南，不回到默认新会话入口。
- `magicwatch_personal_growth_plan.md` 把 V0-V3 工程阶段映射到 C 工程化、LVGL 生命周期、性能分析、状态机、FreeRTOS、低功耗和 C++ 阅读能力。
- `watch_arch_learning_lab.md` 定义需求压力先于抽象、Stage 启停门、产品隔离和 Stage 00 边界；暂定冲突地图不是自动执行路线。
- `architecture_control_learning_workbook.md` 保留此前逐链阅读、用户作答和 Agent 纠偏，不再作为当前学习路线或队列入口。
- 其余三份分别服务于 C++ 代码阅读补盲、学习路径规划和模拟器主线代码导读。

## 文档维护规则

- 新增文档必须说明它属于当前入口、架构专题、开发指南、审计资料还是历史检索。
- 当前入口文档应短、稳定、面向新会话。
- 历史文档保留价值，但不得默认进入上下文。
- 中文文档修改后，对本轮实际改动的中文文件做乱码哨兵检查。
