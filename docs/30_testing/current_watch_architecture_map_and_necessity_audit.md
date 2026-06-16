# 当前 Watch PC XML 架构地图与必要性审查

日期：2026-06-15  
范围：当前主线 `magic_watch_xml_sim`  
路由类型：按需当前事实审计

## 1. 本轮结论

当前 PC XML 主链不是一个包含 Service、EventBus、ScreenManager 的完整手表系统，
而是一条已经跑通的最小垂直闭环：

```text
PC SDL/LVGL Runtime
-> XML Generated View
-> PC UI Adapter
-> watch_core
-> PC UI Adapter
-> LVGL Screen
```

这条链里真正必要的边界是：

- 平台运行环境与产品状态分离
- generated View 与业务状态分离
- LVGL 输入先转换成 typed `UiEvent`
- `watch_core` 决定权威 `PageState`
- Adapter 根据 `PageState + Snapshot` 执行 View 更新

当前没有证据要求新增独立 `ScreenManager`、Service 或 EventBus。它们不是被遗漏，
而是本轮真实需求尚未产生对应压力。

## 2. 构建与启动链

构建目标由
`sim/lv_port_pc_vscode/CMakeLists.txt` 中的 `magic_watch_xml_sim` 组成：

- `xml_sim_main.cpp`
- `watch_core_ui_adapter.c`
- 旧模拟器复用的 `HAL.cpp` / `SimulatorDevice.cpp`
- `magic_watch_core`
- `magic_watch_xml_ui`
- LVGL / SDL

启动调用链：

```text
xml_sim_main
-> lv_init()
-> create_simulator_device()
-> Device::initialize(240x280)
-> magic_watch_ui_init()
-> watch_core_init()
-> watch_core_ui_adapter_init()
-> sync_core_to_view()
-> apply_page_state()
-> screen_health_shortcuts_create()
-> bind subjects + create hit targets
-> lv_screen_load()
```

主循环：

```text
steady_clock elapsed
-> device->tick(elapsed)
-> lv_timer_handler()
-> sleep <= 16 ms
```

这说明 PC 主线已经具备真实的周期运行环境。LVGL 刷新不是每次业务状态修改后
直接调用屏幕驱动完成，而是由 `lv_timer_handler()` 驱动 LVGL 内部刷新流程。

## 3. 当前真实输入链

健康卡点击实际走的是：

```text
SDL mouse
-> LVGL SDL pointer indev
-> Adapter 创建的透明 hit target
-> guarded_click_event_cb()
-> health_hit_target_clicked()
-> WatchCoreUiEvent
-> watch_core_push_event()
-> watch_core_process_pending_events()
-> WatchCorePageState
-> apply_page_state()
-> load_health_detail()
-> lv_screen_load()
```

Back 走同一条合同：

```text
LVGL Back button
-> guarded click
-> watch_core_make_back_event()
-> watch_core queue + Coordinator
-> PageState = HEALTH_SHORTCUTS
-> Adapter 重建并加载健康四卡页
```

关键事实：

- 页面导航规则由 `watch_core` 决定。
- LVGL 对象创建和 screen load 由 Adapter 执行。
- 没有独立 `ScreenManager`，但“页面决策”和“页面执行”已经分开。

## 4. 当前真实数据链

当前已接通两条数据链：

### 4.1 初始化 Snapshot

```text
watch_core_init()
-> WatchCoreUiModelSnapshot 默认值
-> Adapter 初始化 LVGL subjects
-> label 绑定 subject
-> apply_snapshot_to_subjects()
```

### 4.2 动态电池链

```text
SimulatorDevice::tick()
-> BatteryChanged
-> xml_sim_main.cpp device callback
-> WatchCoreBatteryState
-> watch_core_set_battery_state()
-> battery_dirty = true
-> watch_core_ui_adapter_sync_snapshot()
-> battery subject
-> 首页 battery label
```

当前这条链已经证明：Simulator 电池样本可以进入 `watch_core` 权威 Snapshot，
再通过 Adapter 同步到 XML 首页；详情页期间 Core 继续更新，返回首页后显示最新值。

当前 `magic_watch_xml_sim` 仍然没有调用：

- `watch_core_set_health_metric()`
- `watch_core_request_power_action()`
- `watch_core_commit_power_action()`

因此时间、活动数据和 Power executor 还没有进入这条 PC XML 主链。文档中的：

```text
Hardware / Simulator -> Service -> watch_core ModelStore
```

仍是目标边界，不是当前已接通事实。

## 5. PC HAL 的真实作用

`SimulatorDevice` 同时提供两类能力：

1. 当前 XML 主链实际使用的能力：
   - SDL window
   - LVGL mouse / wheel / keyboard indev
   - SDL event pumping

2. 旧模拟器使用、但 XML 主链未消费的能力：
   - time / battery / activity 事件
   - button / crown / touch 语义事件
   - notification / debug 事件

`xml_sim_main.cpp` 现在已经调用 `Device::set_event_callback()`，但当前只消费
`BatteryChanged`。因此第二类事件里只有电池样本进入了 `watch_core`；time /
activity / crown / touch / debug 等其它业务事件仍然没有接收者。

当前甚至存在两条输入表示：

```text
实际生效：SDL mouse -> LVGL indev -> Adapter
当前未接：SimulatorDevice -> HAL Touch/Crown/Button Event -> 无 callback
```

结论：复用 `Device` 对显示和 LVGL 输入初始化有价值，但它携带的完整旧 HAL
业务事件模型对新 XML 主线而言目前过宽，不能把“代码存在”误认为“架构已接通”。

## 6. XML View 与 UI helper

### Generated View

`screen_health_shortcuts_gen.c` 和 `health_shortcut_card_gen.c` 负责：

- 创建对象
- 布局、样式和资源
- 健康卡初始文本

它们不持有业务状态，也不直接调用 `watch_core`，边界合理。

### `magic_watch_ui.*`

当前真正有用的职责：

- 包装 generated UI 初始化
- 集中查找 card 和 metric label，隔离部分生成对象层级细节

当前未生效的职责：

- `magic_watch_ui_set_health_card_event_handler()`
- `magic_watch_health_card_clicked()`

XML 和生成 C 没有把 click event 挂到
`magic_watch_health_card_clicked()`；`card_id` 参数在生成 component 中也未被使用。
真实点击由 Adapter 的透明 hit target 完成。

结论：UI helper 这个边界有必要，但健康卡 callback 这一支目前是未接通路径，
不能和透明 hit target 同时都被描述成活跃输入链。

## 7. Adapter 的必要性与负担

`WatchCoreUiAdapter` 当前承担：

- LVGL 输入转 typed `UiEvent`
- tap/drag guard
- Core queue push 和 drain
- Snapshot 到 LVGL Subject
- 权威 `PageState` 到 screen
- screen 创建、加载和旧 screen 删除
- 手写详情占位 View
- 对生成对象层级和布局常量的适配

### 为什么它现在必要

删除 Adapter 后，XML callback 将不得不直接知道：

- `WatchCore`
- EventQueue
- PageState
- Snapshot
- 页面加载规则

这会让 View 再次拥有业务和导航知识，PC/F411 共享合同也会失去落点。

### 当前风险

Adapter 已经同时容纳输入策略、数据绑定、导航执行和手写详情 View。当前只有
两个 PageState，这个规模仍可接受；如果页面和输入继续增长，它会成为第一个
需要按职责审查的模块。

现在不应仅因为文件较长就拆分。真正的拆分触发条件应是：

- 多个页面重复生命周期代码
- 输入 guard 被多个 View 复制
- 数据同步需要独立周期或 Service
- 页面执行规则不能再由单个 `apply_page_state()` 清楚表达

## 8. `watch_core` 的必要性与限制

`watch_core` 当前拥有：

- 固定容量 `UiEvent` 队列
- 健康数据 Snapshot
- 权威页面状态
- 导航 Coordinator
- 最小 Power 决策合同

它是必要层，因为同一套状态和行为合同已经被 PC/F411 共同使用，并且可以脱离
LVGL 做纯 PC 合同测试。

但要区分：

- EventQueue 合同确实存在。
- PC XML 点击路径仍是 `push -> 立即 drain`。
- 当前 PC 运行时尚未证明多生产者、异步排队或优先级需求。

因此不能从“有队列”直接推导出当前还需要 EventBus、任务调度或事件优先级。

## 9. 是否需要这些层

| 候选层 | 当前判断 | 证据与删除后果 |
| --- | --- | --- |
| PC Platform Runtime | 必要 | 提供 SDL/LVGL display、indev 和周期主循环；删除后 PC 无运行环境 |
| 完整旧 HAL Event 模型 | 当前只部分必要 | display/indev 初始化在用，业务事件 callback 未接；不能宣称已进入新主线 |
| XML / generated View | 必要 | 承担 UI 源、布局、样式和对象创建；删除后会退回手写 PC View |
| UI helper | 部分必要 | init 和对象访问可隔离生成层；健康卡 callback 路径当前未接通 |
| PC UI Adapter | 必要 | 隔离 LVGL 与 `watch_core`，执行 Snapshot/PageState 到 View 的转换 |
| `watch_core` | 必要 | PC/F411 共享权威状态、事件和行为合同 |
| 独立 ScreenManager | 当前不需要 | 页面决策已在 Core，页面执行已在 Adapter；只有两个 PageState |
| Service 层 | 当前未接入 | 没有动态数据源写入 Core；真实时间/电池/传感器接入时再建立 |
| EventBus | 当前不需要 | 当前只有 Adapter 同步 push/drain；没有跨 Service 广播或多订阅证据 |
| 自定义 Render/Dirty 层 | 当前不需要 | LVGL 已由 `lv_timer_handler()` 周期刷新；应用层缺口是动态状态注入，不是重造渲染器 |

## 10. 当前最值得继续追的缺口

本轮不建议立刻重构 Adapter，也不建议先清理 dormant callback。

下一条最有学习价值的真实链路不再是“电池为何止于 HAL callback”，而是：

```text
Power 请求
-> `watch_core` 最小 Power 合同
-> PC executor
-> screen off/on 或等价可观察现象
-> View / Runtime 在什么时机与 Core 同步
```

这条链能直接回答 Stage 06 留下的问题：

- Power 决策是否需要独立 executor
- PC Runtime 和 `watch_core` 的职责边界在哪里
- View 同步应由事件驱动还是运行时轮询触发
- 在已有动态电池闭环后，下一条可观察系统行为应如何最小接入

在这条链审清之前，不应先加入 EventBus、通用 Scheduler 或新的 Power 名词。

## 11. 当前架构图

```text
                         当前未接通
SimulatorDevice battery event
                         |
                         v
                 xml_sim_main callback
                         |
                         v
            watch_core_set_battery_state()

SimulatorDevice time/activity/crown/touch events
                         |
                         X  no active consumer

SDL window / LVGL indev
          |
          v
XML Generated View
          |
          v
transparent hit target + click guard
          |
          v
WatchCoreUiAdapter
  LVGL -> typed UiEvent
          |
          v
watch_core EventQueue
          |
          v
Coordinator -> PageState + Snapshot
          |
          v
WatchCoreUiAdapter
  subjects + screen create/load/delete
          |
          v
LVGL object tree
          |
          v
lv_timer_handler() -> SDL display
```

## 12. 审计边界

本轮没有：

- 审查旧 `magic_watch_sim` 的 C++ `Application / DataCenter / PageManager`
- 审查 F411 Input Port / Adapter / Lite View
- 运行手动 UI 回归
- 修改任何产品代码

这些都必须作为独立闭环，不能因为架构地图已经出现就自动开始。
