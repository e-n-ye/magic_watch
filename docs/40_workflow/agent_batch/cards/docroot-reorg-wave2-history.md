# DOCROOT Wave 2 History

本卡片包用于 docs 根目录第二轮整理中的 `80_history` 迁移波次。

---

### DOCROOT-B9-C01 迁移历史归档到 80_history

- 批次：9
- 状态：TODO
- 依赖：DOCROOT-B7-C01
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查；扫描 `page_reachability_audit.md`、`project_charter.md`、`prototype_progress.md`、`simulator_manual_regression_run_2026_05_19.md` 旧路径引用是否已清空
- 建议提交信息：`docs: move history docs under 80_history`
- Doc Impact：required

#### 问题定位

`page_reachability_audit.md`、`project_charter.md`、`prototype_progress.md`、`simulator_manual_regression_run_2026_05_19.md` 都已被当前入口文档替代或只承担历史检索价值，适合迁入统一历史目录。

#### 实施方案

1. 首次建立 `docs/80_history/`。
2. 将 `page_reachability_audit.md`、`project_charter.md`、`prototype_progress.md`、`simulator_manual_regression_run_2026_05_19.md` 迁入 `docs/80_history/`，文件名保持不变。
3. 更新 `docs/document_map.md`，把这四份文档统一明确为历史检索/历史归档入口。
4. 修复仓库内对这四份文档旧路径的直接 Markdown 链接。
5. 更新 `docs/40_workflow/docs_root_reorg_inventory.md`，把这四项从第二轮候选改为已迁移。

#### 涉及位置

Allowed files:

- `docs/80_history/*.md`
- `docs/document_map.md`
- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/**/*.md` 中直接引用 `docs/page_reachability_audit.md`、`docs/project_charter.md`、`docs/prototype_progress.md`、`docs/simulator_manual_regression_run_2026_05_19.md` 旧路径的文档

Read-only files:

- `docs/10_architecture/*.md`

Forbidden changes:

- 不修改文档正文含义。
- 不迁移 `decision_log.md`、`current_sim_architecture.md`、`simulator_manual_regression_matrix.md`。
- 不删除文件。
- 不顺带修改 `docs/20_guides/` 以外的指南规划。

#### 风险

- `page_reachability_audit.md` 与 `project_charter.md` 仍被少量当前说明文档提及；对策是把范围限定为路径修复，不改动这些当前说明文档的语义。

#### 验收标准

- [ ] `docs/80_history/` 已创建。
- [ ] 四份历史文档已迁入 `docs/80_history/` 且文件名不变。
- [ ] `document_map.md` 已统一为历史检索/历史归档入口。
- [ ] 必要旧路径链接已修复。
- [ ] `docs_root_reorg_inventory.md` 已同步为“已迁移”结论。
- [ ] `decision_log.md`、`current_sim_architecture.md`、`simulator_manual_regression_matrix.md` 未被迁移。

#### 执行记录

（Agent 填写：完成时间 / 实际改动文件 / 自检结果 / 风险回应）
