# V0.5-P0-A 共享合同测试与 F411 所有权审计

日期：2026-06-12  
状态：当前事实基线

## 1. 合同测试结果

本轮新增了纯 PC、无 LVGL、无 SDL、无 HAL 的 `watch_core` 合同测试入口：

- 目标：`magic_watch_core_contract_test`
- 源文件：`watch_core/tests/watch_core_contract_test.c`
- 构建入口：`sim/lv_port_pc_vscode/CMakeLists.txt`

覆盖结果：

1. 通过公开行为证明默认页面语义为健康四卡：通过
2. 四种健康卡事件分别产生对应详情 `PageIntent`：通过
3. 详情页 `Back` 返回健康四卡：通过
4. 健康四卡页 `Back` 为 no-op：通过
5. 非法 feature 不产生页面跳转：通过
6. 空队列处理返回 no-op：通过
7. EventQueue 保持 FIFO：通过
8. 队列满时拒绝新事件，已有事件顺序不变：通过
9. Snapshot 更新可以读取：通过
10. 超长 metric 文本被安全截断并保证 `NUL` 结尾：通过

边界扫描结论：

- `watch_core` 未发现 `lvgl`
- `watch_core` 未发现 `SDL`
- `watch_core` 未发现 `HAL` 或 `CubeMX`
- `watch_core` 未发现 `malloc/free/new/delete/std::`

说明：

- 本轮测试验证的是行为语义，不把内部字段布局、枚举底层大小或结构体 ABI 偶然值固化成合同。
- 默认页面语义现在通过“初始化后 `Back` 为 no-op -> 合法卡片点击进入详情 -> 详情 `Back` 返回健康四卡”这组三段公开行为间接证明，而不是直接读取 `WatchCore` 内部页面字段。
- `watch_core` 当前已经事实性拥有：固定容量事件队列、页面状态、快照和最小 Coordinator 行为；V0.5 后续不应再把它们当成“尚未开始实现”的空白模块。

## 2. 当前稳定语义 vs. 实现细节

稳定语义：

- 健康四卡是初始化后的默认页面语义。
- `UiEvent -> EventQueue -> Coordinator -> PageIntent` 是当前共享链路。
- `Back` 只在详情页生效，首页 `Back` 为 no-op。
- 非法 feature 不得触发页面跳转。
- Snapshot 必须可读、可更新、可安全截断。
- 事件队列必须 FIFO，满队列拒绝新事件但不得打乱旧顺序。
- 当前页面状态应通过公开页面状态合同读取，而不是通过 `PageIntent` 动作或内部字段猜测。
- 单次外部输入后的共享处理规则应是“drain 到稳定态”，而不是由各平台各自定义处理到第几个事件为止。

当前实现细节，不应直接固化成长期合同：

- `WatchCore` 结构体内部字段的排列方式。
- `queue_head / queue_tail / queue_count` 的具体实现字段名。
- 目前 `current_page` 存在于 `WatchCore` 本体并被 F411 bridge 镜像。
- F411 当前用“两块 panel hidden/unhidden”表达页面切换，而不是独立 LVGL screen create/load。
- 表冠输入先进入 LVGL encoder indev，再由 focused clickable 控件回调间接生成 typed `UiEvent`；这是一条当前实现路径，不是长期唯一语义合同。

`P1-B` 新增的公共护栏：

- `WatchCorePageState`：用于表达权威当前页面状态。
- `watch_core_get_current_page_state()`：用于公开读取默认页和导航后状态。
- `watch_core_process_pending_events()`：用于固化“单次输入后 drain 到稳定态”的共享消费规则。

## 3. F411 新旧主链所有权审计

### 3.1 `P0-B` 前运行主链

当前主链是：

```text
编码器硬件
-> watch_input_service_scan_10ms / watch_input_service_get_event
-> watch_input_intent_from_event
-> watch_lvgl_port_feed_input_intent
-> LVGL encoder/pointer indev
-> watch_lvgl_debug_screen 点击/Back 回调
-> watch_core_make_* typed UiEvent
-> watch_core_bridge_dispatch_event
-> watch_core_push_event / watch_core_process_next_event
-> bridge 镜像 current_page + snapshot
-> watch_lvgl_debug_screen apply_snapshot_to_view / apply_page_to_view
```

### 3.2 `P0-B` 迁移表

| 当前职责 | `P0-B` 前位置 | `P0-B` 后归属 |
|---|---|---|
| Core 生命周期 | `watch_core_bridge.c` | `f411_ui_adapter.c` |
| typed `UiEvent` 创建和派发 | `watch_lvgl_debug_screen.c`、`watch_core_bridge.c` | `f411_ui_adapter.c` |
| Snapshot 获取 | `watch_core_bridge.c` | `f411_ui_adapter.c` |
| Snapshot 写入 LVGL | `watch_lvgl_debug_screen.c` | `watch_lite_view.c` |
| `PageIntent` 消费和调度 | `watch_core_bridge.c`、`watch_lvgl_debug_screen.c` | `f411_ui_adapter.c` |
| 页面视觉切换 | `watch_lvgl_debug_screen.c` | `watch_lite_view.c` |
| LVGL 对象创建 | `watch_lvgl_debug_screen.c` | `watch_lite_view.c` |
| 表冠/触摸硬件与 indev | `watch_lvgl_port.c` | `watch_lvgl_port.c`（保持不变） |
| tap-only、swipe 判定、手感阈值 | `watch_lvgl_port.c` | `watch_lvgl_port.c`（保持不变） |

### 3.3 `P0-B` 后当前运行主链

当前主链收口为：

```text
编码器硬件 / 触摸硬件
-> watch_input_service_scan_10ms / watch_input_service_get_event
-> watch_input_intent_from_event
-> watch_lvgl_port_feed_input_intent / LVGL pointer indev
-> watch_lite_view LVGL 点击回调 / swipe commit 可读信号
-> f411_ui_adapter
-> watch_core_push_event / watch_core_process_next_event
-> f411_ui_adapter
-> watch_lite_view_apply_page_intent / watch_lite_view_apply_snapshot
```

### 3.4 `P0-B` 后逐项所有权结论

原始表冠输入由谁读取：

- `watch_input_service_scan_10ms()` 负责扫描，`watch_input_service_get_event()` 负责出队。
- `freertos.c` 周期调用 `watch_bringup_scan_input_10ms()`，`watch_bringup_task()` 再消费事件。

`watch_input_intent` 当前承担什么：

- 只做 `watch_input_event_t -> watch_input_intent_t` 的薄转换。
- 当前不直接生成 typed `UiEvent`，也不直接决定页面导航。

typed `UiEvent` 在哪里生成：

- `P0-B` 前由 `watch_lvgl_debug_screen.c` 在卡片点击、屏上 `Back` 点击和左边缘右滑返回提交处生成。
- `P0-B` 后集中到 `f411_ui_adapter.c`：
  - `watch_core_make_health_card_clicked_event(...)`
  - `watch_core_make_back_event()`

EventQueue 的权威实现是哪一套：

- 当前运行主链实际使用的是 `watch_core` 内部固定容量队列。
- `P0-B` 后证据：`f411_ui_adapter_dispatch_event()` 调用 `watch_core_push_event()` 和 `watch_core_process_next_event()`。
- `P0-B` 时旧 `user/core/event/watch_event_queue.*` 仍被编译，但没有进入当前运行主链。

当前页面状态分别存在哪些位置：

- 权威页面语义状态：`WatchCore.current_page`
- `P0-B` 后 F411 当前装配状态：`f411_ui_adapter_state_t.current_page`
- F411 当前视图展开状态：`watch_lite_view.c` 里 `s_shortcuts_panel / s_detail_panel` 的 hidden/visible

Coordinator 决策在哪里发生：

- 当前仍在 `watch_core_process_next_event()` 内完成。
- F411 没有第二套独立 Coordinator。

Snapshot 在哪里应用到 View：

- `f411_ui_adapter_sync_snapshot()` 从 `watch_core` 拉取 snapshot。
- `watch_lite_view.c` 的 `watch_lite_view_apply_snapshot()` 把 snapshot 写到 LVGL label。

`PageIntent` 在哪里转换成 LVGL 页面变化：

- `f411_ui_adapter_dispatch_event()` 保存 `last_intent` 和 `current_page`。
- `watch_lite_view.c` 的 `watch_lite_view_apply_page_intent()` 再把 `current_page` 转成 header 文案、detail/shortcut 面板切换和 focus group 重建。

旧 `watch_event_queue` 和 `watch_screen_manager` 是否仍被编译：

- `P0-B` 时是。`my_watch_f411.uvprojx` 当时仍登记了两者。
- `P0-C` 后否。当前 MDK target 已移除这两组旧文件登记，但源码文件仍保留在仓库里。

它们是否仍进入当前运行主链：

- 否。仓库内搜索未发现 `watch_event_queue_*` 或 `watch_screen_manager_*` 在 bring-up、task、LVGL port 或当前 Lite UI 路径中被调用。
- 这两套当前属于“被编译，但未初始化、未调用、未掌权”。

`P0-B` 前是否已经存在事实上的 `F411UiAdapter` 职责组合：

- `P0-A` 时已经存在，但分散在三个文件：
  - `watch_core_bridge.c`
  - `watch_lvgl_debug_screen.c`
  - `watch_lvgl_port.c`
- `P0-B` 后已经收口为明确边界：
  - `f411_ui_adapter.c`：core 生命周期、typed `UiEvent` 派发、snapshot 获取、`PageIntent` 消费、view 同步
  - `watch_lite_view.c`：View 构建、snapshot 应用、页面视觉切换、LVGL 点击转 Adapter 调用
  - `watch_lvgl_port.c`：encoder/pointer 输入语义、tap-only、防误触、左边缘右滑状态

这些职责在 `P0-B` 前是否已出现会阻碍下一功能的重复或泄漏：

- 是。
- 当前“typed event 入口、snapshot 应用、page intent 执行、视图生命周期”没有单一装配点。
- 旧 `watch_screen_manager` 虽然仍在工程里，但它并未拥有当前运行链路；继续先隔离它，只会降低工程噪音，不会解决当前真正的扩展障碍。

## 4. `P0-B` 后编译 / 初始化 / 调用 / 运行状态区分

| 模块 | 被编译 | 被初始化 | 被调用 | 当前运行主链实际使用 |
|---|---|---|---|---|
| `watch_core` | 是 | 是 | 是 | 是 |
| `f411_ui_adapter` | 是 | 是 | 是 | 是 |
| `watch_lite_view` | 是 | 是 | 是 | 是 |
| `watch_lvgl_port` | 是 | 是 | 是 | 是 |
| `watch_input_service` | 是 | 是 | 是 | 是 |
| `watch_input_intent` | 是 | 否（纯函数） | 是 | 是 |
| `watch_core_bridge` | 否，已从当前 MDK target 移出 | 否 | 否 | 否 |
| `watch_lvgl_debug_screen` | 否，已从当前 MDK target 移出 | 否 | 否 | 否 |
| 旧 `watch_event_queue` | 否，已从当前 MDK target 移出 | 否 | 否 | 否 |
| 旧 `watch_screen_manager` | 否，已从当前 MDK target 移出 | 否 | 否 | 否 |

补充说明：

- `f411_ui_adapter`：被编译、初始化、调用，且当前主链使用。
- `watch_lite_view`：被编译、初始化、调用，且当前主链使用。
- `watch_core_bridge`：已从当前 MDK 目标和活跃路径移出。
- `watch_lvgl_debug_screen`：已从当前 MDK 目标和活跃路径移出。
- 旧 `watch_event_queue`：源码仍保留，但当前 MDK target 不再编译；未初始化、未调用、不在主链。
- 旧 `watch_screen_manager`：源码仍保留，但当前 MDK target 不再编译；未初始化、未调用、不在主链。

## 5. `P0-B` 收口结论

`P0-B` 已把 `typed UiEvent` 派发、Snapshot 获取、`PageIntent` 消费和视图同步收口到单点装配边界。当前 F411 活跃链路已经不再依赖 `watch_core_bridge` 或 `watch_lvgl_debug_screen`。

`P0-B` 已落实的边界：

- 已形成单点 `F411UiAdapter` 装配边界。
- `watch_lvgl_port` 继续保留硬件读取、indev 注册、原始坐标、tap-only、防误触、左边缘右滑和 `WATCH_TOUCH_SWIPE_BACK_COMMIT_DISTANCE = 36U` 这类手感阈值。
- Lite View 继续只持有 LVGL 对象和具体视觉更新，不直接决定产品导航。

`P0-B` 已回填的真机验收结果：

1. Keil / MDK 编译通过。
2. 四卡首页正常显示。
3. 表冠旋转切卡正常。
4. 表冠短按进入详情正常。
5. 触摸点击卡片进入详情正常。
6. 触摸点击屏上 Back 返回正常。
7. tap-only 防误触正常。
8. 左边缘右滑 Back 正常。
9. 中间起手右滑不会误返回。
10. 首页左边缘右滑不会异常跳转。
11. 无花屏、卡死、停更。
12. `WATCH_TOUCH_SWIPE_BACK_COMMIT_DISTANCE` 仍为 `36U`。

## 6. 对 V0.5 后续拆分的修正

原先 `V0.5-A ~ G` 把 `EventQueue`、`Coordinator`、`Snapshot` 等内容视为待从零实现，与当前代码事实不符。

本轮后建议按以下方向重排：

1. `V0.5-P0-A`：共享合同测试与 F411 所有权审计
2. `V0.5-P0-B`：抽取真正的 `F411UiAdapter`
3. `V0.5-P0-C`：从当前 MDK target 中隔离旧 `watch_screen_manager` / `watch_event_queue` 编译登记，同时保留源码作为历史预研资产
4. `V0.5-P1-*`：先澄清“页面状态 / `PageIntent` / Adapter 消费规则”合同；若需要新增公开路由类型，也应是能表达页面种类与详情 feature 的页面状态结构，而不是先机械补一个平面 `ScreenId`

这表示 V0.5 的核心已经从“空白造 core 模块”转成“给现有共享合同装护栏，并把 F411 当前所有权收口到可扩展边界”。

## 7. `V0.5-P2` 追加结论：第三 Adapter 接入合同与退出门

### 7.1 `P2-A` 修正后的共享事件完成语义

`watch_core_process_pending_events()` 现在以“队列已空”为终止条件，而不是以“最近一个事件是否产生 `PageIntent`”为终止条件。

这意味着以下三件事已被正式拆开：

- 事件是否已被消费
- 事件是否产生页面动作
- 队列是否已经空

纯 PC 合同测试已补齐并通过：

- `首页 Back -> 合法卡片点击`
- `非法 feature -> 合法卡片点击`
- `卡片点击 -> Back`
- drain 后队列为空

因此，`P1` 里“单次外部输入后 drain 到稳定态”的共享合同现在才算真正实现成立。

### 7.2 第三 Adapter 最小接入检查表

1. Adapter 只接收已经形成的 typed `UiEvent`，不把原始硬件事件直接塞给 `watch_core`。
2. Adapter 调用 `watch_core_push_event()` 失败时：
   - 不得继续调用 drain。
   - 不得伪造 `PageIntent`。
   - 不得自行改写本地页面缓存或 View。
   - 应记录日志或返回失败，让平台调度层决定是否重试或丢弃。
3. Adapter 在单次外部输入后，必须调用 `watch_core_process_pending_events()` 直到队列空；不能因为最近一个事件返回 `NONE` 就提前停下。
4. drain 完成后，Adapter 只能基于最终 `WatchCorePageState` 和最终 `UiModelSnapshot` 同步 View，不得在中途事件上做导航裁决。
5. 本地页面缓存只允许作为“最后一次已应用结果”的镜像，不是权威源；权威源始终是 `watch_core_get_current_page_state()`。
6. 平台 View 可以保留自己的对象、focus、panel hidden 状态和手势动画状态，但这些状态不得反向决定产品导航。

### 7.3 代码证据

PC Adapter：

- `dispatch_event()` 中，若 `watch_core_push_event()` 失败，直接 `LV_LOG_WARN(...)` 后 `return`，不会继续 drain，也不会伪造页面切换。
- 成功入队后，调用 `watch_core_process_pending_events()`，随后在 `sync_core_to_view()` 中读取当前 `PageState` 并同步 View。

F411 Adapter：

- `f411_ui_adapter_dispatch_event()` 中，若 `watch_core_push_event()` 失败，直接返回 `false`，不会继续同步页面缓存、snapshot 或 View。
- 成功入队后，调用 `watch_core_process_pending_events()`，再通过 `watch_core_get_current_page_state()` 和 `watch_core_get_ui_snapshot()` 拉取最终状态，然后统一同步 Lite View。

共享结论：

- 两个平台现在都遵守“push 成功后再 drain，到空为止；最后按最终状态同步 View”的主合同。
- PC 与 F411 的差异仅剩 View 技术栈、输入算法和具体视觉更新，不再体现在 Core 事件消费语义上。

### 7.4 View 创建 / 应用失败的最小策略

当前代码尚未把“View 创建或应用失败后的平台返回路径”抽成统一 API，但基于现有实现可以收敛出最小策略：

1. View 失败不能回写 `watch_core` 状态，也不能额外生成补偿导航事件。
2. 平台可以保留上一次可见页面，或仅记录日志后保持当前显示；但不得因为 View 失败而把本地缓存提升为权威源。
3. 这属于平台健壮性问题，不是当前共享合同的继续缺口。只有在后续实际引入复杂页面或异步资源加载时，才需要单独提升为新的实现卡。

### 7.5 `V0.5` 退出判断

结论：`V0.5` 共享合同主线现在可以退出到下一阶段规划，下一轮应进入 Power / Wake / Screen On 语义规划，而不是继续补一个新的共享合同实现卡。

理由：

- 已有纯 PC 合同测试护栏。
- `PageState` / `PageIntent` / 默认页归属 / Adapter drain 语义已一致。
- PC / F411 当前都按同一消费合同运行。
- 本轮没有发现新的、已被代码证实的共享合同断裂点。

保留项：

- F411 Lite View 仍以页面动作类型承载自己的局部视图切换入口，这属于平台内实现细节，不构成共享权威状态泄漏。
- View 创建失败后的平台恢复路径仍偏轻量，但当前没有证据表明它已阻碍 Power 语义规划。

## 8. `V0.5-P3-A` Power 共享语义决策

### 8.1 当前代码证据

- 新主线 `WatchCore` 当前只持有模型、页面状态和 UI 事件队列，没有 Power 状态。
- PC/F411 Adapter 当前只同步最终 `PageState` 和 snapshot，没有平台 Power executor。
- F411 `watch_input_intent_from_event()` 只把表冠旋转、短按和长按转换成上层 Intent；底层 Wake 键事件没有进入当前上层主链。
- F411 当前把表冠 Intent 直接喂给 LVGL encoder；未来接 Power 后，熄屏输入必须在该喂入动作之前被共享状态门控。
- F411 已有 `watch_lcd_backlight_set(percent)`，可把占空比设为 0，但当前接口没有执行成功/失败返回合同。
- 旧 PC `PowerController` 已证明“Controller 返回固定大小 Action、不直接操作 UI”的边界有价值；旧 `AppStateMachine` 也只在页面/显示执行成功后标记 Running 或 ScreenOff。

### 8.2 决策

V0.5 首个共享 Power 合同只定义 `SCREEN_ON` / `SCREEN_OFF`。

```text
PowerRequest
-> PowerController 只做状态表决策
-> PowerAction
-> Platform 执行
-> commit(success/failure)
-> PowerState
```

请求阶段不修改持久状态。平台失败时保持原状态。Power 状态与 `WatchCorePageState` 独立，息屏/唤醒不产生 `PageIntent`，也不清空或恢复页面栈。

F411 当前输入规则固定为：

- 亮屏时表冠旋转和短按继续走现有 UI 语义。
- 熄屏时表冠旋转忽略。
- 熄屏时表冠短按只请求唤醒，首个按下不继续确认。
- 历史 Wake 键不进入当前产品合同。

### 8.3 下一实现边界

下一卡只在 `watch_core` 内实现独立、固定大小、无堆、表驱动的 `WatchCorePowerController` 和纯 PC 合同测试。

本轮及下一卡都不接 PC/F411 Adapter，不控制真实背光，不新增 `ScreenOff` 页面、`SystemEvent`、timer、抬腕/触摸/通知唤醒或 LILYGO 代码。

V0.5 负责把 Power 决策合同做成可解释、可测试的 Core 事实；V0.6 才负责三平台 Action executor、F411 表冠门控和真实亮灭屏/唤醒闭环。

### 8.4 学习结论

V0.5 的学习验收不是“拥有一个叫 PowerController 的文件”，而是能解释：

- 页面状态与 Power 状态为什么是两个状态机。
- Request、Action、平台执行和 commit 为什么不能合成一次调用。
- Controller 为什么不需要成为任务。
- 平台为什么只能执行动作，不能拥有产品策略。
- 为什么真实低功耗仍需要外设、电源域、唤醒源和测量闭环，不能用一次背光关闭代替。
