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

当前实现细节，不应直接固化成长期合同：

- `WatchCore` 结构体内部字段的排列方式。
- `queue_head / queue_tail / queue_count` 的具体实现字段名。
- 目前 `current_page` 存在于 `WatchCore` 本体并被 F411 bridge 镜像。
- F411 当前用“两块 panel hidden/unhidden”表达页面切换，而不是独立 LVGL screen create/load。
- 表冠输入先进入 LVGL encoder indev，再由 focused clickable 控件回调间接生成 typed `UiEvent`；这是一条当前实现路径，不是长期唯一语义合同。

## 3. F411 新旧主链所有权审计

### 3.1 当前运行主链

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

### 3.2 逐项所有权结论

原始表冠输入由谁读取：

- `watch_input_service_scan_10ms()` 负责扫描，`watch_input_service_get_event()` 负责出队。
- `freertos.c` 周期调用 `watch_bringup_scan_input_10ms()`，`watch_bringup_task()` 再消费事件。

`watch_input_intent` 当前承担什么：

- 只做 `watch_input_event_t -> watch_input_intent_t` 的薄转换。
- 当前不直接生成 typed `UiEvent`，也不直接决定页面导航。

typed `UiEvent` 在哪里生成：

- 当前由 `watch_lvgl_debug_screen.c` 在卡片点击、屏上 `Back` 点击和左边缘右滑返回提交处生成：
  - `watch_core_make_health_card_clicked_event(...)`
  - `watch_core_make_back_event()`

EventQueue 的权威实现是哪一套：

- 当前运行主链实际使用的是 `watch_core` 内部固定容量队列。
- 证据：`watch_core_bridge_dispatch_event()` 调用 `watch_core_push_event()` 和 `watch_core_process_next_event()`。
- 旧 `user/core/event/watch_event_queue.*` 仍被编译，但没有进入当前运行主链。

当前页面状态分别存在哪些位置：

- 权威页面语义状态：`WatchCore.current_page`
- F411 当前镜像状态：`watch_core_bridge_state_t.current_page`
- F411 当前视图展开状态：`watch_lvgl_debug_screen.c` 里 `s_shortcuts_panel / s_detail_panel` 的 hidden/visible

Coordinator 决策在哪里发生：

- 当前仍在 `watch_core_process_next_event()` 内完成。
- F411 没有第二套独立 Coordinator。

Snapshot 在哪里应用到 View：

- `watch_core_bridge_sync_state()` 从 `watch_core` 拉取 snapshot。
- `watch_lvgl_debug_screen.c` 的 `apply_snapshot_to_view()` 把 snapshot 写到 LVGL label。

`PageIntent` 在哪里转换成 LVGL 页面变化：

- `watch_core_bridge_dispatch_event()` 保存 `last_intent` 和 `current_page`。
- `watch_lvgl_debug_screen.c` 的 `apply_page_to_view()` 再把 `current_page` 转成 header 文案、detail/shortcut 面板切换和 focus group 重建。

旧 `watch_event_queue` 和 `watch_screen_manager` 是否仍被编译：

- 是。`my_watch_f411.uvprojx` 仍登记了两者。

它们是否仍进入当前运行主链：

- 否。仓库内搜索未发现 `watch_event_queue_*` 或 `watch_screen_manager_*` 在 bring-up、task、LVGL port 或当前 Lite UI 路径中被调用。
- 这两套当前属于“被编译，但未初始化、未调用、未掌权”。

当前是否已经存在事实上的 `F411UiAdapter` 职责组合：

- 已经存在，但职责分散在三个文件：
  - `watch_core_bridge.c`：core 生命周期、事件派发、intent/snapshot 镜像
  - `watch_lvgl_debug_screen.c`：View 构建、snapshot 应用、page intent 执行、LVGL 回调到 typed `UiEvent`
  - `watch_lvgl_port.c`：encoder/pointer 输入语义、tap-only、防误触、左边缘右滑状态

这些职责是否已出现会阻碍下一功能的重复或泄漏：

- 是。
- 当前“typed event 入口、snapshot 应用、page intent 执行、视图生命周期”没有单一装配点。
- 旧 `watch_screen_manager` 虽然仍在工程里，但它并未拥有当前运行链路；继续先隔离它，只会降低工程噪音，不会解决当前真正的扩展障碍。

## 4. 编译 / 初始化 / 调用 / 运行状态区分

| 模块 | 被编译 | 被初始化 | 被调用 | 当前运行主链实际使用 |
|---|---|---|---|---|
| `watch_core` | 是 | 是 | 是 | 是 |
| `watch_core_bridge` | 是 | 是 | 是 | 是 |
| `watch_lvgl_debug_screen` | 是 | 是 | 是 | 是 |
| `watch_lvgl_port` | 是 | 是 | 是 | 是 |
| `watch_input_service` | 是 | 是 | 是 | 是 |
| `watch_input_intent` | 是 | 否（纯函数） | 是 | 是 |
| 旧 `watch_event_queue` | 是 | 否 | 否 | 否 |
| 旧 `watch_screen_manager` | 是 | 否 | 否 | 否 |

## 5. 第二张卡建议

建议选择：

**抽取真正的 `F411UiAdapter`**

理由：

1. 当前真正阻碍下一功能的不是旧 `ScreenManager` 仍掌权，而是它已经不掌权了，但新的 Adapter 职责散落在 bridge / debug screen / lvgl port 三处。
2. Snapshot 应用和 `PageIntent` 执行现在分裂在 `watch_core_bridge.c` 与 `watch_lvgl_debug_screen.c`，后续一旦继续扩详情页、更多输入语义或多页面生命周期，容易再次复制逻辑。
3. `watch_event_queue` 和 `watch_screen_manager` 目前虽然还在 MDK 工程里，但只是“被编译的旧残留”，并未进入当前运行主链；单独优先隔离它们的收益主要是清噪音，不是解除当前架构阻塞。

建议下一张卡聚焦：

- 明确一个 F411 侧单点 Adapter 装配边界
- Adapter 负责接收已经形成的输入结果、将 UI 激活或 Back 语义转换成 typed `UiEvent`、驱动 `watch_core`、获取并应用 `UiModelSnapshot`、消费 `PageIntent` 并协调 Lite View 页面表现
- `watch_lvgl_port` 继续保留硬件读取、indev 注册、原始坐标、tap-only、防误触、左边缘右滑和 `WATCH_TOUCH_SWIPE_BACK_COMMIT_DISTANCE = 36U` 这类手感阈值
- Lite View 继续保留 LVGL 对象创建和具体视觉更新，不持有业务状态，不直接决定产品导航
- 不在该卡顺手扩页面、改 DMA、改输入驱动或清理旧工程残留

## 6. 对 V0.5 后续拆分的修正

原先 `V0.5-A ~ G` 把 `EventQueue`、`Coordinator`、`Snapshot` 等内容视为待从零实现，与当前代码事实不符。

本轮后建议按以下方向重排：

1. `V0.5-P0-A`：共享合同测试与 F411 所有权审计
2. `V0.5-P0-B`：抽取真正的 `F411UiAdapter`
3. `V0.5-P0-C`：基于抽取后的边界，再评估是否需要单独隔离旧 `watch_screen_manager` / `watch_event_queue`
4. `V0.5-P1-*`：仅对当前代码里确实缺失的 `ScreenId`、电源语义和三平台共用边界继续拆卡

这表示 V0.5 的核心已经从“空白造 core 模块”转成“给现有共享合同装护栏，并把 F411 当前所有权收口到可扩展边界”。
