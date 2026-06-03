# DOCROOT Cleanup After Q6

本卡用于在 `DOCROOT-Q6-REVIEW` 验收通过后，按协议清理已完成队列项和已验收卡片文件。

---

### DOCROOT-B7-C01 清理已验收 DOCROOT 队列项与卡片文件

- 批次：7
- 状态：DONE
- 依赖：DOCROOT-B6-C01
- 自检：`git diff --check`
- 建议提交信息：`docs: clean accepted docroot queue items and cards`
- Doc Impact：required

#### 问题定位

`DOCROOT-Q6-REVIEW` 已验收通过，`docroot-reorg-wave1.md` 与 `docroot-reorg-review.md` 两个旧卡片文件也已完成使命，需要按验收后收尾协议清理，避免继续占据当前执行入口。

#### 实施方案

1. 从 `agent-queue.md` 移除已验收的 `DOCROOT-Q6-REVIEW`。
2. 删除已验收卡片文件 `docroot-reorg-wave1.md` 与 `docroot-reorg-review.md`。
3. 在 `agent-progress.md` 追加会话记录，注明清理的队列项、卡片文件与对应提交号。
4. 单独提交本次清理。

#### 涉及位置

Allowed files:

- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/cards/docroot-reorg-wave1.md`
- `docs/40_workflow/agent_batch/cards/docroot-reorg-review.md`
- `docs/40_workflow/agent_batch/cards/docroot-reorg-cleanup-after-q6.md`

Read-only files:

- `docs/40_workflow/agent_batch/agent-plan.md`
- `docs/document_map.md`

Forbidden changes:

- 不创建第二轮迁移内容。
- 不修改 `agent-plan.md`。
- 删除文件时只能一次删除一个明确路径。

#### 风险

- 若在清理时误删仍需参考的卡片文件，会丢失执行上下文；对策是只删除已验收且已执行完的两个旧卡片文件，并把执行事实保留在 `agent-progress.md` 与本卡。

#### 验收标准

- [ ] `agent-queue.md` 不再包含 `DOCROOT-Q6-REVIEW`。
- [ ] `docroot-reorg-wave1.md` 已删除。
- [ ] `docroot-reorg-review.md` 已删除。
- [ ] `agent-progress.md` 已记录清理内容与对应提交号。

#### 执行记录

- 完成时间：2026-06-04 00:20
- 实际改动文件：`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`、`docs/40_workflow/agent_batch/cards/docroot-reorg-cleanup-after-q6.md`
- 自检结果：`git diff --check` 通过
- 风险回应：仅清理已验收的 DONE 队列项与旧卡片文件；第二轮迁移卡未在本提交创建
