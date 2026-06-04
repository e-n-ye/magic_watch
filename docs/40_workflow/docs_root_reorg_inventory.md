# Docs Root Reorg Inventory

日期：2026-06-03

本文档先完成过一次根目录盘点，并已作为 `DOCROOT-B3-C01`、`DOCROOT-B4-C01`、`DOCROOT-B5-C01` 的迁移依据。当前版本补充 `DOCROOT-B6-C01` 的二次评审结论，只评审迁移后仍留在 `docs` 根目录的文件，不再直接搬运。

## 盘点范围与分类说明

- 盘点范围：仅 `docs/*.md`，不包含 `docs/00_current/`、`docs/10_architecture/`、`docs/30_testing/`、`docs/40_workflow/`、`docs/90_resume/` 等子目录文件。
- 分类值只使用本卡允许范围：`保留根目录`、`50_stage_records`、`60_visual_exploration`、`70_hardware_reference`、`20_guides`、`80_history`、`待单独决策`。
- 本文档只给出迁移建议，不代表本轮已批准后续迁移。

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
| `current_sim_architecture.md` | `待单独决策` | 名称与 `docs/00_current/current_architecture.md` 过于接近，需先确认是否保留为历史审计材料。 |
| `decision_log.md` | `待单独决策` | 仍有长期记忆价值，但 `document_map.md` 已把它降为历史检索入口，迁移前需评估 AGENTS 与旧引用。 |
| `document_map.md` | 保留根目录 | 当前文档路由器，必须继续占据根目录入口。 |
| `home_interaction_model.md` | 保留根目录 | 表盘中心交互模型仍是长期架构基线。 |
| `learning_plan.md` | `已迁移至 20_guides` | 学习路径指南，保留价值在于方法论，不必占用根目录默认入口。 |
| `page_reachability_audit.md` | `已迁移至 80_history` | `document_map.md` 已明确它是历史页面审计。 |
| `project_charter.md` | `已迁移至 80_history` | `document_map.md` 已说明该文档被 `00_current/project_brief.md` 替代。 |
| `prototype_progress.md` | `已迁移至 80_history` | 原型推进记录，历史上下文价值大于当前入口价值。 |
| `simulator_manual_regression_matrix.md` | `待单独决策` | 内容属于测试矩阵，更像未来 `30_testing` 二次整理对象，不建议先硬塞到 `20_guides`。 |
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
| `80_history` | 4 | 明确已被替代或一次性运行记录的文档可归档。 |
| `待单独决策` | 3 | 涉及命名冲突、长期记忆角色或测试目录归属，先不机械迁移。 |

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

## 仍需单独决策项

### 1. `docs/current_sim_architecture.md` 与 `docs/00_current/current_architecture.md`

- 二者名字相近，但职责可能不同：前者更像旧的模拟器现状地图，后者是当前主线架构入口。
- 若直接迁移或保留其一，容易让新会话误读“哪个才是当前权威入口”。
- 建议后续单独审计其引用关系和内容重叠度，再决定是迁入历史还是保留补充说明。

### 2. `docs/decision_log.md` 的角色

- 它仍是长期记忆材料，但 `document_map.md` 已把它降为历史检索入口。
- 如果直接迁入 `80_history`，需要同时检查 `AGENTS.md`、旧文档说明和团队使用习惯是否仍把它视为关键背景入口。

### 3. `docs/simulator_manual_regression_matrix.md` 的归属

- 这份文档明显属于测试资产，而不是普通指南或纯历史文档。
- 但本轮目标类别里没有单独的测试归档桶，因此先列为“待单独决策”，避免误迁后让测试入口更混乱。

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

## DOCROOT-B6-C01 结论

- Wave 1 迁移后的根目录剩余文件已经足够聚焦，默认入口没有继续被历史阶段记录、视觉流程或硬件参考淹没。
- 第二轮如果继续推进，应拆成新的独立卡片，只处理 `20_guides`、`80_history` 或“待单独决策”中的一个清晰子集。
- 本卡到此停止，不直接执行第二轮迁移。

## 历史执行顺序回顾

1. 先执行 `DOCROOT-B3-C01`：迁移 `50_stage_records` 候选。
2. 再执行 `DOCROOT-B4-C01`：迁移 `60_visual_exploration` 候选。
3. 再执行 `DOCROOT-B5-C01`：迁移 `70_hardware_reference` 候选。
4. 最后执行 `DOCROOT-B6-C01`：复盘剩余根目录文件，决定 `20_guides`、`80_history` 和“待单独决策”项的第二轮处理。

这个顺序的原因是：

- 阶段记录、视觉资料、硬件参考最不应该继续占据默认入口，且迁移边界相对清晰。
- `decision_log.md`、`current_sim_architecture.md`、`simulator_manual_regression_matrix.md` 这类文件在角色上更模糊，适合放到第二轮评审。
