# v0.4 阶段收口

日期：2026-05-22

## 结论

v0.4 `系统骨架与平台契约收口` 已完成当前目标。

本阶段没有继续堆模拟器页面，也没有继续随机硬件实验，而是把 Magic Watch 从“模拟器 UI 预研 + T-Watch 前哨验证”的摇摆状态，收束为一条更清楚的系统主线：

```text
Platform / HAL sample
  -> Application wiring
  -> service / model boundary
  -> DataCenter
  -> StateMachine / Page
```

这条链路还很轻，但已经能解释下一步代码应该放在哪里。

## 已完成的小闭环

| 轮次 | 输出 | 结果 |
| --- | --- | --- |
| 第一轮 | `docs/v0_4_system_skeleton_entry.md` | 明确暂停随机硬件实验，不继续优先堆 mock 页面。 |
| 第二轮 | `docs/v0_4_simulator_system_boundary_audit.md` | 审计 `Application`、`InputIntentRouter`、`AppStateMachine`、`PageManager`、`DataCenter`、`DisplayPolicyRules`、`SimulatorDevice` 的职责。 |
| 第三轮 | `docs/v0_4_platform_hal_event_contract.md` | 明确 HAL 事件与 T-Watch 已验证能力的映射边界。 |
| 第四轮 | `docs/v0_4_battery_power_service_boundary.md` | 落地第一个真实 service 边界：`BatteryPowerService`。 |

## 当前架构状态

### 已经变清楚的部分

- `Application` 仍是组合根，但电池解释逻辑已经移出。
- `BatteryPowerService` 接收 `hal::BatterySample`，发布 `BatteryModel`，处理低电量阈值和去重 latch。
- 页面仍然只订阅 `BatteryModel`，不解释 PMU 或硬件字段。
- `InputIntentRouter` 继续负责输入样本到高层 intent 的翻译。
- `AppStateMachine` 继续负责 Running / ScreenOff / PoweredOff、主页环、临时壳层和 wake / restore 语义。
- `PageManager` 继续只负责页面注册、实例、栈和临时页面显示。
- `DisplayPolicyRules` 继续作为无副作用规则层。
- `DebugSample` 已明确为 simulator-only，不进入真实硬件契约。

### 仍然保留的债务

- `Application` 仍包含大量页面注册，这是组合根职责，短期可接受。
- `Application` 仍包含 mock message notification 生成，未来若通知服务真实化，应迁出。
- `NotificationToastOverlay` 仍在 `Application.cpp` 内部，短期可接受；通知流扩展时再拆。
- `DataCenter` 仍是轻量 model hub，后续要避免把所有业务解释都塞进去。
- `TouchSample` 当前是手势级事件，不是裸触摸点；真实硬件手势识别尚未闭环。
- `MotionSample` 只承载基础三轴数据；真实 raise-to-wake 中断和算法尚未验证。
- screen off / wake 当前是运行态系统状态，deep sleep 后重启式恢复还不是正式生命周期契约。

## 验证结果

- 构建通过：`cmake --build build --config Debug`。
- 手工回归通过：启动模拟器后按 `B` 热键，低电量通知仍出现。
- 文档闭环完成：新会话可通过 v0.4 文档理解为什么不继续随机硬件实验、不继续堆页面，以及为什么先选电池 service。

## v0.4 不继续做什么

v0.4 到这里收口，不继续追加以下内容：

- 不继续抽第二个 service。
- 不把 `Time`、`Motion`、`Notification` 立即 service 化。
- 不扩展 `HAL.h` 的 PMU 细节字段。
- 不接回 T-Watch 做新的硬件实验。
- 不实现真实通知、天气、计步或健康服务。
- 不引入通用 service framework、RTOS 任务体系或 QPC。

## 下一阶段入口

下一阶段进入：

`v0.5 Power Status 页面闭环`

目标不是“加一个普通页面”，而是给 v0.4 抽出的 `BatteryPowerService` 增加一个真实 UI 消费者，验证：

- 平台层只提供样本。
- service 层解释领域状态。
- `DataCenter` 暴露模型。
- 页面只订阅模型并渲染。

入口文档：

- `docs/v0_5_power_status_page_entry.md`
