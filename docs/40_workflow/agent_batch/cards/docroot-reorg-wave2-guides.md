# DOCROOT Wave 2 Guides

本卡片包用于 docs 根目录第二轮整理中的 `20_guides` 迁移波次。

---

### DOCROOT-B8-C01 迁移开发指南到 20_guides

- 批次：8
- 状态：TODO
- 依赖：DOCROOT-B7-C01
- 自检：`git diff --check`；对本卡改动的中文文档执行 `AGENTS.md` 中规定的乱码哨兵检查；扫描 `cpp_minimal_glossary.md`、`learning_plan.md`、`simulator_reading_guide.md` 旧路径引用是否已清空
- 建议提交信息：`docs: move guide docs under 20_guides`
- Doc Impact：required

#### 问题定位

`cpp_minimal_glossary.md`、`learning_plan.md`、`simulator_reading_guide.md` 都属于按需阅读的开发指南，不应继续占据 `docs` 根目录默认入口。

#### 实施方案

1. 首次建立 `docs/20_guides/`。
2. 将 `cpp_minimal_glossary.md`、`learning_plan.md`、`simulator_reading_guide.md` 迁入 `docs/20_guides/`，文件名保持不变。
3. 更新 `docs/document_map.md`，新增“开发指南/按需阅读”路由，不把这三份文档放回默认新会话入口。
4. 修复仓库内对这三份文档旧路径的直接 Markdown 链接。
5. 更新 `docs/40_workflow/docs_root_reorg_inventory.md`，把这三项从第二轮候选改为已迁移。

#### 涉及位置

Allowed files:

- `docs/20_guides/*.md`
- `docs/document_map.md`
- `docs/40_workflow/docs_root_reorg_inventory.md`
- `docs/**/*.md` 中直接引用 `docs/cpp_minimal_glossary.md`、`docs/learning_plan.md`、`docs/simulator_reading_guide.md` 旧路径的文档

Read-only files:

- `docs/00_current/*.md`
- `docs/10_architecture/*.md`
- `docs/30_testing/*.md`

Forbidden changes:

- 不修改文档正文含义。
- 不顺带迁移 `simulator_manual_regression_matrix.md`。
- 不删除文件。
- 不创建 `80_history` 相关迁移内容。

#### 风险

- 如果把指南路由重新写回默认入口，会抵消前面的根目录降噪；对策是在 `document_map.md` 中明确它们只是按需开发指南。

#### 验收标准

- [ ] `docs/20_guides/` 已创建。
- [ ] 三份指南文档已迁入 `docs/20_guides/` 且文件名不变。
- [ ] `document_map.md` 已新增按需开发指南入口。
- [ ] 必要旧路径链接已修复。
- [ ] `docs_root_reorg_inventory.md` 已同步为“已迁移”结论。
- [ ] `simulator_manual_regression_matrix.md` 未被迁移。

#### 执行记录

（Agent 填写：完成时间 / 实际改动文件 / 自检结果 / 风险回应）
