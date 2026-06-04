# DOCROOT Wave 3 Decision Audits

本卡片包用于 docs 根目录剩余“待单独决策”对象的第三轮审计与最终复核。

---

### DOCROOT-B10-C03 审计 simulator_manual_regression_matrix 目录归属

- 批次：10
- 状态：DONE
- 依赖：DOCROOT-B9-C01
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查；扫描 `simulator_manual_regression_matrix.md` 旧路径引用是否已统一；重点核对 `docs/30_testing/manual_ui_regression_checklist.md` 是否清除错误绝对路径
- 建议提交信息：`docs: audit regression matrix routing`
- Doc Impact：required

#### 问题定位

`simulator_manual_regression_matrix.md` 实际上是测试资产，但仍停留在 docs 根目录；同时已有 `docs/30_testing/manual_ui_regression_checklist.md` 作为更短入口，而且当前还出现了 `watch_docs_sync` 的旧绝对路径线索，说明测试文档路由尚未收口。

#### 实施方案

1. 审计完整矩阵与 `30_testing` 目录下测试入口文档的分工。
2. 判断矩阵应迁入 `docs/30_testing/` 还是继续保留根目录，并给出明确路由。
3. 修复仓库内对该矩阵的旧路径引用，尤其是 `manual_ui_regression_checklist.md` 中的错误绝对路径。

#### 涉及位置

Allowed files:

- `docs/simulator_manual_regression_matrix.md`
- `docs/30_testing/*.md`
- `docs/document_map.md`
- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/**/*.md` 中直接引用 `docs/simulator_manual_regression_matrix.md` 的文档

Read-only files:

- `docs/00_current/*.md`

Forbidden changes:

- 不把本轮扩成自动化测试设计。
- 不顺带迁移 `simulator_manual_regression_run_2026_05_19.md`。
- 不修改任何模拟器代码。

#### 风险

- 这是“测试入口”和“历史运行记录”分工问题，不是单纯目录搬运；若判断错误，测试路由会比现在更乱。

#### 验收标准

- [x] `simulator_manual_regression_matrix.md` 的目录归属已有明确结论。
- [x] `manual_ui_regression_checklist.md` 与其它引用文档的路径已一致。
- [x] `document_map.md` 与盘点表已反映最终测试路由。

#### 执行记录

- 完成时间：2026-06-04
- 实际改动文件：`docs/30_testing/simulator_manual_regression_matrix.md`、`docs/30_testing/manual_ui_regression_checklist.md`、`docs/document_map.md`、`docs/40_workflow/docs_root_reorg_inventory.md`、`docs/50_stage_records/v0_4_system_skeleton_entry.md`、`docs/50_stage_records/v0_5_long_battery_regression_closure.md`、`docs/50_stage_records/v0_shell_baseline.md`、`docs/60_visual_exploration/settings_visual_reference.md`、`docs/70_hardware_reference/hardware_frontier_entry.md`、`docs/80_history/prototype_progress.md`、`docs/80_history/simulator_manual_regression_run_2026_05_19.md`、`docs/40_workflow/agent_batch/cards/docroot-reorg-wave3-decisions.md`、`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`
- 自检结果：`git diff --check` 通过；`watch_docs_sync` 错误绝对路径已清除；`docs/simulator_manual_regression_matrix.md` 旧路径真实正文引用已清空，仅剩 `docs/decision_log.md` 中的历史示例文本与当前卡片文本中的旧路径字符串；本轮实际改动中文文档乱码哨兵检查通过
- 风险回应：本轮结论已明确为“完整矩阵迁入 `docs/30_testing/` 与更短回归清单同层”；未改动 `simulator_manual_regression_run_2026_05_19.md` 的历史归档位置，只修复它对矩阵的新路径引用

---

### DOCROOT-B10-C01 审计 current_sim_architecture 根目录归属

- 批次：10
- 状态：DONE
- 依赖：DOCROOT-B9-C01
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查；扫描 `current_sim_architecture.md` 旧路径引用与 `current_architecture.md` 路由说明是否一致
- 建议提交信息：`docs: audit current_sim_architecture routing`
- Doc Impact：required

#### 问题定位

`docs/current_sim_architecture.md` 与 `docs/00_current/current_architecture.md` 名称过近，但角色明显不应相同；不先澄清，就无法判断它应保留根目录、迁入历史，还是需要补一层显式路由说明。

#### 实施方案

1. 审计 `current_sim_architecture.md` 的实际内容定位、引用来源和与 `00_current/current_architecture.md` 的职责重叠。
2. 明确它是“当前事实基线补充”“历史审计材料”还是“应合并/降级引用”的对象。
3. 只在证据充分后，更新 `document_map.md`、`docs_root_reorg_inventory.md` 和必要直接引用。

#### 涉及位置

Allowed files:

- `docs/current_sim_architecture.md`
- `docs/document_map.md`
- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/**/*.md` 中直接引用 `docs/current_sim_architecture.md` 或 `docs/00_current/current_architecture.md` 且需要路由澄清的文档

Read-only files:

- `docs/00_current/*.md`
- `docs/10_architecture/*.md`

Forbidden changes:

- 不修改模拟器代码。
- 不顺带整理 `decision_log.md`。
- 不顺带迁移测试矩阵文档。

#### 风险

- 如果只看文件名而不看实际引用角色，容易把“当前入口”和“旧现状地图”误判成重复文档。

#### 验收标准

- [x] `current_sim_architecture.md` 的角色已有明确结论。
- [x] 如需迁移或降级，`document_map.md` 与盘点表已同步。
- [x] 不与 `docs/00_current/current_architecture.md` 形成双权威入口。

#### 执行记录

- 完成时间：2026-06-04
- 实际改动文件：`docs/80_history/current_sim_architecture.md`、`docs/current_sim_architecture.md`、`docs/document_map.md`、`docs/40_workflow/docs_root_reorg_inventory.md`、`docs/40_workflow/agent_batch/cards/docroot-reorg-wave3-decisions.md`、`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`
- 自检结果：`current_sim_architecture.md` 已迁入 `docs/80_history/`；`document_map.md` 已把它降为历史归档补读入口；`docs/current_sim_architecture.md` 的真实正文路径引用已清空，仅剩当前卡片文本和执行历史中的旧路径字符串；本轮实际改动中文文档乱码哨兵检查通过；`git diff --check` 通过（仅有 LF/CRLF 警告，无 diff 格式错误）
- 风险回应：本轮没有把 `current_sim_architecture.md` 合并进 `docs/00_current/current_architecture.md`，而是按其自述“第一版现状地图”角色迁入历史归档，从路由上明确“当前主线入口”与“旧模拟器现状材料”的分工

---

### DOCROOT-B10-C02 审计 decision_log 根目录归属

- 批次：10
- 状态：DONE
- 依赖：DOCROOT-B9-C01
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查；扫描 `decision_log.md` 旧路径引用与 `document_map.md` / `current_decisions.md` / `workflow.md` 的入口说明是否一致
- 建议提交信息：`docs: audit decision log routing`
- Doc Impact：required

#### 问题定位

`decision_log.md` 已被当前入口文档降为历史检索材料，但它仍被 `workflow.md`、阶段记录和旧说明引用；在没有证据前直接迁到 `80_history`，可能打断现有团队使用习惯。

#### 实施方案

1. 审计 `decision_log.md` 的实际使用方式、引用来源和入口角色。
2. 判断它应继续保留根目录作为“历史检索但稳定可预期位置”，还是迁入 `80_history` 并补齐路由。
3. 只修复直接路径、入口说明和盘点表，不改历史决策正文语义。

#### 涉及位置

Allowed files:

- `docs/decision_log.md`
- `docs/document_map.md`
- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/00_current/current_decisions.md`
- `docs/workflow.md`
- `docs/**/*.md` 中直接引用 `docs/decision_log.md` 且需要路由修复的文档

Read-only files:

- `AGENTS.md`

Forbidden changes:

- 不重写历史决策内容。
- 不顺带清理其它历史文档。
- 不修改代码或测试矩阵正文。

#### 风险

- 这份文档兼具“长期记忆”与“历史检索”双重属性，若只按目录整洁性处理，可能削弱跨会话可追溯性。

#### 验收标准

- [x] `decision_log.md` 的根目录归属已有明确结论。
- [x] 与 `document_map.md`、`current_decisions.md`、`workflow.md` 的入口说法一致。
- [x] 若决定迁移，必要旧路径链接已修复。

#### 执行记录

- 完成时间：2026-06-04
- 实际改动文件：`docs/document_map.md`、`docs/00_current/current_decisions.md`、`docs/workflow.md`、`docs/40_workflow/docs_root_reorg_inventory.md`、`docs/40_workflow/agent_batch/cards/docroot-reorg-wave3-decisions.md`、`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`
- 自检结果：`decision_log.md` 保持原路径未迁移；`document_map.md`、`current_decisions.md`、`workflow.md` 对它的入口说法已统一为“非默认入口的稳定历史检索路径”；`docs/decision_log.md` 真实正文路径引用无需迁移修复；本轮实际改动中文文档乱码哨兵检查通过；`git diff --check` 通过（仅有 LF/CRLF 警告，无 diff 格式错误）
- 风险回应：本轮没有为了目录整洁性强行把 `decision_log.md` 迁入 `80_history`；考虑到 `workflow.md` 仍把它当历史记忆落点、`AGENTS.md` 仍列它为关键背景，保留根目录更能避免跨会话检索入口漂移

---

### DOCROOT-B10-C04 复核 docs 根目录是否已干净

- 批次：10
- 状态：DONE
- 依赖：DOCROOT-B10-C01, DOCROOT-B10-C02, DOCROOT-B10-C03
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查；重新扫描 `docs/*.md` 根目录清单、`document_map.md`、`docs_root_reorg_inventory.md`、关键旧路径引用
- 建议提交信息：`docs: review docs root cleanliness`
- Doc Impact：required

#### 问题定位

只有在 3 个“待单独决策”项都被逐一处理后，才能做一次最终审核，判断 docs 根目录是否只剩应保留的稳定入口，而不是把“还没想清楚的例外”误判成已干净。

#### 实施方案

1. 重新核对 docs 根目录 `.md` 清单与 `docs_root_reorg_inventory.md` 结论是否一致。
2. 核对 `document_map.md` 是否已经覆盖所有根目录保留项与按需入口。
3. 若审核通过，在盘点表或独立审核卡中明确写出“当前 docs 已干净”的判断依据；若不通过，列出剩余阻塞项。

#### 涉及位置

Allowed files:

- `docs/document_map.md`
- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/cards/*.md`
- `docs/**/*.md` 中本轮审核必需的直接引用修复

Read-only files:

- `AGENTS.md`

Forbidden changes:

- 不新增新的大类迁移桶，除非前三张卡已明确证明需要。
- 不顺带做与 docs 根目录清洁判断无关的文档润色。

#### 风险

- 如果没有独立审核卡，容易把“迁完几份文档”误当成“根目录已经干净”，而忽略剩余错误路径或路由矛盾。

#### 验收标准

- [x] `docs_root_reorg_inventory.md` 不再保留未处理的“待单独决策”项，或明确说明仍未通过审核的原因。
- [x] `document_map.md` 与根目录实际清单一致。
- [x] 可以明确判断“当前 docs 已干净”或“尚未干净且阻塞项是什么”。

#### 执行记录

- 完成时间：2026-06-04
- 实际改动文件：`docs/40_workflow/docs_root_reorg_inventory.md`、`docs/40_workflow/agent_batch/cards/docroot-reorg-wave3-decisions.md`、`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`
- 自检结果：`docs/*.md` 根目录当前仅剩 7 份保留项，和 `docs_root_reorg_inventory.md` / `document_map.md` 结论一致；根目录候选旧路径扫描未发现新的当前正文残留，仅剩历史会话记录、卡片说明文本和 `docs/decision_log.md` 的历史事故示例；本轮实际改动中文文档乱码哨兵检查通过；`git diff --check` 通过（仅有 LF/CRLF 警告，无 diff 格式错误）
- 风险回应：本轮只做独立复核与结论落档，不提前清理已验收队列项或旧卡片文件；这样能把“审核是否通过”和“验收后清理”分成两个可独立回退的闭环
