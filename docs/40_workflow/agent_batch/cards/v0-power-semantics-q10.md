# V0.5 P3 Power / Wake / Screen On 共享语义规划

## 卡片 V0.5-P3-A Power / Wake / Screen On 共享语义决策

- ID：`V0.5-P3-A`
- 标题：Power / Wake / Screen On 共享语义决策
- 批次：V0.5 P3
- 状态：TODO
- 依赖：`V0.5-P2-B`

### Problem

`V0.5-P0` 到 `P2` 已经收口共享导航状态、事件 drain 和 Adapter 消费合同，但 Power 语义尚未进入新主线 `watch_core`。

旧 PC C++ 主线已经存在较完整的 `PowerController`、`AppStateMachine`、页面栈恢复和显示策略；这些代码只能作为行为证据，不能原样搬入纯 C `watch_core`。F411 当前产品输入只采用表冠旋转与按下，底层仍存在的 `WATCH_INPUT_WAKE_SHORT` / `WATCH_INPUT_WAKE_LONG` 属于历史遗留，不进入当前产品合同。

如果不先区分共享逻辑状态、平台硬件执行和页面状态，下一轮容易把 PC 全量产品行为塞进 Core，或让 F411 / LILYGO 平台重新拥有产品级 Power 决策。

### Locked premises

本卡执行时不得重新打开以下已确认前提：

1. `ScreenOff` 是系统显示状态，不是页面身份，不新增 `ScreenId`。
2. F411 当前 Power 输入只考虑表冠：
   - 熄屏时表冠按下请求唤醒。
   - 该次按下只负责唤醒，不同时确认卡片。
   - 熄屏时表冠旋转忽略，不唤醒、不切卡。
3. `WATCH_INPUT_WAKE_SHORT` / `WATCH_INPUT_WAKE_LONG` 是历史遗留，不纳入当前共享 Power 合同。
4. PC 旧 C++ `PowerController` 只作为行为和边界参考，不作为新主线移植源。
5. V0.5 只定义共享逻辑语义；真实背光、LCD、MCU sleep 和硬件唤醒源闭环属于 V0.6 平台执行。

### Implementation plan

1. 只读审计旧 PC Power 路径、新主线 `watch_core`、PC/F411 Adapter 和 F411 表冠输入链。
2. 形成最小 Power 状态转移表，优先验证 `ScreenOn` / `ScreenOff` 是否足以支撑首个共享闭环。
3. 形成输入与状态规则矩阵，至少覆盖：
   - 亮屏时表冠旋转与按下继续进入现有导航语义。
   - 熄屏时表冠按下只请求唤醒。
   - 熄屏时表冠旋转被忽略。
   - 重复息屏、重复唤醒和平台执行失败。
4. 明确职责边界：
   - Power 决策核拥有共享逻辑 Power 状态和转换规则。
   - Coordinator 负责协调 Power Action、页面状态和 Adapter。
   - Adapter 只翻译输入结果、派发共享语义并应用最终结果。
   - Platform Port 只执行背光、显示和未来 MCU 低功耗动作，不决定产品策略。
5. 明确 Power 决策与平台执行是否采用“请求 Action -> 平台执行 -> 成功后提交状态”的两阶段合同，避免硬件失败时共享状态提前漂移。
6. 输出下一实现卡 `V0.5-P3-B` 的唯一建议方案、范围和验收标准，但不创建或执行 `P3-B`。
7. 明确 V0.5 与 V0.6 的交界：V0.5 到哪一层结束，三平台硬件闭环从哪里开始。

### Allowed files

- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/30_testing/v0_5_watch_core_contract_and_f411_ownership_audit.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/cards/v0-power-semantics-q10.md`

### Read-only files

- `docs/00_current/project_brief.md`
- `docs/00_current/current_decisions.md`
- `docs/00_current/current_architecture.md`
- `docs/10_architecture/state_machine.md`
- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `watch_core/**`
- `sim/lv_port_pc_vscode/src/App/State/PowerController.*`
- `sim/lv_port_pc_vscode/src/App/State/AppStateMachine.*`
- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/input/**`
- `try/my_watch_f411_v2.1/user/services/input/**`
- `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`

### Forbidden changes

- 不修改任何代码、测试、公共 API、工程文件或生成文件。
- 不实现 PowerController、Power 状态、Power Action 或新的 `UiEvent`。
- 不修改 PC/F411 Adapter、Lite View、Input Port 或硬件驱动。
- 不把旧 PC `PowerController`、`AppStateMachine`、页面栈或 LVGL timer 搬入 `watch_core`。
- 不把 `ScreenOff` 建模成页面，不新增 `ScreenId`。
- 不进入 `ScreenDim`、`WakePending`、`PoweredOff`、AOD、通知唤醒、抬腕唤醒、触摸唤醒、长续航或亮度策略实现。
- 不接 LILYGO，不进入 V0.6，不扩页面。
- 不创建或提前实现 `V0.5-P3-B`。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `rg -n "WATCH_INPUT_WAKE|WATCH_INPUT_ENCODER|WATCH_INPUT_INTENT" try/my_watch_f411_v2.1/user/app try/my_watch_f411_v2.1/user/services`
- `rg -n "PowerController|PowerState|PowerAction|ScreenOff|WakeScreen" sim/lv_port_pc_vscode/src/App/State docs/00_current docs/10_architecture`
- 对本轮实际修改的中文 Markdown 做乱码哨兵检查

### Acceptance checklist

- 清楚区分 Power 逻辑状态、`WatchCorePageState` 和平台硬件状态。
- 给出唯一一套最小状态转移方案，并记录被否决的扩大方案及原因。
- 明确 `ScreenOff` 不是页面，Power 切换不得自行改写当前页面。
- 明确 F411 只采用表冠输入，历史 Wake 键不进入合同。
- 明确熄屏时表冠按下只唤醒、表冠旋转忽略。
- 明确 Controller、Coordinator、Adapter、Platform Port 的责任。
- 明确平台执行失败时共享状态如何保持一致。
- 明确 V0.5 与 V0.6 的交界。
- 给出 `V0.5-P3-B` 的目标、Allowed / Forbidden 范围、纯 PC 测试场景和 F411 验证要求。
- 不修改代码，不把规划写成已实现事实。

### Risks

- 旧 PC Power 路径功能丰富，若直接照搬会把页面栈、通知、timer 和显示策略错误压进共享 Core。
- 若把 Power 状态等同于页面状态，会破坏 P1 已建立的页面权威源合同。
- 若在同一轮加入 Dim、通知、抬腕和真实低功耗，会失去可独立验收的小闭环。
- 若忽略平台执行失败，逻辑状态可能显示为熄屏而硬件仍亮屏，或反之。

### Doc Impact

`required`

### Suggested commit message

`docs: decide v0.5 shared power semantics`

### Execution record

- 2026-06-13：卡片已创建，尚未执行。
- 2026-06-13：用户已明确 F411 当前只使用表冠语义；底层 Wake 键是历史遗留，不得作为当前 Power 输入依据。
- 2026-06-13：用户已选择最小唤醒规则：熄屏时仅表冠按下唤醒，首个按下不继续确认；熄屏旋转忽略。

### Stop policy

- 本卡完成后停止，等待用户验收 Power 语义决策。
- 不自动创建或执行 `V0.5-P3-B`。
