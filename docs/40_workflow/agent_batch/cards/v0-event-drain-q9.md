# V0.5 P2 共享事件处理完成语义与第三 Adapter 接入门

## 卡片 V0.5-P2-A Pending Event Drain 完成语义修正

- ID：`V0.5-P2-A`
- 标题：Pending Event Drain 完成语义修正
- 状态：DONE
- 依赖：`V0.5-P1-C`

### Problem

`watch_core_process_pending_events()` 当前把 `PAGE_INTENT_NONE` 同时当成“队列已空”和“本事件合法消费但不产生页面动作”的结束条件。这样一来，`首页 Back -> 合法卡片点击`、`非法 feature -> 合法卡片点击` 这类序列会在第一个 no-op 事件后提前停住，后续事件残留在队列里，违反了 `P1` 已批准的“单次外部输入后 drain 到稳定态”合同。

### Implementation plan

1. 只在 `watch_core` 内修正 pending drain 的完成判定，确保“事件无页面动作”不会提前终止 drain。
2. 保持现有公开 API 不变，不改 `WatchCorePageState`、`PageIntent`、Adapter、View 或输入代码。
3. 扩充纯 PC 合同测试，至少补齐：
   - `首页 Back -> 合法卡片点击`
   - `非法 feature -> 合法卡片点击`
   - drain 后队列已空
4. 运行 PC 构建与合同测试；F411 只要求编译冒烟，不要求大轮真机回归。

### Allowed files

- `watch_core/src/watch_core.c`
- `watch_core/tests/watch_core_contract_test.c`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/cards/v0-event-drain-q9.md`

### Read-only files

- `watch_core/include/watch_core/watch_core.h`
- `sim/lv_port_pc_vscode/src/XmlUi/**`
- `try/my_watch_f411_v2.1/**`

### Forbidden changes

- 不修改 `watch_core` 公共 API、结构体定义或其它运行语义。
- 不修改 PC/F411 Adapter、Lite View、Input Port、DMA、触摸阈值或电源代码。
- 不扩展页面、页面栈、第三平台或 Power 语义。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
- 对本轮实际修改的中文 Markdown 做乱码哨兵检查

### Acceptance checklist

- `首页 Back -> 合法卡片点击` 最终进入详情且队列不残留。
- `非法 feature -> 合法卡片点击` 仍能处理后一个事件。
- `卡片点击 -> Back` 最终回到首页。
- 空队列仍返回 no-op。
- 原合同测试继续通过。
- PC 构建和合同测试通过。
- F411 仅做编译冒烟，不写成真机已通过。

### Risks

- 如果顺手改公开 API，会把本卡从“确定性缺陷修正”扩大成新一轮契约设计。
- 如果同时改 Adapter，会让回归来源重新混淆。

### Doc Impact

`required`

### Suggested commit message

`fix: drain pending watch core events to queue empty`

### Execution record

- 2026-06-12：已基于代码审计确认当前缺口：`watch_core_process_pending_events()` 使用 `intent.type == WATCH_CORE_PAGE_INTENT_NONE` 作为循环终止条件，无法区分“队列已空”和“事件无页面动作但已消费”。
- 2026-06-12：已将 pending drain 终止条件改为“持续消费直到 `core->queue_count == 0U`”，并保持“返回最后一个非 `NONE` 的 `PageIntent`”这一现有公开行为不变，因此未修改任何公共 API。
- 2026-06-12：已补充纯 PC 合同测试覆盖：
  - `首页 Back -> 合法卡片点击`
  - `非法 feature -> 合法卡片点击`
  - drain 后队列为空
- 2026-06-12：`cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过；`sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe` 运行通过并输出 `watch_core contract tests passed.`。
- 2026-06-12：本卡未执行 F411 Keil 编译或真机冒烟；该项保持未验证，不能写成通过。

### Stop policy

- 完成后可继续执行 `V0.5-P2-B`，无需等待单独人工验收；若验证失败则停止。

---

## 卡片 V0.5-P2-B 第三 Adapter 接入合同核验与 V0.5 退出门

- ID：`V0.5-P2-B`
- 标题：第三 Adapter 接入合同核验与 V0.5 退出门
- 状态：TODO
- 依赖：`V0.5-P2-A`

### Problem

`P2-A` 修正后，需要把“第三 Adapter 至少要遵守什么”“入队失败、无导航动作、最终页面状态与 snapshot 应如何消费”“V0.5 是否已具备退出到 Power 规划的门槛”写成清晰检查表，否则下一轮很容易又回到各平台各自猜测 drain、页面权威源和错误处理的状态。

### Implementation plan

1. 只读复核 `watch_core`、PC Adapter、F411 Adapter 在 `P2-A` 后的实际消费语义。
2. 输出第三 Adapter 最小接入检查表，至少覆盖：
   - `push_event()` 失败时 Adapter 责任
   - drain 直到队列空，而不是直到 `PageIntent == NONE`
   - snapshot 与最终 `PageState` 的同步次序
   - 本地页面缓存不是权威源
   - View 创建/应用失败时的最小策略
3. 明确给出一个结论：
   - `V0.5` 可进入 Power 语义规划；或
   - 仍需先补一张已证实的共享合同卡
4. 只更新文档和队列，不实现第三平台或 Power。

### Allowed files

- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/30_testing/v0_5_watch_core_contract_and_f411_ownership_audit.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/cards/v0-event-drain-q9.md`

### Read-only files

- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`
- `sim/lv_port_pc_vscode/src/XmlUi/**`
- `try/my_watch_f411_v2.1/user/app/**`

### Forbidden changes

- 不修改任何运行代码、测试、Adapter、View、Input Port、Power 或第三平台实现。
- 不把规划写成已完成实现。
- 不直接进入 Power、页面扩张或第三平台代码接入。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
- 对本轮实际修改的中文 Markdown 做乱码哨兵检查

### Acceptance checklist

- 明确 `push_event()` 失败时 Adapter 的责任。
- 明确“无导航动作”不能终止 drain。
- 明确 Adapter 只按最终 `PageState` 和 snapshot 同步 View。
- 明确本地页面缓存不是权威源。
- 明确 View 创建/应用失败后的最小策略。
- 只形成一个结论，不实现第三平台或 Power。

### Risks

- 如果没有代码证据支撑，P2-B 容易退化成空泛架构宣言。
- 如果越界去改 Adapter 或 Power，会破坏本轮最小闭环。

### Doc Impact

`required`

### Suggested commit message

`docs: define third adapter entry contract`

### Execution record

- 2026-06-12：卡片已创建，待 `V0.5-P2-A` 完成后按实际代码结果核验，不提前写成通过。

### Stop policy

- 完成后停止，等待用户确认；不进入 Power 规划或其它新卡。
