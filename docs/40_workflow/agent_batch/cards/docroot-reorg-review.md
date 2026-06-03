# DOCROOT Review 卡片包

本卡片包用于 DOCROOT wave1 迁移后的二次评审。只有在 `DOCROOT-B3-C01`、`DOCROOT-B4-C01`、`DOCROOT-B5-C01` 已完成并验收后才执行。

---

### DOCROOT-B6-C01 残余根目录文档二次评审

- 批次：6
- 状态：DONE
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

- 完成时间：2026-06-03 23:50
- 实际改动文件：`docs/40_workflow/docs_root_reorg_inventory.md`、`docs/document_map.md`、批处理运行文件
- 自检结果：`git diff --check` 通过；本轮中文文档乱码哨兵检查通过
- 风险回应：只形成“明确保留根目录 / 第二轮候选 / 待单独决策”结论，不直接执行第二轮迁移；未改动任何根目录正文文档
