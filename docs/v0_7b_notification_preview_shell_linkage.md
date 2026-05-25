# v0.7b 通知预览与壳层联动

日期：2026-05-24

## 目标

本轮只收口两件事：

- 明确“什么时候弹通知预览，什么时候只进通知列表”
- 让 `NotificationWakePreview` 能顺着壳层进入完整 `Notifications` 列表

本轮不做：

- 通知详情页
- 已读 / 未读模型
- 清空策略重构
- 手机同步
- 分类中心或通知权限体系

## 当前前后对比

### 改之前

- `NotificationWakePreview` 已经能显示共享通知模型
- 但它更像一个 5 秒后自动消失的独立壳层
- 用户没有稳定路径从预览进入完整通知列表
- 通知列表已经打开时，新的通知仍可能继续叠 toast

### 改之后

- `NotificationWakePreview` 仍然只在特定场景下出现
- 用户可以通过点击预览卡片或按下表冠进入完整 `Notifications`
- 如果通知列表已经打开，新的通知只刷新列表，不再额外叠 toast
- 通知预览与通知列表成为同一条壳层链路上的两个阶段

## 本轮规则

### 1. 什么时候弹 `NotificationWakePreview`

只有在下面这个条件下：

- 当前处于 `ScreenOff`
- 且 `notification_wake_enabled == true`
- 且收到新的通知

此时行为为：

```text
通知到达
-> 唤醒屏幕
-> 打开 NotificationWakePreview
```

### 2. 什么时候只显示 toast

当系统处于 `Running`，且当前不在通知列表页时：

```text
通知到达
-> 不弹 wake preview
-> 只显示 toast
```

### 3. 什么时候只刷新列表

当系统已经停留在 `Notifications` 列表页时：

```text
通知到达
-> 不弹 wake preview
-> 不再额外叠 toast
-> 只刷新 Notifications 列表内容
```

## 本轮交互收口

### 预览 -> 列表

新增两条壳层联动路径：

- 点击 `NotificationWakePreview` 的通知卡片
- 在 `NotificationWakePreview` 上按下表冠

两者都会：

```text
NotificationWakePreview
-> Notifications
```

并且：

- 结束“通知唤醒临时会话”
- 转入正常通知列表浏览上下文

## 边界

### 修改

- `AppStateMachine`
  - 明确通知到达时的三种分流：
    - `ScreenOff` -> preview
    - `Running` -> toast
    - `Notifications` 已打开 -> 只刷新列表
  - 新增从 `NotificationWakePreview` 进入 `Notifications` 的壳层切换

- `NotificationWakePage`
  - 通知卡片支持点击进入完整通知列表
  - 当预览页收到新的通知模型刷新时，自动关闭计时重置

### 保持不变

- `NotificationsPage` 列表结构
- `NotificationCenterModel`
- `NotificationService`
- `BatteryPowerService`
- `NotificationToastOverlay`

## 验收标准

- `N` 注入通知后：
  - 若当前为 `ScreenOff` 且允许通知亮屏，出现 `NotificationWakePreview`
  - 点击预览卡片可进入完整 `Notifications`
  - 预览页按表冠也可进入完整 `Notifications`
- 若当前已在 `Notifications`：
  - 新通知只刷新列表，不再额外叠 toast
- `B` 低电量通知基线仍然正常
- 长续航与 Steps 主链路不被破坏

## 下一轮建议

如果这轮稳定，下一步再进入 `v0.7c`：

- 已读 / 清空
- 通知详情
- 是否需要动作按钮

在这之前，不建议继续扩大通知功能面。
