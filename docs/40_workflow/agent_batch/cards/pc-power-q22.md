# PC-POWER-Q22 PC Power executor 最小闭环

本文件只登记阶段 1 中 PC Power executor 的当前执行拆分。
不得从本文件扩展能力路线阶段 2-4 卡片。

三张卡可以在规划轮一起落库，但执行时必须每张卡后停止验收。
`PC-POWER-01` 是下一张执行入口；`PC-POWER-02/03` 只能在依赖满足后继续。

## PC-POWER-01 PC Debug Power executor 最小可视闭环

- 批次：`PC-POWER-Q22`
- 状态：DONE
- 依赖：`MAINLINE-DATA-01` DONE
- 自检：
  - `git status --short -uall`
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
  - 人工运行 `magic_watch_xml_sim`，验证 `C` 显示 overlay、`R` 隐藏 overlay
  - 人工检查 overlay 显示后点击健康卡是否点穿进入详情
  - 人工回归动态电池、四卡、详情与 Back
  - `git diff --check`
  - 本轮中文文档乱码哨兵检查
- 建议提交信息：`pc: add minimal power executor overlay`
- Doc Impact：`required`

### 问题定位

`watch_core` 已经拥有最小 Power request/action/commit 合同，但 PC XML 主线还没有
平台 executor。当前缺口不是继续新增 Core 名词，而是证明 PC 运行时能执行 Core
给出的 Power action，并产生可观察 screen off / wake 效果。

本卡只证明 executor 最小闭环，不宣称 screen-off 输入门控完成。

### 实施方案

- 复用 `SimulatorDevice` 现有 debug action：
  - `C / SimCoverSleep` 转成 `WATCH_CORE_POWER_REQUEST_SCREEN_OFF`
  - `R / SimRaiseToWake` 转成 `WATCH_CORE_POWER_REQUEST_WAKE`
  - `F / SimRaiseDismiss` 本轮忽略并记录 no-op
- 在 `xml_sim_main.cpp` 使用 `pc_power_request_pending` 和 pending request，避免使用
  `power_dirty`，以免和 DATA-01 的 UI dirty 混淆。
- Device callback 只登记 pending request，不执行 overlay、不 commit、不调 Adapter/View。
- Power request 只有一个主循环消费点：`device->tick()` 返回后处理 pending request。
- 主循环消费点调用 `watch_core_request_power_action()`。
- 如果 `action.type == WATCH_CORE_POWER_ACTION_NONE`，只记录 no-op，不执行 overlay，不
  调用 `watch_core_commit_power_action()`。
- 增加 PC-only overlay helper，只做平台执行：
  - `WATCH_CORE_POWER_ACTION_TURN_SCREEN_OFF`：显示黑色 screen off overlay。
  - `WATCH_CORE_POWER_ACTION_WAKE_SCREEN`：隐藏 overlay。
- overlay helper 执行成功后才调用 `watch_core_commit_power_action(..., true)`。
- 本卡不模拟 failure；Core 合同测试已经覆盖失败保持状态，平台失败注入留给后续。
- 手工验收必须检查 overlay 是否点穿。如果点穿存在，本卡仍可通过 executor 闭环，
  但执行记录必须写明“screen-off 输入门控未完成，转入 PC-POWER-02”。

### 涉及位置

Allowed files

- `sim/lv_port_pc_vscode/src/xml_sim_main.cpp`
- `sim/lv_port_pc_vscode/README.md`
- `docs/40_workflow/agent_batch/cards/pc-power-q22.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/00_current/magicwatch_capability_growth_roadmap.md`

Read-only files

- `watch_core/**`
- `sim/lv_port_pc_vscode/src/XmlUi/**`
- `sim/lv_port_pc_vscode/src/Platform/Simulator/**`
- `ui/lvgl_pro/**`
- `try/my_watch_f411_v2.1/**`

Forbidden changes

- 不修改 `watch_core` API。
- 不修改 PC Adapter、XML 或 generated C。
- 不修改 F411 / LILYGO。
- 不做 DIM / AOD / 真实低功耗 / 背光驱动。
- 不新增 EventBus、Queue、Service、Scheduler。
- 不扩页面，不改变 DATA-01 电池链路。
- 不做自动 timeout。
- 不把 overlay 点穿检查写成输入门控已完成，除非实际验证无点穿。

### 风险

- 把 executor 写进 Adapter 或 View，模糊平台执行边界。
- callback 里直接执行 LVGL 或 commit，形成第二个消费点。
- `action == NONE` 时仍执行 overlay 或 commit，污染 Core 状态。
- overlay 可见但点穿仍存在，却误写成 screen-off 输入门控完成。

### 验收标准

- `C` 能触发 screen off request，并显示 overlay。
- `R` 能触发 wake request，并隐藏 overlay。
- 日志能看出 request、action、overlay 执行和 commit 结果。
- `action == NONE` 只记录 no-op，不执行 overlay，不 commit。
- Power request 只有主循环一个消费点，callback/Adapter/View 不直接执行。
- 动态电池、四卡、详情和 Back 不退化。
- 必须记录 overlay 显示后点击健康卡是否点穿：
  - 若不点穿，只能说明 `PC-POWER-01` 当前人工检查未发现点穿。
  - 若点穿，本卡仍可因 executor 闭环通过，但必须把输入门控问题留给
    `PC-POWER-02`，不得宣称 screen-off 输入门控完成。

### 执行记录

- 2026-06-16：已完成 `PC-POWER-01` 的最小 PC power request -> action ->
  overlay -> commit 闭环，并完成人工 UI 验收。
- 当前改动：`sim/lv_port_pc_vscode/src/xml_sim_main.cpp`、
  `sim/lv_port_pc_vscode/README.md`、
  `docs/40_workflow/agent_batch/cards/pc-power-q22.md`、
  `docs/40_workflow/agent_batch/agent-progress.md`
- 当前自检：
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe` 通过
  - 自动化 `C/R/F` 键烟测已看到：
    - `SimCoverSleep -> SCREEN_OFF -> TURN_SCREEN_OFF -> overlay_applied=true -> commit=true`
    - `SimRaiseToWake -> WAKE -> WAKE_SCREEN -> overlay_applied=true -> commit=true`
    - `SimRaiseDismiss -> mapped_request=NONE`
  - `git diff --check` 通过（仅 LF/CRLF 提示，无 diff 错误）
  - 本轮实际改动中文文档乱码哨兵检查无命中
- 人工验收：
  - `C` 时确认出现黑色 overlay
  - `R` 时确认 overlay 消失
  - overlay 显示后点击首页健康卡，确认不会点穿进入详情
  - 动态电池、四卡、详情与 `Back` 确认均未退化
- 风险回应：当前只在 `xml_sim_main.cpp` 增加本地 overlay helper、pending power
  request 和单一消费点日志；未修改 `watch_core` API、Adapter、XML 或 generated C，
  未提前修改 `PC-POWER-02/03` 的实现范围；当前人工检查未发现点穿，因此
  `PC-POWER-02` 应只做最小复核与记录，不必为挡点击新增抽象。

## PC-POWER-02 PC screen-off 输入门控修正

- 批次：`PC-POWER-Q22`
- 状态：DONE
- 依赖：`PC-POWER-01` DONE，且 `PC-POWER-01` 已记录 overlay 点穿检查结果
- 自检：
  - `git status --short -uall`
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
  - 人工运行 `magic_watch_xml_sim`，验证 overlay 显示后点击健康卡不得进入详情
  - 人工验证唤醒后页面状态保持，动态电池、四卡、详情与 Back 不退化
  - `git diff --check`
  - 本轮中文文档乱码哨兵检查
- 建议提交信息：`pc: block input through screen off overlay`
- Doc Impact：`required`

### 问题定位

`PC-POWER-01` 的成功标准是 executor 闭环成立。screen off 时普通 UI 输入是否仍会
点穿到底层页面，是独立输入门控问题。点穿未解决前，不得宣称 screen-off 输入门控
完成。

### 实施方案

- 以 `PC-POWER-01` 的点穿检查结果为前提，只修 PC overlay 阻挡或最小输入门控。
- 优先在 PC overlay 层解决点击阻挡，保持 `watch_core`、Adapter、XML/generated 不变。
- 如果 overlay 已经不点穿，本卡只做最小复核与记录，不引入额外抽象。
- screen off 期间普通点击不得产生健康卡详情跳转或其它页面导航。
- 唤醒后应回到息屏前的 Core 页面状态，不新增页面恢复机制。

### 涉及位置

Allowed files

- `sim/lv_port_pc_vscode/src/xml_sim_main.cpp`
- `sim/lv_port_pc_vscode/README.md`
- `docs/40_workflow/agent_batch/cards/pc-power-q22.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/00_current/magicwatch_capability_growth_roadmap.md`

Read-only files

- `watch_core/**`
- `sim/lv_port_pc_vscode/src/XmlUi/**`
- `sim/lv_port_pc_vscode/src/Platform/Simulator/**`
- `ui/lvgl_pro/**`
- `try/my_watch_f411_v2.1/**`

Forbidden changes

- 不修改 `watch_core` API。
- 不修改 PC Adapter、XML 或 generated C。
- 不修改 F411 / LILYGO。
- 不做自动 timeout。
- 不新增 EventBus、Queue、Service、Scheduler。
- 不新增页面或页面恢复系统。
- 不改变 DATA-01 电池链路。

### 风险

- 为了挡点击引入通用 Input Manager 或 EventBus。
- 把 screen-off 输入门控写进 Core UI 队列，污染现有导航合同。
- 未验证点穿却把输入门控写成完成。

### 验收标准

- overlay 显示后点击健康卡不得进入详情。
- overlay 显示后普通 UI 点击不得触发页面导航。
- `R` 唤醒后页面状态保持，不因挡点击丢失原页面。
- 动态电池、四卡、详情和 Back 不退化。
- 执行记录明确说明 `PC-POWER-01` 的点穿问题是否已解决。

### 执行记录

- 2026-06-16：已按最小复核路径完成 `PC-POWER-02`，未新增产品代码改动。
- 当前改动：`sim/lv_port_pc_vscode/README.md`、
  `docs/40_workflow/agent_batch/cards/pc-power-q22.md`、
  `docs/40_workflow/agent_batch/agent-progress.md`、
  `docs/00_current/magicwatch_capability_growth_roadmap.md`
- 当前自检：
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe` 通过
  - `git diff --check` 通过（仅 LF/CRLF 提示，无 diff 错误）
  - 本轮实际改动中文文档乱码哨兵检查无命中
- 人工复核：
  - overlay 显示后点击健康卡，不会进入详情
  - overlay 显示后普通 UI 点击未触发页面导航
  - `R` 唤醒后页面状态保持
  - 动态电池、四卡、详情与 `Back` 未退化
- 风险回应：由于 `PC-POWER-01` 已确认 overlay 不点穿，本卡不再为挡点击引入
  额外输入抽象，也不修改 `watch_core`、Adapter、XML 或 generated C；后续只保留
  `PC-POWER-03` 的 timeout 探针。

## PC-POWER-03 PC 自动 timeout 探针

- 批次：`PC-POWER-Q22`
- 状态：TODO
- 依赖：`PC-POWER-02` DONE，或用户明确接受暂缓输入门控后再规划执行
- 自检：
  - `git status --short -uall`
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
  - 人工运行 `magic_watch_xml_sim`，验证无输入超时后请求 screen off
  - 人工验证输入或调试动作可唤醒，动态电池、四卡、详情与 Back 不退化
  - `git diff --check`
  - 本轮中文文档乱码哨兵检查
- 建议提交信息：`pc: add minimal power timeout probe`
- Doc Impact：`required`

### 问题定位

自动 timeout 涉及输入活动统计、计时、重置策略和唤醒行为，不应混入
`PC-POWER-01` 的 executor 最小闭环。本卡只在 executor 和输入门控边界清楚后，
做 PC-only 最小 timeout 探针。

### 实施方案

- 只在 PC main 局部保存 inactivity 计时状态。
- 无输入达到固定探针时长后，登记 `WATCH_CORE_POWER_REQUEST_SCREEN_OFF`。
- 有输入或调试 wake 动作时，登记 `WATCH_CORE_POWER_REQUEST_WAKE` 或重置计时。
- 继续复用 `PC-POWER-01` 的唯一 Power request 主循环消费点。
- timeout 是 PC-only 探针，不进入 Core API，不进入 Adapter，不新增设置页。

### 涉及位置

Allowed files

- `sim/lv_port_pc_vscode/src/xml_sim_main.cpp`
- `sim/lv_port_pc_vscode/README.md`
- `docs/40_workflow/agent_batch/cards/pc-power-q22.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/00_current/magicwatch_capability_growth_roadmap.md`

Read-only files

- `watch_core/**`
- `sim/lv_port_pc_vscode/src/XmlUi/**`
- `sim/lv_port_pc_vscode/src/Platform/Simulator/**`
- `ui/lvgl_pro/**`
- `try/my_watch_f411_v2.1/**`

Forbidden changes

- 不修改 `watch_core` API。
- 不修改 PC Adapter、XML 或 generated C。
- 不修改 F411 / LILYGO。
- 不做设置页、快捷设置、DIM、AOD、真实低功耗或背光驱动。
- 不新增 EventBus、Queue、Service、Scheduler。
- 不改变 DATA-01 电池链路。

### 风险

- 把 timeout policy 提前产品化，扩成设置页或快捷设置。
- 为计时引入不必要的调度抽象。
- 输入门控未完成时继续扩大 Power 行为，导致验收边界不清。

### 验收标准

- 无输入达到固定探针时长后可复现 screen off request。
- 输入或调试动作可唤醒。
- timeout 与手动 `C/R` 调试动作不冲突。
- 动态电池、四卡、详情和 Back 不退化。
- 未执行的 F411 / LILYGO / 真实低功耗检查如实记录为未验证。

### 执行记录

- 尚未执行。
