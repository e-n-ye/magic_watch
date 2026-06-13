# V0.5 P3 Power / Wake / Screen On 共享语义规划

## 卡片 V0.5-P3-A Power / Wake / Screen On 共享语义决策

- ID：`V0.5-P3-A`
- 标题：Power / Wake / Screen On 共享语义决策
- 批次：V0.5 P3
- 状态：DONE
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

### Decision result

#### 1. 唯一最小状态模型

V0.5 首个 Power 合同只使用两个逻辑状态：

```text
SCREEN_ON
SCREEN_OFF
```

初始化状态固定为 `SCREEN_ON`，因为当前 PC/F411 初始化后都会直接创建 UI，F411 bring-up 还会立即打开背光。

以下概念本轮不加入 Power 状态：

- `ScreenDim`：需要亮度策略、timer 和平台亮度能力，当前没有共同闭环。
- `WakePending`：两阶段执行可以用 Action + commit 表达，不需要把执行中状态暴露为产品状态。
- `PoweredOff`：涉及启动和关机域，不是 Screen On/Off 最小闭环。
- AOD、长续航、通知唤醒、抬腕和触摸唤醒：都需要额外策略或平台输入证据。

#### 2. 状态、请求、动作和提交必须分开

最小共享语义固定为：

```text
PowerRequest -> PowerController decision -> PowerAction
-> Platform apply -> commit(success/failure) -> PowerState
```

其中：

- `PowerState` 是持久逻辑事实。
- `PowerRequest` 是领域请求，只表达“请求息屏”或“请求唤醒”，不携带 GPIO、PWM、LCD 命令。
- `PowerAction` 是瞬时执行命令，只表达 `TurnScreenOff`、`WakeScreen` 或 `None`。
- 请求阶段不得提前修改 `PowerState`。
- 平台执行成功后才能 commit 到目标状态。
- 平台执行失败、非法 commit 或陈旧 Action 均保持原状态。
- V0.5 只支持同步执行后立即 commit；异步 token、跨线程完成事件和超时恢复留到确有平台证据时再设计。

这与 `PageState` / `PageIntent` 的拆分原则一致，但两个状态机彼此独立：

- `WatchCorePageState` 回答“当前产品页面是什么”。
- Power 状态回答“屏幕逻辑上是否应处于可见工作态”。
- Power Action 不产生 `PageIntent`，也不修改页面状态、模型快照或现有 UI 事件队列。
- `ScreenOff` 不是页面，不保存页面栈；V0.5 唤醒后继续显示息屏前同一页面状态。

#### 3. 最小状态转移表

| 当前状态 | 请求 | 输出 Action | 请求后状态 | 平台成功 commit 后 | 平台失败后 |
|---|---|---|---|---|---|
| `SCREEN_ON` | `REQUEST_SCREEN_OFF` | `TURN_SCREEN_OFF` | `SCREEN_ON` | `SCREEN_OFF` | `SCREEN_ON` |
| `SCREEN_ON` | `REQUEST_WAKE` | `NONE` | `SCREEN_ON` | `SCREEN_ON` | `SCREEN_ON` |
| `SCREEN_OFF` | `REQUEST_WAKE` | `WAKE_SCREEN` | `SCREEN_OFF` | `SCREEN_ON` | `SCREEN_OFF` |
| `SCREEN_OFF` | `REQUEST_SCREEN_OFF` | `NONE` | `SCREEN_OFF` | `SCREEN_OFF` | `SCREEN_OFF` |

`NONE` 表示幂等 no-op，不需要平台执行，也不能被解释为失败。

#### 4. F411 表冠规则

| Power 状态 | 输入 | 共享语义 | 是否继续进入现有 LVGL/导航链 |
|---|---|---|---|
| `SCREEN_ON` | 表冠顺/逆时针旋转 | 保持现有切卡语义 | 是 |
| `SCREEN_ON` | 表冠短按 | 保持现有确认/进入详情语义 | 是 |
| `SCREEN_OFF` | 表冠顺/逆时针旋转 | 忽略 | 否 |
| `SCREEN_OFF` | 表冠短按 | `REQUEST_WAKE` | 否；首个按下只唤醒 |
| 任意状态 | 历史 Wake 键事件 | 不属于当前产品合同 | 否 |

表冠长按的产品语义当前没有闭环证据，不作为本轮 Power 唤醒输入。

#### 5. 所有权边界

| 层 | 应负责 | 不应负责 |
|---|---|---|
| `PowerController` | 持有逻辑 Power 状态；按状态表把请求变成 Action；成功后提交状态 | 不读表冠 GPIO，不操作 LVGL/LCD/PWM，不保存页面栈 |
| Coordinator / composition boundary | 根据当前 Power 状态路由输入；保证熄屏首个表冠按下只唤醒；安排 apply 后 commit | 不复制 Power 状态机，不伪造平台成功 |
| UI Adapter | 在平台接入阶段转交已形成的请求和同步最终状态；继续维护 View 边界 | 不把 `ScreenOff` 当 screen/page，不决定 Power 策略 |
| Platform Port | 执行背光、显示和未来 MCU 低功耗动作，并报告成功或失败 | 不决定何时息屏、什么输入能唤醒，不修改页面状态 |
| View | 保留当前页面对象和视觉状态 | 不成为 Power 或页面业务状态权威源 |

当前 F411 的 `watch_lcd_backlight_set(0U)` 已具备最小“背光不可见”执行能力，但没有错误返回合同；这是 V0.6 平台执行设计输入，不是 V0.5 立即修改理由。

#### 6. 被否决方案

1. **直接移植旧 PC `PowerController`**
   - 否决原因：它包含 `Booting`、`PoweredOff`、长续航、抬腕、触摸、display policy 和 timer 请求，超出新主线首个闭环。
2. **把 `ScreenOff` 建模成页面或 `ScreenId`**
   - 否决原因：会让显示电源状态与产品导航状态耦合，唤醒时还会引入不必要的页面栈恢复合同。
3. **请求时立即修改 Power 状态**
   - 否决原因：平台执行失败后 Core 与硬件事实会分裂。
4. **新增公开 `WakePending`**
   - 否决原因：当前同步 apply/commit 已足够；为尚不存在的异步平台提前增加产品状态。
5. **把 Power 请求塞进现有 `WatchCoreUiEvent` 队列**
   - 否决原因：现有队列服务 UI 导航语义；Power 还包含平台执行结果，直接扩进去会混淆 UI 事件、系统请求和硬件完成事实。
6. **在 V0.5 直接接 F411 背光和表冠拦截**
   - 否决原因：那属于平台执行闭环，会把纯 Core 合同和硬件回归混成一轮，并提前侵入 V0.6。

#### 7. 下一实现卡唯一建议

下一张卡应为：

```text
V0.5-P3-B 纯 C PowerController 决策合同与测试
```

目标：

- 在 `watch_core` 内新增独立、固定大小、无堆的 `WatchCorePowerController`。
- 使用静态状态转移表实现 `SCREEN_ON` / `SCREEN_OFF`、两个 Request 和三个 Action。
- 将 Controller 组合进 `WatchCore`，但不加入现有 `WatchCoreUiEvent` 队列。
- 提供只读状态、请求决策和平台结果 commit 的最小公共合同。
- 保持 `WatchCorePageState`、`UiModelSnapshot`、UI 事件队列和两个 Adapter 行为不变。

建议公共形状：

```text
WatchCorePowerState
WatchCorePowerRequest
WatchCorePowerActionType
WatchCorePowerAction
WatchCorePowerController

get state
request action
commit applied result
```

`P3-B` Allowed：

- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`
- `watch_core/tests/watch_core_contract_test.c`
- 对应路线图、审计、卡片和运行状态文档

`P3-B` Forbidden：

- 不修改 PC/F411 Adapter、F411 输入链、Lite View、LCD/背光驱动或 MDK 工程。
- 不新增 `ScreenOff` 页面、`SystemEvent`、timer、触摸/抬腕/通知唤醒或 LILYGO 实现。
- 不修改现有导航事件语义，不把 Power Action 变成 `PageIntent`。
- 不开始 V0.6 平台执行。

`P3-B` 纯 PC 合同测试至少覆盖：

1. 初始化为 `SCREEN_ON`。
2. `SCREEN_ON + REQUEST_SCREEN_OFF` 返回 `TURN_SCREEN_OFF`，请求阶段状态不变。
3. 平台失败 commit 后仍为 `SCREEN_ON`。
4. 平台成功 commit 后为 `SCREEN_OFF`。
5. `SCREEN_OFF + REQUEST_WAKE` 返回 `WAKE_SCREEN`，请求阶段状态不变。
6. 唤醒失败保持 `SCREEN_OFF`，成功后回到 `SCREEN_ON`。
7. 重复息屏、重复唤醒返回 `NONE`。
8. 非法或与当前状态不匹配的 commit 被拒绝且状态不变。
9. Power 往返前后页面状态和 snapshot 不变。
10. 现有全部 watch_core 合同测试继续通过。

F411 验证要求：

- `P3-B` 完成前必须至少执行一次 Keil/MDK 编译，补上 `P2-A` 以来尚未完成的 F411 编译验证。
- 因本卡不接 Adapter 或硬件 Power 执行，不要求反复真机 Power 测试。
- 建议只做一次四卡、详情、Back、表冠原闭环冒烟，确认 `WatchCore` 结构扩展没有引入现有行为回归。

#### 8. V0.5 后续走向与退出门

`P3-A` 之后，V0.5 不再继续横向发散功能。后续整体规划应围绕三个闭环：

1. **实现 Power 决策合同**
   - 即 `P3-B`，只完成纯 C 状态机与合同测试。
2. **做 V0.5 Core 组合与边界复核**
   - 核对 Input / UI Event / PageState / PageIntent / PowerState / PowerAction / Coordinator 各自所有权。
   - 若没有新的代码证据表明合同断裂，不再为了形式新增 `SystemEvent`、总线或任务。
3. **形成 V0.5 学习与退出总结**
   - 用状态表、事件链和失败路径回答 V0 完成定义中的 Core 问题。
   - 把 PC/F411/LILYGO 平台执行卡留给 V0.6。

V0.5 可以结束的条件：

- 能从公开合同解释页面状态和 Power 状态为何是两个独立状态机。
- 能解释 Request、Action、平台执行和状态 commit 的先后关系。
- 能证明平台失败不会让 Core 提前进入错误 Power 状态。
- 能证明 Power 往返不改变页面权威状态。
- 能明确表冠原始输入、共享 Power 决策和背光执行分别属于哪一层。
- 纯 PC 合同测试通过，F411 至少编译通过且原 UI 行为冒烟无回归。

V0.6 从以下工作开始：

- PC/F411/LILYGO 各自实现 Power Action executor。
- F411 在 LVGL 喂入前按共享 Power 状态拦截表冠输入。
- F411 实际执行背光开关并验证首个按下只唤醒。
- PC 提供等价模拟路径。
- LILYGO 接入自身显示、电源和未来抬腕唤醒源。
- 再根据真实平台证据决定是否需要异步完成事件、`ScreenDim`、抬腕或触摸唤醒。

#### 9. V0.5 学习验收

本阶段的重点不是记住几个类型名，而是能够独立回答：

- 状态、请求、动作和硬件事实有什么区别？
- 为什么 ScreenManager/PageState 与 PowerController/PowerState 必须分开？
- 为什么 Controller 可以只是纯函数式事件处理模块，而不是一个 FreeRTOS 任务？
- 为什么平台只执行 Action，不能反向拥有产品策略？
- 为什么失败路径必须在写代码前进入状态转移表？
- 为什么旧 PC 代码可以提供行为证据，却不能原样成为新 MCU 架构？
- 为什么当前不需要 `ScreenDim`、`SystemEvent`、异步 token 或完整低功耗模式？

能结合 Magic Watch 当前代码回答这些问题，才算完成 V0.5 的学习目标。

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
- 2026-06-13：开始执行，只做共享语义决策、V0.5 学习目标与后续实现边界，不修改代码。
- 2026-06-13：已完成只读审计并形成唯一方案：双状态、Request/Action/commit 两阶段合同、Power 与页面状态解耦；下一实现卡固定为纯 C `PowerController` 合同与测试。
- 2026-06-13：自检通过；本卡只修改允许的规划、审计和运行状态文档，没有修改代码或创建 `P3-B` 卡片。

### Stop policy

- 本卡完成后停止，等待用户验收 Power 语义决策。
- 不自动创建或执行 `V0.5-P3-B`。
