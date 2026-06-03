# Magic Watch v0.1 Architecture Closure

Date: 2026-05-16

本文记录 v0.1 的一次小型架构收口：不增加真实业务能力，只把当前模拟器壳层中的显示唤醒策略从通知和页面局部状态中分离出来。

## Current Responsibility Split

当前主链路保持不变：

```text
HAL / SimulatorDevice
  -> Application
  -> InputIntentRouter
  -> DataCenter / EventBus
  -> AppStateMachine
  -> PageManager
  -> Page / UI
```

- `InputIntentRouter` 负责把按键、触摸、表冠等原始输入翻译成应用层输入意图。
- `AppStateMachine` 负责真正的系统状态和页面跳转，例如 `Running`、`ScreenOff`、`PoweredOff`、主页环、临时壳层页面。
- `PageManager` 只负责如何显示页面、管理根页面、普通页面栈和临时页面。
- `DataCenter` 保存跨页面共享模型，并通过 `EventBus` 发布更新。

## Display Policy Model

v0.1 新增 `DisplayPolicyModel`，先作为轻量模型放在 `AppEvents` / `DataCenter` 层。

它当前表达：

- 表冠按下是否允许亮屏。
- 通知是否允许从熄屏态唤醒预览。
- 翻腕亮屏开关。
- 单击亮屏占位。
- 持续亮屏占位。
- 自动熄屏开关和默认超时时间。

这不是最终低功耗实现，也不接真实 IMU、触控控制器或显示驱动。它只是先把“谁允许唤醒屏幕”这类策略从通知中心和页面局部状态里拿出来。

## Quick Settings Boundary

Quick Settings 现在可以更新显示策略模型：

- `通知亮屏` 更新 `DisplayPolicyModel::notification_wake_enabled`。
- `翻腕亮屏` 更新 `DisplayPolicyModel::raise_to_wake_enabled`。
- `持续亮屏5分钟` 先更新 `DisplayPolicyModel::always_on_display_enabled`，但暂不改变真实熄屏行为。

通知列表仍由 `NotificationCenterModel` 管理。通知中心不再拥有 `wake_on_notification` 这类显示策略字段。

## State Machine Boundary

`AppStateMachine` 仍然拥有真实状态切换：

- 表冠按下可以从熄屏态回到表盘，前提是显示策略允许。
- `screen_off_timeout_ms` 现在会驱动模拟器中的自动熄屏计时器，默认 5 秒无输入后进入熄屏模拟态。
- 通知到达时，如果当前是熄屏态，状态机读取 `DisplayPolicyModel::notification_wake_enabled` 决定是否打开通知唤醒预览。
- `P` / `5` 只作为 PC 模拟器调试输入，用于手动切换熄屏或打开电源菜单，不代表真实手表硬件。

这保持了一个重要边界：策略数据在 `DataCenter`，状态执行在 `AppStateMachine`。

## Still Mock

以下内容仍然是 v0 shell / mock，不代表真实硬件或真实业务已经完成：

- 翻腕亮屏没有接真实 IMU。
- 单击亮屏没有接低功耗触控扫描。
- 持续亮屏没有接真实屏幕功耗策略。
- 自动熄屏只是 LVGL 模拟器状态切换，不代表真实显示驱动已经进入低功耗。
- 通知仍然是模拟注入，不是手机同步。
- Quick Settings 中除显示策略相关项外，多数开关仍是 UI shell。

## Simulator Input Contract

- `P`: PC 模拟器调试键，短按切换熄屏，长按打开电源菜单。
- `5` / 小键盘 `5`: `P` 的历史兼容别名，暂时保留。
- `Enter` / `Space`: 表冠按下。
- `Q` / `E`: 表冠逆时针 / 顺时针旋转。
- `N`: 注入一条模拟消息通知。
- `B`: 注入一条模拟低电量通知。

## Next Suggested Step

下一步可以继续沿着同一方向收口：

- 把主页环相关状态从 `AppStateMachine` 中进一步整理成清晰子区域，必要时再抽出 `HomeSurfaceCoordinator`。
- 为 `DisplayPolicyModel` 增加更明确的页面入口，让 Display 设置页能展示这些策略，而不是只停留在 Quick Settings。
- 继续保持真实业务延后，先确保架构边界稳定。
