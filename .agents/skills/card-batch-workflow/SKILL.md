---
name: card-batch-workflow
description: Use when the Magic Watch project needs card-based batch planning or execution, including turning requirements into agent cards, running the current batch in goal mode, stopping for user review after a batch, or coordinating an execution window with a separate planning window that adds future cards.
---

# Card Batch Workflow

Use this skill to run Magic Watch work as bounded task cards instead of long free-form prompts.

This skill is an orchestration workflow. It does not replace `doc-memory-workflow`: use both when a card creates or updates persistent architecture, status, testing, or routing facts.

## Runtime Files

Project runtime state lives under:

- `docs/40_workflow/agent_batch/agent-rules.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-plan.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/agent-inbox.md`

Do not create card runtime files directly under `docs/`.

## Modes

### Planning Mode

Use when the user wants to discuss scope, split work, generate cards, or prepare future work.

Planning Mode has two cases:

- Normal planning: no other execution window is actively using the runtime files. In this case, write confirmed cards directly into `agent-plan.md`, `agent-queue.md`, or `cards/` as appropriate.
- Parallel planning: another execution window may be actively executing the current batch or queue item. In this case, use `agent-inbox.md` only as a temporary draft pool, unless the new cards clearly belong to a future queue entry and do not modify the active queue item.

1. Read `agent-rules.md`, `agent-queue.md`, `agent-plan.md`, `agent-progress.md`, and `agent-inbox.md`.
2. Generate cards with concrete batch or queue entry, dependency, allowed files, forbidden changes, self-checks, Doc Impact, suggested commit message, and stop policy.
3. In normal planning, write confirmed cards directly to the formal runtime files:
   - short inline cards -> `agent-plan.md`
   - longer grouped work -> `docs/40_workflow/agent_batch/cards/`
   - execution entry -> `agent-queue.md`
4. Use `agent-inbox.md` only when planning must be staged temporarily because another execution window may still be using the active batch, active queue item, or current session block.
5. If the execution window is running, add new cards only to `agent-inbox.md` or to a future queue entry; do not change the active queue item.
6. Do not edit the current batch, active queue item, or current session block while another execution window may be using it.
7. Ask the user to confirm batch, queue order, dependencies, and stop policy before writing normal-planning cards into the formal runtime files, or before moving parallel-planning drafts out of `agent-inbox.md`.

### Execution Mode

Use when the user asks to run goal mode, execute the current batch, or continue cards.

1. Run `git status --short -uall`. If the worktree is dirty outside the current card scope, stop.
2. Read `agent-rules.md`, `agent-queue.md`, `agent-plan.md`, and `agent-progress.md` in full.
3. Read the "do not repeat" section in `agent-progress.md`.
4. If `agent-queue.md` has active queue entries, announce the queue items and stop policy; otherwise announce the current batch and TODO cards.
5. Append one new session block to `agent-progress.md`; never edit older session blocks.
6. Execute only TODO cards from the active queue item or current batch, in declared order.
7. For each card, update status to IN_PROGRESS, perform only the allowed work, run self-checks, update execution record, mark DONE or BLOCKED, and commit if allowed.
8. Stop according to the queue item's stop policy, or when the current batch is complete, blocked, or reaches the configured card limit.

### Review Mode

Use when the user wants to inspect a completed batch.

1. Summarize DONE and BLOCKED cards.
2. Report changed files, commits, self-checks, manual checks not executed, Doc Impact, and risks.
3. Do not start the next batch until the user explicitly asks.
4. After explicit user approval, clean accepted DONE queue items, DONE cards, and accepted stale drafts; if the user requested deletion, delete accepted card files one explicit path at a time; record cleanup in `agent-progress.md` and commit separately.

## Hard Rules

- `agent-plan.md` is the source of truth for inline card status; card files are the source of truth for cards stored in those files.
- `agent-queue.md` is the execution entry when it has active queue items.
- `agent-progress.md` stores the current batch pointer, auto-commit setting, card limit, do-not-repeat list, and session history. It must not mirror the full status table.
- `agent-inbox.md` is only a temporary draft pool for cards that must be staged during parallel planning or multi-window coordination. It is not the default destination for ordinary single-window planning.
- A card's `Allowed files`, `Read-only files`, and `Forbidden changes` are scope locks.
- If code or docs require edits outside a card's allowed files, mark the card BLOCKED unless the user expands scope.
- If a dependency is TODO, IN_PROGRESS, or BLOCKED, do not execute dependent cards.
- A completed batch must stop for user review. Do not automatically begin the next batch.
- A queue item can opt into continuous execution when the user has approved that stop policy.
- If a batch has more cards than the configured card limit, stop after reaching the limit even if more TODO cards remain.
- Code cards default to `git status --short -uall`, `cmake --build sim/lv_port_pc_vscode/build --config Debug`, and `git diff --check` unless the card says otherwise.
- Chinese docs edited in a card must run the garble sentinel defined in `AGENTS.md` on the touched Chinese files.
- Commit once per DONE card when auto-commit is enabled. Do not split a card into multiple commits unless the card explicitly requires it.

## Card Shape

Every card in `agent-plan.md` must include:

- ID and title
- Batch
- Status: TODO, IN_PROGRESS, DONE, or BLOCKED
- Dependencies
- Self-check
- Suggested commit message
- Doc Impact
- Problem
- Implementation plan
- Scope with Allowed files, Read-only files, and Forbidden changes
- Risks
- Acceptance checklist
- Execution record

Cards missing required fields are invalid. Mark them BLOCKED and ask the user to fix the card.
