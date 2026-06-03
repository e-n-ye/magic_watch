# DOCROOT Wave 1 卡片包

本卡片包用于验证 `agent-queue.md` 队列入口。执行时按文件内顺序连续执行，完成整个队列项后停止等待用户验收。

---

### DOCROOT-B3-C01 迁移阶段记录到 50_stage_records

- 批次：3
- 状态：DONE
- 依赖：DOCROOT-B2-C01
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查
- 建议提交信息：`docs: move stage records under 50_stage_records`
- Doc Impact：required

#### 问题定位

`v0_*` 阶段记录数量最多，是根目录噪音的第一来源，但其中保留了阶段上下文，适合集中归档而不是删除或改名。

#### 实施方案

1. 仅迁移盘点文档中标记为阶段记录的 `v0_*` 文件到 `docs/50_stage_records/`，文件名保持不变。
2. 更新 `docs/document_map.md` 中涉及这些文件的入口说明，明确它们属于历史阶段记录，不回到默认入口。
3. 修复仓库内直接引用这些旧路径的必要 Markdown 链接。
4. 保持 `docs/00_current/v0_scope.md` 原位不动，仅迁移根目录历史版 `docs/50_stage_records/v0_scope.md`。

#### 涉及位置

Allowed files:

- `docs/50_stage_records/*.md`
- `docs/document_map.md`
- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/**/*.md` 中直接引用本卡迁移文件旧路径的文档

Read-only files:

- `docs/00_current/*.md`
- `docs/10_architecture/*.md`

Forbidden changes:

- 不删除文件。
- 不修改文档正文含义，只允许路径迁移和最小必要链接修复。
- 不改动 `docs/00_current/v0_scope.md` 的内容或位置。
- 不顺带迁移视觉、硬件或指南类文档。

#### 风险

- `v0_scope` 双版本语义容易被搞混；对策是把历史版与当前版在 `document_map.md` 中明确区分。

#### 验收标准

- [ ] 盘点文档标记的阶段记录已全部迁入 `docs/50_stage_records/`。
- [ ] 文件名保持不变。
- [ ] `document_map.md` 已更新对应路径与说明。
- [ ] 仓库内必要链接已修复。
- [ ] `docs/00_current/v0_scope.md` 未被误改或误迁。

#### 执行记录

- 完成时间：2026-06-03 22:30
- 实际改动文件：`docs/50_stage_records/*.md`、`docs/document_map.md`、`docs/40_workflow/docs_root_reorg_inventory.md`、`docs/decision_log.md`、`docs/prototype_progress.md`、`docs/60_visual_exploration/settings_visual_reference.md`、批处理运行文件
- 自检结果：`git diff --check` 通过；旧的 `docs/v0_*.md` / `docs/v0_scope.md` / `docs/v0_shell_baseline.md` 路径已在允许修改范围内完成替换；本轮中文文档乱码哨兵检查通过，`decision_log.md` 中命中的疑似乱码字符串是规则说明原文，不是新写入乱码
- 风险回应：保留 `docs/00_current/v0_scope.md` 原位不动；`document_map.md` 新增按需路由，明确 `50_stage_records` 只作为历史阶段记录入口，不回到默认新会话

---

### DOCROOT-B4-C01 迁移视觉探索与视觉流程到 60_visual_exploration

- 批次：4
- 状态：DONE
- 依赖：DOCROOT-B2-C01
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查
- 建议提交信息：`docs: move visual exploration docs under 60_visual_exploration`
- Doc Impact：required

#### 问题定位

视觉方向稿、视觉参考和视觉流程文档目前散落在根目录，影响当前工程入口的可读性。

#### 实施方案

1. 仅迁移盘点文档标记为视觉探索/视觉流程的文件到 `docs/60_visual_exploration/`。
2. 默认包含：`settings_visual_reference.md`、`weather_shortcut_visual_direction.md`、`weather_shortcut_implementation_spec.md`、`visual_asset_workflow.md`、`weather_icon_workflow.md`、`web_visual_sandbox_workflow.md`。
3. 更新 `docs/document_map.md` 中对应路由，标记为按需视觉资料。
4. 修复必要的 Markdown 链接。

#### 涉及位置

Allowed files:

- `docs/60_visual_exploration/*.md`
- `docs/document_map.md`
- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/**/*.md` 中直接引用本卡迁移文件旧路径的文档

Read-only files:

- `docs/00_current/*.md`
- `docs/10_architecture/*.md`
- `docs/30_testing/*.md`

Forbidden changes:

- 不删除文件。
- 不修改视觉方案内容本身，只做迁移与路径修复。
- 不迁移 `watchface_style_architecture.md`，它先视为架构文档，不纳入本卡。

#### 风险

- `watchface_style_architecture.md` 名称容易被误判为视觉资料；对策是本卡显式排除它。

#### 验收标准

- [ ] 视觉探索/流程文档已迁入 `docs/60_visual_exploration/`。
- [ ] `document_map.md` 已改成按需视觉资料路由。
- [ ] 必要链接已修复。
- [ ] `watchface_style_architecture.md` 保持原位。

#### 执行记录

- 完成时间：2026-06-03 23:05
- 实际改动文件：`docs/60_visual_exploration/*.md`、`docs/document_map.md`、`docs/prototype_progress.md`、批处理运行文件
- 自检结果：`git diff --check` 通过；视觉文档旧路径扫描在允许范围内无残留；中文文档乱码哨兵检查通过
- 风险回应：`watchface_style_architecture.md` 保持在根目录未迁移；`document_map.md` 已明确 `60_visual_exploration` 为按需视觉资料入口，不回到默认新会话

---

### DOCROOT-B5-C01 迁移硬件参考到 70_hardware_reference

- 批次：5
- 状态：TODO
- 依赖：DOCROOT-B2-C01
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查
- 建议提交信息：`docs: move hardware references under 70_hardware_reference`
- Doc Impact：required

#### 问题定位

硬件候选、边界探索和 TWatch 参考验证资料都属于按需参考，不应继续占据根目录入口。

#### 实施方案

1. 仅迁移盘点文档标记为硬件参考的文件到 `docs/70_hardware_reference/`。
2. 默认包含：`hardware_frontier_entry.md`、`hardware_component_selection_strategy.md`、`hardware_candidate_shortlist.md`、`hardware_candidate_constraints.md`、`twatch_s3_plus_reference_validation_plan.md`、`twatch_s3_plus_reference_validation_run.md`。
3. 更新 `docs/document_map.md` 中对应路由，明确这批文档是未来硬件接入前的参考材料。
4. 修复必要的 Markdown 链接。

#### 涉及位置

Allowed files:

- `docs/70_hardware_reference/*.md`
- `docs/document_map.md`
- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/**/*.md` 中直接引用本卡迁移文件旧路径的文档

Read-only files:

- `docs/00_current/*.md`
- `docs/10_architecture/*.md`

Forbidden changes:

- 不删除文件。
- 不修改硬件路线判断和正文结论。
- 不新增硬件选型结论。
- 不顺带修改 `sim/**` 或代码实现。

#### 风险

- 硬件参考容易和“当前硬件边界契约”混淆；对策是 `document_map.md` 里明确这是历史/按需参考，不是默认入口。

#### 验收标准

- [ ] 硬件参考文档已迁入 `docs/70_hardware_reference/`。
- [ ] `document_map.md` 已更新为按需参考路由。
- [ ] 必要链接已修复。
- [ ] 未新增任何硬件实现或选型结论。

#### 执行记录

（Agent 填写：完成时间 / 实际改动文件 / 自检结果 / 风险回应）
