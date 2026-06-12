# V0.5 P1 共享导航合同澄清

## 卡片 V0.5-P1-A 页面状态与 Adapter 消费契约决策

- ID：`V0.5-P1-A`
- 标题：页面状态与 Adapter 消费契约决策
- 状态：DONE
- 依赖：`V0.5-P0-C`

### Problem

`PageIntent` 当前同时被用作持久页面状态、瞬时跳转动作、Adapter 页面镜像和 View 当前视图状态。PC 与 F411 的初始化和事件消费策略也不一致。如果不先澄清“状态属于谁、谁消费、谁可以镜像、谁不能自行猜首页”，下一轮直接编码很容易把公共 API、两个 Adapter 和测试一起震松。

### Implementation plan

1. 读取当前 `watch_core`、PC Adapter、F411 Adapter 和 Lite View 的最小必要代码与路线文档。
2. 明确区分三类对象：
   - 持久页面状态
   - 瞬时页面跳转动作
   - 平台侧 View/LVGL 对象状态
3. 比较并裁剪至少三种候选方案：
   - 增加独立 `PageId/PageState`，保留 `PageIntent` 为动作
   - 保留现有类型，但补公开当前路由读取合同
   - 由 Core 初始化后产出首个页面意图，Adapter 不再假设首页
4. 对每种方案说明：
   - 初始化规则
   - 队列 drain 规则
   - Adapter 是否允许保留镜像
   - 第三个平台最小接入合同
5. 只输出一套选定方案，并记录被否决方案及原因。
6. 同步更新队列、路线图和执行记录，不进入实现。

### Allowed files

- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/30_testing/v0_5_watch_core_contract_and_f411_ownership_audit.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/cards/v0-navigation-contract-q8.md`

### Read-only files

- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`
- `sim/lv_port_pc_vscode/src/XmlUi/**`
- `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lite_view.*`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
- `try/my_watch_f411_v2.1/user/app/input/**`

### Forbidden changes

- 不修改 `watch_core` API、结构体或运行实现。
- 不修改 PC Adapter、F411 Adapter、Lite View、Input Port 或 MDK 工程。
- 不新增 `ScreenId`、`PowerController`、系统事件或新页面。
- 不把规划写成已完成实现。
- 不直接进入 `V0.5-P1-B` 或 `V0.5-P1-C`。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `rg -n "PageIntent|current_page|apply_snapshot|process_pending_events" watch_core sim/lv_port_pc_vscode/src/XmlUi try/my_watch_f411_v2.1/user/app`
- 对本轮实际修改的中文 Markdown 做乱码哨兵检查

### Acceptance checklist

- 明确区分持久页面状态、瞬时页面动作和平台 View 状态。
- 明确初始化时谁提供默认路由，Adapter 是否允许自行假设首页。
- 明确事件处理是一条一条消费还是 drain 到稳定态，以及该规则是否跨平台统一。
- 明确是否真的需要 `ScreenId`，而不是只改名。
- 给出未来第三 Adapter 的最小合同检查表。
- 不修改运行代码。

### Risks

- 如果只围绕命名争论，可能制造三份重复路由状态。
- 如果过早把电源语义拉进来，会把导航合同与亮灭屏语义缠在一起。
- 如果不区分“当前状态”和“页面动作”，后续测试会继续依赖偶然实现。

### Doc Impact

`required`

### Suggested commit message

`docs: plan shared navigation contract clarification`

### Execution record

- 2026-06-12：已核对 `watch_core` 当前事实：`watch_core_init()` 会把 `WatchCore.current_page` 初始化为健康四卡；`watch_core_process_next_event()` 既更新 `current_page`，又把同一个 `WatchCorePageIntent` 作为返回值吐给 Adapter。
- 2026-06-12：已核对 PC Adapter 当前事实：`watch_core_ui_adapter_init()` 初始化后直接 `watch_core_ui_adapter_load_health_shortcuts()`；事件处理采用 `drain_core_events()` 循环消费直到 `WATCH_CORE_PAGE_INTENT_NONE`。
- 2026-06-12：已核对 F411 当前事实：`f411_ui_adapter_init()` 在 `watch_core_init()` 之后手工把 `s_state.current_page` 设为健康四卡；`f411_ui_adapter_dispatch_event()` 每次只调用一次 `watch_core_process_next_event()`；`watch_lite_view.c` 还保留一份 `s_current_page` 作为视图展开状态。
- 2026-06-12：已确认真正缺口不是单纯增加 `ScreenId`，而是把“持久页面状态”“瞬时页面动作”“平台侧视图状态”三者合同拆清。仅加 `ScreenId` 会在 `current_page + PageIntent + Adapter/View 镜像` 之外再增加第四份命名状态，不能解决默认路由和消费策略分裂。
- 2026-06-12：本卡选定方案如下：
  1. 共享合同中拆分“页面状态”和“页面动作”。
  2. `PageIntent` 只保留为瞬时页面动作，不再承担长期权威状态语义。
  3. 下一卡 `P1-B` 应为 `watch_core` 增加独立的公开页面状态读取合同；该状态至少要能表达“当前页种类 + 详情 feature”，因此不是只加一个平面的 `ScreenId`，而应采用等价于 `PageState/PageRoute` 的结构。
  4. 默认页面来源归 `watch_core` 所有。Adapter 初始化后必须从 Core 读取公开页面状态，不得自行假设首页。
  5. 共享消费规则统一为“单次外部输入派发后，将 Core 队列 drain 到稳定态”。也就是 Adapter 推入一个 typed `UiEvent` 后，应持续调用处理接口直到返回 no-op，再统一应用 snapshot 和最终页面状态；平台若未来需要节流，只能在调度层做，不得改写语义。
  6. Adapter 可以保留“最后已应用页面状态”的本地缓存用于 view diff、focus 或动画，但该缓存不是权威源；Lite View 内部的展开/hidden 状态也只属于视图实现，不得反向决定产品导航。
- 2026-06-12：已否决的候选方案：
  - 仅增加 `ScreenId`：不足以承载详情 feature，且会制造重复状态命名。
  - 保留现有类型不拆分，只补当前路由 getter：仍会把 `PageIntent` 同时当作状态和动作，语义继续混叠。
  - 让 Core 初始化时直接吐出首个 `PageIntent`：虽然能去掉 Adapter 猜首页，但仍未解决“动作类型兼任长期状态”的混叠，收益不如先拆状态合同。
- 2026-06-12：已形成第三 Adapter 最小接入检查表：
  1. 只把平台输入翻译成 typed `UiEvent`。
  2. 初始化后先读取 Core 的公开页面状态和 snapshot，不自行猜默认页。
  3. 单次输入后 drain Core 到稳定态。
  4. 把 `PageIntent` 当动作消费，把公开页面状态当权威源读取。
  5. 本地页面镜像只做缓存，不做导航裁决。
  6. View 对象状态、动画、focus、screen/panel 切换都属于平台实现，不回写 Core 语义。

### Stop policy

- 完成后停止，等待用户确认，不进入 `V0.5-P1-B`。

---

## 卡片 V0.5-P1-B `watch_core` 导航合同护栏

- ID：`V0.5-P1-B`
- 标题：`watch_core` 导航合同护栏
- 状态：DONE
- 依赖：`V0.5-P1-A`

### Problem

在 `P1-A` 做出导航合同决策后，需要先用纯 PC 护栏把公共语义锁住，否则两个平台 Adapter 会继续靠各自假设消费事件和默认页面。

### Implementation plan

1. 只按 `P1-A` 的决议，对 `watch_core` 公共导航合同做最小必要调整。
2. 扩充或重写纯 PC 合同测试，覆盖：
   - 初始化默认路由证明方式
   - 当前页面状态读取或页面动作语义
   - 队列消费策略
3. 确认原有十类合同测试仍继续通过。
4. 不修改 PC/F411 Adapter，只把公共合同先锁住。

### Allowed files

- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`
- `watch_core/tests/**`
- `sim/lv_port_pc_vscode/CMakeLists.txt`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/cards/v0-navigation-contract-q8.md`

### Read-only files

- `sim/lv_port_pc_vscode/src/XmlUi/**`
- `try/my_watch_f411_v2.1/**`

### Forbidden changes

- 不修改 PC/F411 Adapter、Lite View、Input Port 或真机 UI。
- 不引入电源语义、系统事件或新页面。
- 不顺手抽取 `F411UiAdapter` 新职责。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
- `rg -n "lvgl|SDL|HAL|CubeMX|malloc|free|new|delete|std::" watch_core`
- 对本轮实际修改的中文 Markdown 做乱码哨兵检查

### Acceptance checklist

- 初始化路由、状态读取或页面动作语义可通过公开合同验证。
- 队列消费规则有明确测试覆盖。
- 原十类合同测试继续通过。
- `watch_core` 仍保持纯 C、固定容量、无平台依赖。
- PC/F411 现有代码至少保持编译层面可继续对接。

### Risks

- 如果 `P1-A` 方案不清楚，本卡很容易把实现细节误固化成公共 API。
- 如果同时修改 Adapter，会让失败来源无法定位。

### Doc Impact

`required`

### Suggested commit message

`test: tighten watch core navigation contract`

### Execution record

- 2026-06-12：已在 `watch_core` 公共合同中新增独立页面状态类型 `WatchCorePageState`，把“当前页面状态”从 `PageIntent` 动作语义中拆出。
- 2026-06-12：已新增公开 getter `watch_core_get_current_page_state()`，用于读取权威默认页和导航后页面状态，不再要求 Adapter 通过猜测首页或依赖内部字段获取当前路由。
- 2026-06-12：已新增 `watch_core_process_pending_events()`，把“单次外部输入后 drain 到稳定态”的共享消费规则固化为公共处理接口；现有 `watch_core_process_next_event()` 保留，供后续 Adapter 对齐前兼容使用。
- 2026-06-12：已扩充纯 PC 合同测试，新增覆盖：
  - 初始化后公开页面状态为健康四卡
  - 导航后公开页面状态可读
  - `process_pending_events()` 会 drain 到稳定态，并返回最后一个非 no-op 页面动作
- 2026-06-12：`cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过；`sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe` 运行通过并输出 `watch_core contract tests passed.`。
- 2026-06-12：本卡未修改 PC/F411 Adapter、Lite View、Input Port 或真机代码；现有 simulator 构建保持通过，说明下一卡可在不扩大 scope 的前提下进入 `P1-C`。

### Stop policy

- 完成后停止，等待用户确认，不进入 `V0.5-P1-C`。

---

## 卡片 V0.5-P1-C PC/F411 Adapter 合同对齐

- ID：`V0.5-P1-C`
- 标题：PC/F411 Adapter 合同对齐
- 状态：DONE
- 依赖：`V0.5-P1-B`

### Problem

即便公共合同被锁住，PC 与 F411 仍可能保留不同的默认页假设、事件消费节奏或页面镜像方式。本卡的目标是把两端收敛到同一共享导航合同，而不是统一 View 技术栈。

### Implementation plan

1. 按 `P1-B` 的公共合同，分别调整 PC Adapter 与 `F411UiAdapter` 的初始化和事件消费方式。
2. 保持：
   - PC 继续使用 XML View
   - F411 继续使用 Lite View
   - Input Port、触摸阈值、手势语义和视觉实现不动
3. 明确并收口：
   - 默认页面来源
   - `PageIntent` 消费规则
   - Snapshot 获取与应用时机
   - Adapter 是否保留镜像，镜像是否只做缓存而非权威源
4. 回归 PC 与 F411 的现有验收闭环。

### Allowed files

- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/cards/v0-navigation-contract-q8.md`

### Read-only files

- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`
- `try/my_watch_f411_v2.1/user/app/input/**`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lite_view.*`

### Forbidden changes

- 不改 Input Port 的原始坐标、tap-only、防误触、左边缘右滑和手感阈值。
- 不改 Lite View 的视觉布局和页面复杂度。
- 不引入电源语义、新页面、图标或性能 overlay。
- 不进入 `V0.5-P2`。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
- 对本轮实际修改的中文 Markdown 做乱码哨兵检查

### Acceptance checklist

- PC 四卡、详情、Back 回归通过。
- F411 原 12 项真机验收继续通过。
- 两端不再各自猜测默认页面。
- 两端事件消费策略一致，或差异有明确平台理由并写清。
- 不把 Input Port 和 View 职责重新卷回 Adapter。

### Risks

- 如果平台实现和公共合同同时变化，回归失败时很难定位问题。
- 如果 Adapter 重新接管业务导航，P0 的所有权收口会被破坏。

### Doc Impact

`required`

### Suggested commit message

`refactor: align pc and f411 adapter navigation contract`

### Execution record

- 2026-06-12：已将 PC Adapter 改为统一消费新合同：单次 `UiEvent` 入队后调用 `watch_core_process_pending_events()`，随后读取 `watch_core_get_current_page_state()` 和 snapshot，再按最终页面状态加载 shortcuts/detail。
- 2026-06-12：已移除 PC 初始化阶段“直接假设首页并 load shortcuts”的旧路径，改为初始化后立刻从 Core 读取默认页面状态并同步到 View。
- 2026-06-12：已将 F411 Adapter 改为统一消费新合同：单次事件后调用 `watch_core_process_pending_events()`，再读取 `watch_core_get_current_page_state()` 和 snapshot；`current_page` 本地缓存已改为 `WatchCorePageState`，不再把 `PageIntent` 当权威当前页面来源。
- 2026-06-12：F411 侧仍保留 `last_intent` 作为最近一次页面动作缓存，但页面显示和 swipe-back 判定已改为基于公开页面状态。
- 2026-06-12：`cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过；`sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe` 运行通过并输出 `watch_core contract tests passed.`。
- 2026-06-12：用户已回填人工验收：
  - PC 四卡首页正常显示
  - PC 点击任意卡片可进入对应详情
  - PC 详情页 `Back` 可返回四卡
  - 当前 PC 端仍未提供左边缘右滑退出语义；该项不属于本卡既有验收边界，也未被写成通过
  - F411 编译通过
  - F411 四卡、详情、Back、表冠原闭环正常
  - F411 原 12 项真机验收继续通过
  - 未出现花屏、卡死、停更
- 2026-06-12：基于上述回填，本卡状态收口为 `DONE`。

### Stop policy

- 完成后停止，等待用户确认。
