# Docs Root Reorg Inventory

日期：2026-06-03

本文档先完成过一次根目录盘点，并已作为 `DOCROOT-B3-C01`、`DOCROOT-B4-C01`、`DOCROOT-B5-C01` 的迁移依据。当前版本补充 `DOCROOT-B6-C01` 的二次评审结论，只评审迁移后仍留在 `docs` 根目录的文件，不再直接搬运。

## 盘点范围与分类说明

- 盘点范围：仅 `docs/*.md`，不包含 `docs/00_current/`、`docs/10_architecture/`、`docs/30_testing/`、`docs/40_workflow/`、`docs/90_resume/` 等子目录文件。
- 分类值只使用本卡实际结论范围：`保留根目录`、`30_testing`、`50_stage_records`、`60_visual_exploration`、`70_hardware_reference`、`20_guides`、`80_history`、`待单独决策`。
- 本文档已完成 Wave 1 / Wave 2 / Wave 3 审计与最终复核；当前结论可作为 docs 根目录清洁度判断依据。

## Wave 1 迁移完成情况

- 已迁入 `docs/50_stage_records/`：全部 `v0_*` 阶段记录、历史版 `v0_scope.md`、`v0_shell_baseline.md`。
- 已迁入 `docs/60_visual_exploration/`：`settings_visual_reference.md`、`weather_shortcut_visual_direction.md`、`weather_shortcut_implementation_spec.md`、`visual_asset_workflow.md`、`weather_icon_workflow.md`、`web_visual_sandbox_workflow.md`。
- 已迁入 `docs/70_hardware_reference/`：`hardware_frontier_entry.md`、`hardware_component_selection_strategy.md`、`hardware_candidate_shortlist.md`、`hardware_candidate_constraints.md`、`twatch_s3_plus_reference_validation_plan.md`、`twatch_s3_plus_reference_validation_run.md`。

上述迁移已完成，本卡不再把它们算作“剩余根目录候选”。

## 迁移后剩余根目录文件评审

| 文件 | 建议类别 | 理由 |
| --- | --- | --- |
| `architecture_principles.md` | 保留根目录 | 长期架构原则，仍是跨阶段入口文档。 |
| `cpp_minimal_glossary.md` | `已迁移至 20_guides` | 面向代码阅读的辅助指南，不属于默认入口。 |
| `current_sim_architecture.md` | `已迁移至 80_history` | 内容自定义为“第一版现状地图”，不是当前权威架构入口；迁入历史归档可避免与 `docs/00_current/current_architecture.md` 形成双权威。 |
| `decision_log.md` | 保留根目录 | 已被 `document_map.md` / `current_decisions.md` 降为历史检索材料，但 `workflow.md` 仍把它作为历史记忆落点；保留稳定根目录路径比迁入 `80_history` 更稳。 |
| `document_map.md` | 保留根目录 | 当前文档路由器，必须继续占据根目录入口。 |
| `home_interaction_model.md` | 保留根目录 | 表盘中心交互模型仍是长期架构基线。 |
| `learning_plan.md` | `已迁移至 20_guides` | 学习路径指南，保留价值在于方法论，不必占用根目录默认入口。 |
| `page_reachability_audit.md` | `已迁移至 80_history` | `document_map.md` 已明确它是历史页面审计。 |
| `project_charter.md` | `已迁移至 80_history` | `document_map.md` 已说明该文档被 `00_current/project_brief.md` 替代。 |
| `prototype_progress.md` | `已迁移至 80_history` | 原型推进记录，历史上下文价值大于当前入口价值。 |
| `simulator_manual_regression_matrix.md` | `已迁移至 30_testing` | 内容属于测试矩阵，应与更短回归清单一起留在 `docs/30_testing/`。 |
| `simulator_manual_regression_run_2026_05_19.md` | `已迁移至 80_history` | 一次性回归运行记录，适合历史归档。 |
| `simulator_reading_guide.md` | `已迁移至 20_guides` | 阅读引导型文档，属于开发指南。 |
| `target_architecture.md` | 保留根目录 | 长期目标架构骨架，仍是跨阶段核心架构文档。 |
| `watchface_style_architecture.md` | 保留根目录 | 虽与视觉有关，但本质是表盘风格架构边界，不建议并入视觉探索目录。 |
| `workflow.md` | 保留根目录 | 仍是项目级协作工作流入口。 |

## 建议汇总

| 类别 | 数量 | 建议说明 |
| --- | ---: | --- |
| 保留根目录 | 6 | 当前入口、长期架构、长期协作文档继续保留在根目录。 |
| `20_guides` | 3 | 阅读/学习型材料适合在第二轮迁入开发指南目录。 |
| `30_testing` | 1 | 完整手动回归矩阵应与测试入口文档放在同一目录。 |
| `80_history` | 5 | 明确已被替代、属于旧现状地图或一次性运行记录的文档可归档。 |
| `待单独决策` | 0 | 第二轮单独决策项已全部完成，独立复核卡已确认当前 docs 根目录可视为干净。 |

## 明确保留根目录名单

- `architecture_principles.md`
- `document_map.md`
- `home_interaction_model.md`
- `target_architecture.md`
- `watchface_style_architecture.md`
- `workflow.md`

这些文件分别承担默认入口、长期架构原则、核心交互模型、目标骨架或项目级协作流程职责；当前不建议继续降到二级目录。

## 第二轮候选迁移名单

`DOCROOT-B8-C01` 已完成 `20_guides` 波次迁移，`DOCROOT-B9-C01` 已完成 `80_history` 波次迁移；以下列表仅保留尚未迁移的候选。

## 已迁移

### `20_guides`

- [cpp_minimal_glossary.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/cpp_minimal_glossary.md)
- [learning_plan.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/learning_plan.md)
- [simulator_reading_guide.md](/D:/MY_Desk/watch/magic_watch/docs/20_guides/simulator_reading_guide.md)

### `80_history`

- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/page_reachability_audit.md)
- [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/project_charter.md)
- [prototype_progress.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/prototype_progress.md)
- [simulator_manual_regression_run_2026_05_19.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/simulator_manual_regression_run_2026_05_19.md)
- [current_sim_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/80_history/current_sim_architecture.md)

### `30_testing`

- [simulator_manual_regression_matrix.md](/D:/MY_Desk/watch/magic_watch/docs/30_testing/simulator_manual_regression_matrix.md)

## 单独决策项收口结论

### `docs/decision_log.md`

- `document_map.md` 与 `current_decisions.md` 都已经把它降为“非默认入口”的历史检索材料。
- `workflow.md` 仍要求在必要时把历史理由、阶段判断或检索轨迹写入这里，说明它还承担稳定记忆落点职责。
- 由于 `AGENTS.md` 仍把它列为关键背景之一，本轮不迁入 `80_history`；结论改为“保留根目录，但不回默认入口”。

## 暂不迁移项

- `document_map.md`
- `workflow.md`
- `architecture_principles.md`
- `target_architecture.md`
- `home_interaction_model.md`
- `watchface_style_architecture.md`
- `decision_log.md`

这些文件要么是当前入口，要么已完成二次评审并确认应保留在根目录。

## DOCROOT-B6-C01 结论

- Wave 1 迁移后的根目录剩余文件已经足够聚焦，默认入口没有继续被历史阶段记录、视觉流程或硬件参考淹没。
- 第二轮如果继续推进，应拆成新的独立卡片，只处理 `20_guides`、`80_history` 或“待单独决策”中的一个清晰子集。
- 本卡到此停止，不直接执行第二轮迁移。

## DOCROOT-B10-C04 最终复核结论

- 当前 `docs/*.md` 根目录只剩 7 份文档：`architecture_principles.md`、`document_map.md`、`home_interaction_model.md`、`target_architecture.md`、`watchface_style_architecture.md`、`workflow.md`、`decision_log.md`。
- 其中前 6 份分别承担当前入口、长期架构专题或项目级协作工作流职责；`decision_log.md` 虽不再是默认入口，但保留为稳定历史检索路径。
- `20_guides`、`30_testing`、`50_stage_records`、`60_visual_exploration`、`70_hardware_reference`、`80_history` 的候选根目录文档都已完成迁移或明确归类。
- 旧路径字符串仍可能出现在历史执行记录、卡片说明文本和 `docs/decision_log.md` 的历史编码事故示例里，但这些都不属于当前正文路由残留。
- 结论：当前 docs 根目录已干净，可视为“只剩应保留的稳定入口与稳定检索路径”。

## 历史执行顺序回顾

1. 先执行 `DOCROOT-B3-C01`：迁移 `50_stage_records` 候选。
2. 再执行 `DOCROOT-B4-C01`：迁移 `60_visual_exploration` 候选。
3. 再执行 `DOCROOT-B5-C01`：迁移 `70_hardware_reference` 候选。
4. 最后执行 `DOCROOT-B6-C01`：复盘剩余根目录文件，决定 `20_guides`、`80_history` 和“待单独决策”项的第二轮处理。

这个顺序的原因是：

- 阶段记录、视觉资料、硬件参考最不应该继续占据默认入口，且迁移边界相对清晰。
- `decision_log.md`、`current_sim_architecture.md` 这类文件在角色上更模糊，适合放到第二轮评审。
