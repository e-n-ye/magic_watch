# Docs Root Reorg Inventory

日期：2026-06-03

本文档只盘点 `docs` 根目录下的 Markdown 文件，不移动文件，不修改 `document_map.md`，不修复链接。

## 盘点范围与分类说明

- 盘点范围：仅 `docs/*.md`，不包含 `docs/00_current/`、`docs/10_architecture/`、`docs/30_testing/`、`docs/40_workflow/`、`docs/90_resume/` 等子目录文件。
- 分类值只使用本卡允许范围：`保留根目录`、`50_stage_records`、`60_visual_exploration`、`70_hardware_reference`、`20_guides`、`80_history`、`待单独决策`。
- 本文档只给出迁移建议，不代表本轮已批准后续迁移。

## 根目录文件全量盘点

| 文件 | 建议类别 | 理由 |
| --- | --- | --- |
| `architecture_principles.md` | 保留根目录 | 长期架构原则，仍是跨阶段入口文档。 |
| `cpp_minimal_glossary.md` | `20_guides` | 面向代码阅读的辅助指南，不属于默认入口。 |
| `current_sim_architecture.md` | `待单独决策` | 名称与 `docs/00_current/current_architecture.md` 过于接近，需先确认是否保留为历史审计材料。 |
| `decision_log.md` | `待单独决策` | 仍有长期记忆价值，但 `document_map.md` 已把它降为历史检索入口，迁移前需评估 AGENTS 与旧引用。 |
| `document_map.md` | 保留根目录 | 当前文档路由器，必须继续占据根目录入口。 |
| `hardware_candidate_constraints.md` | `70_hardware_reference` | 硬件候选约束，属于按需硬件参考。 |
| `hardware_candidate_shortlist.md` | `70_hardware_reference` | 硬件候选清单，属于按需硬件参考。 |
| `hardware_component_selection_strategy.md` | `70_hardware_reference` | 硬件选型策略，属于按需硬件参考。 |
| `hardware_frontier_entry.md` | `70_hardware_reference` | 硬件前沿入口，属于按需硬件参考。 |
| `home_interaction_model.md` | 保留根目录 | 表盘中心交互模型仍是长期架构基线。 |
| `learning_plan.md` | `20_guides` | 学习路径指南，保留价值在于方法论，不必占用根目录默认入口。 |
| `page_reachability_audit.md` | `80_history` | `document_map.md` 已明确它是历史页面审计。 |
| `project_charter.md` | `80_history` | `document_map.md` 已说明该文档被 `00_current/project_brief.md` 替代。 |
| `prototype_progress.md` | `80_history` | 原型推进记录，历史上下文价值大于当前入口价值。 |
| `settings_visual_reference.md` | `60_visual_exploration` | 视觉参考文档，属于按需视觉资料。 |
| `simulator_manual_regression_matrix.md` | `待单独决策` | 内容属于测试矩阵，更像未来 `30_testing` 二次整理对象，不建议先硬塞到 `20_guides`。 |
| `simulator_manual_regression_run_2026_05_19.md` | `80_history` | 一次性回归运行记录，适合历史归档。 |
| `simulator_reading_guide.md` | `20_guides` | 阅读引导型文档，属于开发指南。 |
| `target_architecture.md` | 保留根目录 | 长期目标架构骨架，仍是跨阶段核心架构文档。 |
| `twatch_s3_plus_reference_validation_plan.md` | `70_hardware_reference` | TWatch 参考验证计划，属于硬件参考。 |
| `twatch_s3_plus_reference_validation_run.md` | `70_hardware_reference` | TWatch 参考验证运行记录，属于硬件参考。 |
| `v0_1_architecture_closure.md` | `50_stage_records` | `v0_*` 阶段收口记录。 |
| `v0_2_shell_closure.md` | `50_stage_records` | `v0_*` 阶段收口记录。 |
| `v0_4_battery_power_service_boundary.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_4_platform_hal_event_contract.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_4_simulator_system_boundary_audit.md` | `50_stage_records` | `v0_*` 阶段审计记录。 |
| `v0_4_stage_closure.md` | `50_stage_records` | `v0_*` 阶段收口记录。 |
| `v0_4_system_skeleton_entry.md` | `50_stage_records` | `v0_*` 阶段入口记录。 |
| `v0_5_long_battery_regression_closure.md` | `50_stage_records` | `v0_*` 阶段回归收口记录。 |
| `v0_5_power_status_page_entry.md` | `50_stage_records` | `v0_*` 阶段入口记录。 |
| `v0_6_steps_activity_service_entry.md` | `50_stage_records` | `v0_*` 阶段入口记录。 |
| `v0_7_notification_service_entry.md` | `50_stage_records` | `v0_*` 阶段入口记录。 |
| `v0_7b_notification_preview_shell_linkage.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_7c1_notification_read_state_and_detail_entry.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_7c2a_notification_clear_semantics.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_7c2b1_notification_swipe_delete.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_7d_notification_stage_closure.md` | `50_stage_records` | `v0_*` 阶段收口记录。 |
| `v0_8a_health_monitoring_settings_model_entry.md` | `50_stage_records` | `v0_*` 阶段入口记录。 |
| `v0_8b_health_monitoring_settings_model_impl.md` | `50_stage_records` | `v0_*` 阶段实现记录。 |
| `v0_8c1_sleep_home_settings_shell.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_8c2_sleep_info_body.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_8d1_blood_oxygen_home_settings_shell.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_8d2_blood_oxygen_info_body.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_8e1_heart_rate_home_resting30.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_8e2_heart_rate_settings.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_8e3_heart_rate_info_body.md` | `50_stage_records` | `v0_*` 阶段专题记录。 |
| `v0_scope.md` | `50_stage_records` | 根目录版是历史阶段范围文档，已被 `docs/00_current/v0_scope.md` 接管当前摘要入口。 |
| `v0_shell_baseline.md` | `50_stage_records` | 明确写的是早期 shell baseline，属于阶段记录。 |
| `visual_asset_workflow.md` | `60_visual_exploration` | 视觉资产流程文档，属于按需视觉资料。 |
| `watchface_style_architecture.md` | 保留根目录 | 虽与视觉有关，但本质是表盘风格架构边界，不建议并入视觉探索目录。 |
| `weather_icon_workflow.md` | `60_visual_exploration` | 天气图标流程文档，属于按需视觉资料。 |
| `weather_shortcut_implementation_spec.md` | `60_visual_exploration` | 天气快捷页视觉/实现规范，属于视觉探索链路。 |
| `weather_shortcut_visual_direction.md` | `60_visual_exploration` | 天气快捷页视觉方向稿，属于按需视觉资料。 |
| `web_visual_sandbox_workflow.md` | `60_visual_exploration` | 视觉沙箱流程文档，属于按需视觉资料。 |
| `workflow.md` | 保留根目录 | 仍是项目级协作工作流入口。 |

## 建议汇总

| 类别 | 数量 | 建议说明 |
| --- | ---: | --- |
| 保留根目录 | 7 | 当前入口、长期架构、长期协作文档先保留。 |
| `50_stage_records` | 24 | `v0_*` 及同类阶段记录优先迁走，噪音最大、风险相对最低。 |
| `60_visual_exploration` | 6 | 视觉方向稿与流程文档聚合迁移，便于按需补读。 |
| `70_hardware_reference` | 6 | 硬件候选与 TWatch 参考资料应退出默认入口。 |
| `20_guides` | 4 | 阅读/学习型材料更适合作为指南库。 |
| `80_history` | 4 | 明确已被替代或一次性运行记录的文档可归档。 |
| `待单独决策` | 3 | 涉及命名冲突、长期记忆角色或测试目录归属，先不机械迁移。 |

## 高风险冲突项

### 1. `docs/v0_scope.md` 与 `docs/00_current/v0_scope.md`

- 根目录版 `docs/v0_scope.md` 是历史阶段范围文档，仍保存完整 `v0` 起始边界。
- `docs/00_current/v0_scope.md` 是当前阶段摘要入口，已经接管“当前范围”职责。
- 这两个文件不能合并成一个，也不能把根目录版当作当前入口继续暴露。

### 2. `docs/current_sim_architecture.md` 与 `docs/00_current/current_architecture.md`

- 二者名字相近，但职责可能不同：前者更像旧的模拟器现状地图，后者是当前主线架构入口。
- 若直接迁移或保留其一，容易让新会话误读“哪个才是当前权威入口”。
- 建议后续单独审计其引用关系和内容重叠度，再决定是迁入历史还是保留补充说明。

### 3. `docs/decision_log.md` 的角色

- 它仍是长期记忆材料，但 `document_map.md` 已把它降为历史检索入口。
- 如果直接迁入 `80_history`，需要同时检查 `AGENTS.md`、旧文档说明和团队使用习惯是否仍把它视为关键背景入口。

### 4. `docs/simulator_manual_regression_matrix.md` 的归属

- 这份文档明显属于测试资产，而不是普通指南或纯历史文档。
- 但本轮目标类别里没有单独的测试归档桶，因此先列为“待单独决策”，避免误迁后让测试入口更混乱。

### 5. `docs/watchface_style_architecture.md` 的误判风险

- 名称看起来像视觉文档，但其核心内容是风格宿主与架构边界。
- 不建议在视觉迁移批次中顺手搬走，否则会把架构专题混进视觉资料。

## 暂不迁移项

- `document_map.md`
- `workflow.md`
- `architecture_principles.md`
- `target_architecture.md`
- `home_interaction_model.md`
- `watchface_style_architecture.md`
- `current_sim_architecture.md`
- `decision_log.md`
- `simulator_manual_regression_matrix.md`

这些文件要么是当前入口，要么存在命名/职责冲突，需要二次评审后再决定。

## 后续迁移顺序建议

1. 先执行 `DOCROOT-B3-C01`：迁移 `50_stage_records` 候选。
2. 再执行 `DOCROOT-B4-C01`：迁移 `60_visual_exploration` 候选。
3. 再执行 `DOCROOT-B5-C01`：迁移 `70_hardware_reference` 候选。
4. 最后执行 `DOCROOT-B6-C01`：复盘剩余根目录文件，决定 `20_guides`、`80_history` 和“待单独决策”项的第二轮处理。

这个顺序的原因是：

- 阶段记录、视觉资料、硬件参考最不应该继续占据默认入口，且迁移边界相对清晰。
- `decision_log.md`、`current_sim_architecture.md`、`simulator_manual_regression_matrix.md` 这类文件在角色上更模糊，适合放到第二轮评审。
