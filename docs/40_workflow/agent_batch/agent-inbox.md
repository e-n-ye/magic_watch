# Agent 卡片草稿池

本文件用于并行规划窗口记录新需求或草稿卡片。

执行窗口正在运行时，其他窗口可以向本文件追加内容，但不得修改当前批次、当前会话块或正在执行的卡片。

## 使用规则

- 新需求先写入本文件，等待用户确认批次和依赖。
- 确认后的卡片再移动到 `agent-plan.md`。
- 不在本文件维护 DONE / BLOCKED 状态。

## 草稿

### DOCROOT-B2-C01 docs 根目录盘点与分类建议

- 批次：2
- 状态：TODO
- 依赖：无
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查
- 建议提交信息：`docs: inventory docs root and propose reorg buckets`
- Doc Impact：required

#### 问题定位

`docs` 根目录同时承载当前入口、历史阶段记录、视觉探索、硬件参考和流程资料，噪音过高；但直接批量迁移会把当前路由文档和历史文档混在一起。

#### 实施方案

1. 新增一份根目录盘点文档，列出当前根目录全部文件。
2. 对每个文件标记目标类别：保留根目录、迁入 `50_stage_records`、迁入 `60_visual_exploration`、迁入 `70_hardware_reference`、迁入 `20_guides`、迁入 `80_history`、或“待单独决策”。
3. 单独列出高风险冲突项，至少包含 `docs/v0_scope.md` 与 `docs/00_current/v0_scope.md` 的语义区别。
4. 给出后续迁移顺序建议，只提建议，不移动文件，不改 `document_map.md`。

#### 涉及位置

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

#### 风险

- 若盘点结论把“当前入口文档”误归为历史档，会导致后续迁移破坏新会话路由；对策是先显式列出“保留根目录 / 待单独决策”清单。

#### 验收标准

- [ ] 有一份完整的根目录盘点文档。
- [ ] 每个根目录文件都有明确类别建议。
- [ ] 明确列出高风险冲突项和暂不迁移项。
- [ ] 明确后续批次顺序。
- [ ] 本卡未移动文件、未修改 `document_map.md`、未修复链接。

#### 执行记录

（Agent 填写：完成时间 / 实际改动文件 / 自检结果 / 风险回应）

---

### DOCROOT-B3-C01 迁移阶段记录到 50_stage_records

- 批次：3
- 状态：TODO
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
4. 保持 `docs/00_current/v0_scope.md` 原位不动，仅迁移根目录历史版 `docs/v0_scope.md`。

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

（Agent 填写：完成时间 / 实际改动文件 / 自检结果 / 风险回应）

---

### DOCROOT-B4-C01 迁移视觉探索与视觉流程到 60_visual_exploration

- 批次：4
- 状态：TODO
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

（Agent 填写：完成时间 / 实际改动文件 / 自检结果 / 风险回应）

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

---

### DOCROOT-B6-C01 残余根目录文档二次评审

- 批次：6
- 状态：TODO
- 依赖：DOCROOT-B3-C01, DOCROOT-B4-C01, DOCROOT-B5-C01
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查
- 建议提交信息：`docs: review remaining root docs after reorg wave 1`
- Doc Impact：required

#### 问题定位

完成三类安全迁移后，根目录仍会留下混合角色文档，其中一部分可能应继续保留，一部分才适合进入 `20_guides` 或 `80_history`。

#### 实施方案

1. 复盘迁移后的根目录剩余文件。
2. 明确保留根目录的“当前入口/长期架构”名单。
3. 仅对确认为历史遗留或指南型的剩余文件提出第二轮迁移建议，不在本卡直接大搬运。
4. 若需要继续迁移，再拆下一批独立卡片。

#### 涉及位置

Allowed files:

- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/document_map.md`

Read-only files:

- `docs/*.md`
- `docs/00_current/*.md`
- `docs/10_architecture/*.md`
- `docs/20_guides/*.md`
- `docs/80_history/*.md`

Forbidden changes:

- 不直接批量迁移剩余文档。
- 不删除文件。
- 不改动当前入口文档正文含义。

#### 风险

- 如果把长期架构文档也机械迁走，会让新会话补读路径变长；对策是先评审，再决定是否新增后续卡。

#### 验收标准

- [ ] 有一份迁移后剩余根目录评审结论。
- [ ] 明确保留根目录名单。
- [ ] 明确第二轮候选迁移名单。
- [ ] 本卡不直接执行第二轮迁移。

#### 执行记录

（Agent 填写：完成时间 / 实际改动文件 / 自检结果 / 风险回应）
