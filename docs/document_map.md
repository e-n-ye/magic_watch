# Magic Watch 文档任务路由器

日期：2026-05-26

本文件不再是“推荐阅读清单”，而是任务路由器。默认上下文必须短；历史文档可以检索，但不能污染每次新会话。

## 默认新会话

默认只读：

1. `AGENTS.md`
2. [document_map.md](/D:/MY_Desk/watch/magic_watch/docs/document_map.md)
3. [project_brief.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/project_brief.md)
4. [current_decisions.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_decisions.md)
5. [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)
6. [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/v0_scope.md)

如果时间很紧，最少读：

- [document_map.md](/D:/MY_Desk/watch/magic_watch/docs/document_map.md)
- [project_brief.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/project_brief.md)
- [current_decisions.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_decisions.md)
- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)

## 当前默认不读

以下文档不再作为默认入口：

- [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/project_charter.md): 旧阶段项目宪章，已由 `00_current/project_brief.md` 接管当前定位。
- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md): 历史决策流水账，只在查历史原因时检索。
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md): 页面归属专题文档，只在页面相关任务中选读。

## 按任务选读

### 架构收口 / Controller 拆分

先读：

- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)
- [current_decisions.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_decisions.md)

后续补充：

- `docs/10_architecture/state_machine.md`，在第 1A 轮创建后使用。
- `docs/10_architecture/event_flow.md`，在事件流收口时使用。

### 页面归属 / 可达性 / ShellPages 拆分

先读：

- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)
- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)

只在需要判断当前主路径、旧页面、Launcher 入口、HomeRing surface 时读取。

### 新增或调整 Service

先读：

- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)
- [v0_4_battery_power_service_boundary.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_battery_power_service_boundary.md)
- [v0_6_steps_activity_service_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_6_steps_activity_service_entry.md)
- [v0_7_notification_service_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_7_notification_service_entry.md)

### 输入意图 / 手势 / 表冠

先读：

- [home_interaction_model.md](/D:/MY_Desk/watch/magic_watch/docs/home_interaction_model.md)
- [v0_4_platform_hal_event_contract.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_platform_hal_event_contract.md)

### UI 生命周期 / SettingsPages / ShellPages 拆分

先读：

- [current_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/00_current/current_architecture.md)
- [current_sim_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/current_sim_architecture.md)
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)，仅在页面归属需要时。

### 硬件边界 / 真实硬件接入

先读：

- [hardware_frontier_entry.md](/D:/MY_Desk/watch/magic_watch/docs/hardware_frontier_entry.md)
- [hardware_candidate_constraints.md](/D:/MY_Desk/watch/magic_watch/docs/hardware_candidate_constraints.md)
- [twatch_s3_plus_reference_validation_plan.md](/D:/MY_Desk/watch/magic_watch/docs/twatch_s3_plus_reference_validation_plan.md)
- [twatch_s3_plus_reference_validation_run.md](/D:/MY_Desk/watch/magic_watch/docs/twatch_s3_plus_reference_validation_run.md)

### 查历史原因

只在需要复盘某个旧决定时读：

- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)

读取时只检索相关日期或关键词，不默认通读全文。

## 文档维护规则

- 新增文档必须说明它属于当前入口、架构专题、开发指南、审计资料还是历史检索。
- 当前入口文档应短、稳定、面向新会话。
- 历史文档保留价值，但不得默认进入上下文。
- 中文文档修改后，对本轮实际改动的中文文件做乱码哨兵检查。
