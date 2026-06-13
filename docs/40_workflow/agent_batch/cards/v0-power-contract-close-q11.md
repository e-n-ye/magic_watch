# V0.5 P3-P4 Power 合同实现、边界复核与学习收口

## 卡片 V0.5-P3-B 纯 C PowerController 决策合同与测试

- ID：`V0.5-P3-B`
- 标题：纯 C PowerController 决策合同与测试
- 批次：V0.5 P3
- 状态：DONE
- 依赖：`V0.5-P3-A`

### 问题定位

`V0.5-P3-A` 已经把共享 Power 语义锁定为 `SCREEN_ON / SCREEN_OFF`、`Request -> Action -> Platform apply -> commit` 两阶段合同，但当前 `watch_core` 仍没有实际可调用的 Power 状态机实现。

如果继续停留在纯文档阶段，`V0.5` 还不能用代码证明以下关键判断：

- 请求阶段不会提前改写持久 Power 状态。
- 平台执行失败不会让 Core 漂移到错误状态。
- Power 状态与页面状态是两个独立状态机。
- 现有页面、快照和 UI 事件队列不会被 Power 合同污染。

### 实施方案

1. 在 `watch_core` public contract 中新增最小 Power 类型：
   - `WatchCorePowerState`
   - `WatchCorePowerRequest`
   - `WatchCorePowerActionType`
   - `WatchCorePowerAction`
   - `WatchCorePowerController`
2. 只在 `watch_core.h` / `watch_core.c` 内实现固定大小、无堆、表驱动的独立 PowerController，不新建源文件，不改工程登记。
3. `WatchCore` 组合 PowerController，但 Power 不进入现有 `WatchCoreUiEvent` 队列，也不新增 `SystemEvent`。
4. 提供最小公开接口：
   - 读取当前 Power 状态
   - 基于请求产出 Action
   - 基于平台执行结果 commit Action
5. `WatchCorePowerAction` 固定携带：
   - `type`
   - `source_state`
   - `target_state`
   以便在不引入异步 token 的前提下，拒绝“当前状态不匹配”的陈旧或非法 commit。
6. 纯 PC 合同测试至少覆盖：
   - 初始 `SCREEN_ON`
   - 请求阶段状态不变
   - 息屏失败 / 成功 commit
   - 唤醒失败 / 成功 commit
   - 重复息屏 / 重复唤醒返回 `NONE`
   - 非法或状态不匹配 commit 被拒绝
   - Power 往返不改变 `PageState`、snapshot 和现有 UI 队列合同
   - 原有全部合同测试继续通过
7. 同步最小必要文档，只写已成立代码事实，不写 F411 未执行的人工验证结果。

### 涉及位置

Allowed files

- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`
- `watch_core/tests/watch_core_contract_test.c`
- `docs/30_testing/v0_5_watch_core_contract_and_f411_ownership_audit.md`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/cards/v0-power-contract-close-q11.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `docs/40_workflow/agent_batch/cards/v0-power-semantics-q10.md`
- `docs/00_current/current_decisions.md`
- `sim/lv_port_pc_vscode/src/App/State/PowerController.*`
- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lite_view.*`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/**`

Forbidden changes

- 不修改 PC/F411 Adapter、Lite View、Input Port、LCD/背光驱动或 MDK 工程。
- 不新增 `ScreenOff` 页面、`SystemEvent`、timer、异步 token、`ScreenDim`、AOD、通知/抬腕/触摸唤醒或 LILYGO 代码。
- 不把 Power Action 变成 `PageIntent`，不把 Power 请求塞进现有 UI 事件队列。
- 不重写现有页面导航合同，不顺手重构 `watch_core` 其它模块。
- 不把旧 PC `PowerController` 或 `AppStateMachine` 原样搬入 `watch_core`。

### 风险

- 若把 Power 状态改写时机放在 request 阶段，平台失败会直接破坏共享状态一致性。
- 若 commit 校验太弱，后续平台接入时容易接受状态不匹配的错误 Action。
- 若把 Power 合同混进 UI 队列，会倒退回“一个大事件池包办所有语义”的旧问题。

### 自检

- `git status --short -uall`
- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
- `git diff --check`
- `rg -n "lvgl|SDL|HAL|CubeMX|malloc|free|new|delete|std::" watch_core`
- 对本轮实际改动的中文 Markdown 做乱码哨兵检查

### 验收标准

- `watch_core` 公开最小 Power 合同已存在，且保持纯 C、固定大小、无堆。
- 请求阶段不会修改持久 Power 状态。
- 平台失败不会导致 Power 状态错误迁移。
- 非法或状态不匹配的 commit 被拒绝。
- Power 往返不改页面状态、不改 snapshot、不污染现有 UI 队列语义。
- 现有全部合同测试继续通过。
- 文档已同步“Power 合同已进入 Core”，但未把 F411 编译或冒烟写成已通过。

### Doc Impact

`required`

### 建议提交信息

`feat: add watch core power decision contract`

### 执行记录

- 2026-06-13：卡片创建，待执行。
- 2026-06-13：开始执行；本轮只实现 `watch_core` 内的最小 Power 合同与纯 PC 测试，不接平台执行器。
- 2026-06-13：已在 `watch_core` 内新增最小 `PowerState / PowerRequest / PowerAction / commit` 合同；`cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过；`magic_watch_core_contract_test.exe` 通过。
- 2026-06-13：用户已回填 F411 Keil / MDK 编译通过；四卡首页、表冠切卡、表冠短按详情、触摸进详情、触摸 Back、tap-only、防误触、左边缘右滑 Back、中间起手不误返回、首页左边缘不误跳转均正常；未出现花屏、卡死、停更。

### Stop policy

- 本卡完成后停止，等待用户回填 F411 Keil / MDK 编译与一次原 UI 冒烟。

## 卡片 V0.5-P3-C Core 所有权复核与 V0.5 退出门

- ID：`V0.5-P3-C`
- 标题：Core 所有权复核与 V0.5 退出门
- 批次：V0.5 P3
- 状态：DONE
- 依赖：`V0.5-P3-B`

### 问题定位

`P3-B` 实现完成后，`watch_core` 将同时持有 UI 导航与最小 Power 状态合同。此时需要一次只读复核，确认没有因为新增 Power 合同而重新引入第二套权威状态、临时平台策略或“看起来像 SystemEvent 的影子结构”。

### 实施方案

1. 只读审计 `watch_core`、PC Adapter、F411 Adapter 与现有文档，形成最终所有权矩阵：
   - Input / InputIntent
   - UiEvent / EventQueue
   - PageState / PageIntent / Snapshot
   - PowerRequest / PowerAction / PowerState
   - Adapter / View / Platform Port
2. 明确以下判断是否成立：
   - 页面状态与 Power 状态是两个独立状态机
   - Power 不进入现有 UI 事件队列
   - Adapter 不拥有 Power 策略
   - Platform 不伪造成功、不成为策略源
   - 当前没有代码证据证明必须再加 `SystemEvent`、总线或新的总 Coordinator
3. 把 `P3-B` 的纯 PC 验证结果、用户回填的 F411 编译与原 UI 冒烟结果写回审计文档与卡片。
4. 若发现真实合同断裂，只记录证据并把本卡标为 `BLOCKED`；不在本卡顺手改代码。

### 涉及位置

Allowed files

- `docs/30_testing/v0_5_watch_core_contract_and_f411_ownership_audit.md`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/cards/v0-power-contract-close-q11.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_core/**`
- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lite_view.*`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/**`
- `docs/40_workflow/agent_batch/cards/v0-power-semantics-q10.md`

Forbidden changes

- 不修改任何运行代码、测试、工程文件或平台实现。
- 不新建 `SystemEvent`、新 Coordinator、总线、页面栈或 Power 平台执行器。
- 不把未执行的人工验证写成通过。

### 风险

- 如果审计和修代码混在一张卡里，很容易把“发现问题”和“修复问题”揉成一个大闭环，失去退出门判断价值。
- 如果不单独复核，V0.5 很可能在“功能看起来够了”时遗漏状态所有权漂移。

### 自检

- `git status --short -uall`
- `git diff --check`
- `rg -n "SystemEvent|PowerAction|PowerState|PageState|PageIntent|UiEvent" watch_core docs/30_testing/v0_5_watch_core_contract_and_f411_ownership_audit.md docs/00_current/magicwatch_long_term_roadmap.md`
- 对本轮实际改动的中文 Markdown 做乱码哨兵检查

### 验收标准

- 已形成最终所有权矩阵，并能指出每层权威源。
- 已明确当前是否还存在新的共享合同断裂点。
- 若无断裂点，已明确 V0.5 可以进入学习总结与 V0.6 交接。
- 若有断裂点，已给出证据和单独修复建议，但未越权改代码。

### Doc Impact

`required`

### 建议提交信息

`docs: audit v0.5 core ownership boundaries`

### 执行记录

- 2026-06-13：卡片创建，待执行。
- 2026-06-13：已基于 `watch_core`、PC Adapter、F411 Adapter 和审计文档形成最终所有权矩阵；确认页面状态与 Power 状态是两个独立状态机，Power 未进入 UI 事件队列，当前没有新的共享合同断裂点。
- 2026-06-13：已把 `P3-B` 的 F411 编译与原 UI 冒烟回填同步进审计结论；本卡收口后直接进入 `V0.5-P4-A`，不新增 `SystemEvent`、总线或新 Coordinator。

### Stop policy

- 本卡完成后停止，等待用户确认审计结论。

## 卡片 V0.5-P4-A 学习沉淀与 V0.6 交接

- ID：`V0.5-P4-A`
- 标题：学习沉淀与 V0.6 交接
- 批次：V0.5 P4
- 状态：DONE
- 依赖：`V0.5-P3-C`

### 问题定位

`V0.5` 对当前项目不只是代码阶段，也是学习沉淀阶段。若不单独把旧学习路线、当前真实实现路径和 V0.6 交界写清，后续窗口很容易继续按照过时的“从零实现 V0.5-A~G”理解项目。

### 实施方案

1. 新增阶段记录文档，收口 `V0.5` 最终学习结论与阶段完成定义。
2. 明确写清：
   - 为什么旧 `V0.5-A~G` 路线已经过时
   - 当前 `V0.5` 实际完成了哪些能力
   - 为什么现在只保留 `Power 合同实现 -> 边界复核 -> 学习总结`
   - `V0.6` 从哪些平台执行工作开始
3. 更新 `magicwatch_personal_growth_plan.md` 中已过时的 `V0.5-A~G` 表述，使其改成当前真实学习路径，而不是删除学习目标本身。
4. 更新 `current_decisions.md`、roadmap 和 `document_map.md` 的最小路由，使新窗口知道这份总结属于阶段记录，不是默认入口。
5. 不把 `V0.5` 写成整个 `V0` 已完成；只把 `V0.5` 的学习和 Core 合同阶段写成完成。

### 涉及位置

Allowed files

- `docs/50_stage_records/v0_5_core_contract_learning_closure.md`
- `docs/20_guides/magicwatch_personal_growth_plan.md`
- `docs/document_map.md`
- `docs/00_current/current_decisions.md`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/cards/v0-power-contract-close-q11.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `docs/30_testing/v0_5_watch_core_contract_and_f411_ownership_audit.md`
- `docs/40_workflow/agent_batch/cards/v0-power-semantics-q10.md`
- `watch_core/**`
- `sim/**`
- `try/my_watch_f411_v2.1/**`

Forbidden changes

- 不修改任何代码、测试、工程文件或平台实现。
- 不开始 V0.6 卡片实现。
- 不把未完成的平台 Power executor、背光门控或三平台闭环写成已实现。

### 风险

- 若只在聊天里解释、不落文档，后续窗口仍会把旧学习卡当当前计划。
- 若把 V0.5 和 V0.6 混写，会让“共享合同阶段”和“平台执行阶段”重新失焦。

### 自检

- `git status --short -uall`
- `git diff --check`
- `rg -n "V0\\.5-A|V0\\.5-B|V0\\.5-C|V0\\.5-D|V0\\.5-E|V0\\.5-F|V0\\.5-G" docs/20_guides/magicwatch_personal_growth_plan.md docs/50_stage_records/v0_5_core_contract_learning_closure.md docs/00_current/magicwatch_long_term_roadmap.md`
- 对本轮实际改动的中文 Markdown 做乱码哨兵检查

### 验收标准

- 已新增 `V0.5` 学习收口文档，并说明旧路线为何过时。
- 已更新成长计划中的过时 `V0.5-A~G` 表述。
- 已明确 `V0.6` 的起点是平台 Action executor，不是继续补 Core 名词。
- 文档路由已足够支撑新窗口继续工作而不误读阶段状态。

### Doc Impact

`required`

### 建议提交信息

`docs: close v0.5 core learning stage`

### 执行记录

- 2026-06-13：卡片创建，待执行。
- 2026-06-13：开始执行；本轮只做文档沉淀、路线修正和 `V0.6` 交界收口，不修改任何代码或平台实现。
- 2026-06-13：已新增 `V0.5` 学习收口阶段记录；已更新成长计划中过时的 `V0.5-A~G` 表述；已把 `V0.6` 起点收口为平台 Action executor，而不是继续补 Core 名词。
- 2026-06-13：`git diff --check` 通过（仅 LF/CRLF 提示）；中文文档乱码哨兵检查无命中；`V0.5-P4-A` 收口为 `DONE`。

### Stop policy

- 本卡完成后停止，等待用户确认 `V0.5` 学习收口结论。
