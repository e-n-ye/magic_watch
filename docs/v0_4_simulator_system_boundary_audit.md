# v0.4 模拟器系统边界审计

日期：2026-05-22

## 目的

本文是 v0.4 的第二个小闭环：只审计当前 PC LVGL 模拟器中的系统边界，不改代码，不新增页面，不推进硬件实验。

本轮回答：

- 当前关键模块真实承担什么职责。
- 输入意图、系统状态、页面栈、显示策略、平台采样、mock / service 数据分别归谁。
- 后续新增 `Battery / PowerStatus`、`Time`、`Sensor`、`Notification` 等 service 时，应该优先落在哪个边界。

## 当前主链路

当前模拟器主链路仍然是：

```text
HAL / SimulatorDevice
  -> Application
  -> InputIntentRouter
  -> DataCenter / EventBus
  -> AppStateMachine
  -> PageManager
  -> Page / UI
```

这条链路不需要推倒重写。v0.4 的重点是把它讲清楚，并在 service 逐步出现时避免职责继续堆进 `Application`、页面或状态机。

## 模块职责表

| 模块 | 当前实际职责 | 应继续拥有 | 不应继续扩大 |
| --- | --- | --- | --- |
| `Application` | 组合根；注册页面；连接 HAL 回调；把 HAL 时间、电池、运动样本转为 app model；把输入交给 `InputIntentRouter`；生成少量 mock 通知。 | 组合与接线；最薄的 HAL-to-app 适配；启动状态机和 toast overlay。 | 不应继续承载业务服务、通知生成策略、电池策略或传感器解释逻辑。 |
| `InputIntentRouter` | 读取 `PageManager` 上下文，把 button / crown / touch / debug 事件翻译为 `InputCommand`；区分主页环和普通页面中的同一手势语义。 | 原始输入到高层 intent 的语义分流。 | 不应执行导航、不应改模型、不应持有系统状态。 |
| `AppStateMachine` | 消费导航和输入 intent；维护 `PowerState`、`ShellSurface`、主页环索引、screen off 页面恢复、通知唤醒会话、raise-to-wake 会话、auto screen off / keep screen on timer。 | 系统状态、shell 行为、主页环流转、screen off / wake / restore 和 timer 执行。 | 不应继续吸收平台驱动细节、真实 PMU 规则、传感器算法或页面内部 UI 流。 |
| `PageManager` | 页面工厂注册；页面实例缓存；根页面、普通页面栈、临时页面；页面转场；screen off 前后状态捕获与恢复。 | “如何显示页面”和“页面栈是什么”。 | 不应决定为什么跳转、不应解释输入、不应拥有业务状态。 |
| `DataCenter` | 保存最近时间、电池、运动、通知、显示策略和主页环预览模型；提供同步 `EventBus`；提供显示策略和通知的 setter。 | 共享模型存储与事件发布；短期可作为轻量 model hub。 | 不应无限膨胀为所有业务真相；后续 service 出现后，复杂解释逻辑应移出。 |
| `DisplayPolicyRules` | 纯规则函数：时间窗口、raise-to-wake 是否允许、screen-off display 是否激活、auto screen off 是否抑制、冲突判断。 | 无副作用规则判断。 | 不应拥有 timer、页面跳转、确认弹窗或渲染逻辑。 |
| `SimulatorDevice` | 初始化 SDL/LVGL 输入设备；生成时间、电池模拟样本；识别键盘、鼠标手势、调试热键；发出 HAL event。 | PC 平台实现和模拟数据源。 | 不应承载 app 层语义，不应直接知道页面或显示策略。 |

## 状态归属表

| 状态或数据 | 当前归属 | 判断 |
| --- | --- | --- |
| 原始时间、电池、触摸、表冠、按键、运动、debug 样本 | `HAL.h` 类型，由 `SimulatorDevice` 产生 | 合理；它表达“硬件侧发生了什么”。 |
| 高层输入意图 | `InputIntentRouter` 输出 `InputCommand`，`DataCenter` 发布 | 合理；后续真实平台也应复用这层意图。 |
| 电源运行态 | `AppStateMachine::PowerState` | 合理；Running / ScreenOff / PoweredOff 不应下沉到页面。 |
| 临时壳层页面 | `AppStateMachine::ShellSurface` + `PageManager::temporary_page_` | 可接受；状态机决定语义，PageManager 保存显示结构。 |
| 页面栈和当前可见页 | `PageManager` | 合理；页面管理器不解释跳转原因。 |
| 主页环索引和预览进度 | `AppStateMachine` + `HomeRingPreviewModel` | 当前可接受；主页环是系统壳层，不是普通页面栈。后续若继续膨胀，再考虑 `HomeSurfaceCoordinator`。 |
| screen off 前页面恢复状态 | `AppStateMachine` 捕获，`PageManager::State` 承载 | 合理；这是系统状态和页面栈的交界。 |
| 显示策略配置 | `DataCenter::DisplayPolicyModel` | 当前合理；后续可由 service 或 coordinator 包装，但模型不应长在页面里。 |
| 显示策略纯判断 | `DisplayPolicyRules` | 合理；已经减少多处重复。 |
| 显示策略执行 timer | `AppStateMachine` | 当前合理；timer 直接影响 screen off / wake 状态切换。 |
| 显示策略设置和确认弹窗 | `SettingsPages` | 当前合理；这是 UI 选择流，不是系统执行流。 |
| screen off 实际渲染 | `ScreenOffPage` | 当前合理；渲染不应搬进状态机。 |
| 通知列表和 toast 活跃项 | `DataCenter::NotificationCenterModel` | 当前可接受；真实通知 service 出现后应移出通知生成和过滤逻辑。 |
| mock 通知生成 | `Application` | 暂可接受但偏重；后续应迁到 mock service 或 debug injection service。 |
| 电池低电量通知 latch | `Application` | 当前是明确债务；适合未来 `Battery / PowerStatus service` 接走。 |
| 页面本地 UI 控件和未确认编辑值 | 各 `PageBase` 子类 | 合理；页面只拥有控件生命周期和局部编辑态。 |

## 对未来 service 的落点判断

### Battery / PowerStatus service

这是第一批真实 service 的推荐入口。

原因：

- `SimulatorDevice` 已经产生 `BatterySample`。
- T-Watch 已验证 AXP2101 基础电池、电压、充电和外部供电状态。
- 当前 `Application` 里已经出现低电量通知 latch，说明电池策略开始超过单纯模型转发。

建议边界：

- Platform / HAL：只提供 `BatterySample`。
- Battery service：解释电池、电压、充电、外部供电、低电量阈值和去重 latch。
- DataCenter：保存对 UI 暴露的 `BatteryModel` 或后续 `PowerStatusModel`。
- UI：只订阅模型并渲染。
- AppStateMachine：只消费必要事件，例如低电量触发的系统提示，不直接读 PMU 细节。

### Time service

当前时间只是 `SimulatorDevice -> Application -> DataCenter` 的直通链路，尚未出现复杂策略。

暂不急着抽 service。等出现时区、RTC 同步、手机同步、闹钟触发或日切事件时再提炼。

### Sensor / Motion service

当前 `MotionSample` 只保存基础 X/Y/Z，raise-to-wake 仍由 debug action 模拟。

暂不急着做真实姿态 service。下一步应先在 Platform / HAL 契约文档里明确 BMA / 模拟姿态如何进入 `MotionSample` 或高层 intent，再决定是否抽 service。

### Notification service

当前通知大部分是 mock 和 debug injection。

不建议作为第一批真实 service，因为真实通知会迅速牵涉 BLE、手机侧协议、分类、权限、去重、toast、wake preview 和历史列表，影响面大于电池。

## 当前可接受的债务

- `Application` 仍有 toast overlay 和 mock 通知生成，短期可接受；未来 service 化时应拆薄。
- `AppStateMachine` 内部有 `EdgeBackOverlay` 这种少量 UI 反馈代码，当前可接受；若手势反馈继续复杂，应迁出到 shell UI 层。
- `DataCenter` 的 setter 已经覆盖较多显示策略和通知行为，仍可接受；service 出现后要避免继续把业务解释堆进这里。
- `SimulatorDevice` 同时负责 SDL 初始化、手势识别和模拟数据源，当前可接受；真实硬件适配前应把“设备输入”和“模拟数据源”继续讲清楚。

## 下一轮建议

第三轮进入：

`Platform / HAL 事件契约收口`

边界：

- 对齐当前 `HAL.h` 事件类型与 T-Watch 已验证能力。
- 重点看 `TouchSample`、`MotionSample`、`BatterySample`、`CrownSample` / 按键替代、screen off / wake。
- 优先写契约，不急着改类型。

验收：

- 能说明模拟器输入和 T-Watch 输入分别映射到哪些 app intent。
- 能说明哪些仍是 debug 替代语义。
- 能判断 `Battery / PowerStatus service` 第一轮代码应该接收哪些输入、输出哪些模型。
