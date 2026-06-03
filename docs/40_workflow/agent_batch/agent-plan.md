# Agent 任务计划

本文件是任务卡片清单，卡片状态以本文件为权威源。

执行规则见：`docs/40_workflow/agent_batch/agent-rules.md`。
运行时进度见：`docs/40_workflow/agent_batch/agent-progress.md`。
并行规划草稿见：`docs/40_workflow/agent_batch/agent-inbox.md`。

---

## DOCROOT-B2-C01 docs 根目录盘点与分类建议

- 批次：2
- 状态：DONE
- 依赖：无
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查
- 建议提交信息：`docs: inventory docs root and propose reorg buckets`
- Doc Impact：required

### 问题定位

`docs` 根目录同时承载当前入口、历史阶段记录、视觉探索、硬件参考和流程资料，噪音过高；但直接批量迁移会把当前路由文档和历史文档混在一起。

### 实施方案

1. 新增一份根目录盘点文档，列出当前根目录全部文件。
2. 对每个文件标记目标类别：保留根目录、迁入 `50_stage_records`、迁入 `60_visual_exploration`、迁入 `70_hardware_reference`、迁入 `20_guides`、迁入 `80_history`、或“待单独决策”。
3. 单独列出高风险冲突项，至少包含 `docs/v0_scope.md` 与 `docs/00_current/v0_scope.md` 的语义区别。
4. 给出后续迁移顺序建议，只提建议，不移动文件，不改 `document_map.md`。

### 涉及位置

Allowed files:

- `docs/40_workflow/docs_root_reorg_inventory.md`

Read-only files:

- `docs/document_map.md`
- `docs/*.md`
- `docs/00_current/*.md`
- `docs/10_architecture/*.md`
- `docs/30_testing/*.md`

Forbidden changes:

- 不移动、重命名、删除任何文档。
- 不修改 `docs/document_map.md`。
- 不修复链接。
- 不把 `docs/v0_scope.md` 与 `docs/00_current/v0_scope.md` 合并成一个文件。

### 风险

- 若盘点结论把“当前入口文档”误归为历史档，会导致后续迁移破坏新会话路由；对策是先显式列出“保留根目录 / 待单独决策”清单。

### 验收标准

- [ ] 有一份完整的根目录盘点文档。
- [ ] 每个根目录文件都有明确类别建议。
- [ ] 明确列出高风险冲突项和暂不迁移项。
- [ ] 明确后续批次顺序。
- [ ] 本卡未移动文件、未修改 `document_map.md`、未修复链接。

### 执行记录

- 完成时间：2026-06-03（本轮会话）
- 实际改动文件：
  - `docs/40_workflow/docs_root_reorg_inventory.md`
- 自检结果：
  - `git diff --check` 通过
  - 中文文档乱码哨兵检查通过，未命中本轮改动文件
  - `docs/*.md` 根目录清单已与盘点表逐项核对
- 风险回应：
  - 只给出分类建议与迁移顺序，没有移动、重命名、删除任何文档
  - `v0_scope` 双版本、`current_sim_architecture`/`current_architecture`、`decision_log`、`simulator_manual_regression_matrix`、`watchface_style_architecture` 已单独标为高风险冲突或待决项，避免误迁
