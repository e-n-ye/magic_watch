# v0.7 Notification Service 最小闭环

日期：2026-05-24

## 目标

本轮只做一件事：

- 把模拟器消息通知从 `Application` 里的直接 mock 注入，
  收口成 `Platform sample -> NotificationService -> DataCenter -> Notifications page`

本轮不做：

- 通知详情页
- 已读 / 未读
- 清空策略优化
- 手机侧同步
- BLE / 权限 / 去重
- 通知预览行为重设计

## 为什么现在做它

前面已经完成了两条同类真实链路：

- `BatterySample -> BatteryPowerService -> DataCenter`
- `ActivitySample -> StepsActivityService -> DataCenter`

通知现在虽然已经有：

- `Notifications` 列表页
- `NotificationWakePreview`
- `NotificationToastOverlay`
- `NotificationCenterModel`

但消息通知的生产入口仍然偏重：

- 模拟器按 `N`
- `SimulatorDevice` 发一个 debug action
- `Application` 直接构造 mock `NotificationItem`
- `DataCenter::push_notification(...)`

这意味着：

- 通知页虽然已经在消费共享模型
- 但“消息通知如何进入系统”还没有被整理成 service 边界

本轮的价值不是扩功能，而是把这条半成品链路掰正。

## 当前前后对比

### 改之前

```text
N 热键
-> SimulatorDevice(DebugAction)
-> Application 直接 make_mock_message_notification()
-> DataCenter
-> Notifications / WakePreview / Toast
```

### 改之后

```text
N 热键
-> SimulatorDevice(NotificationSample)
-> NotificationService
-> DataCenter
-> Notifications / WakePreview / Toast
```

差别在于：

- `Application` 不再持有“怎么生成消息通知”的业务细节
- 平台层表达“收到了一条通知样本”
- service 层负责把样本解释成 app 层通知对象

## 本轮边界

### 新增

- `hal::NotificationSample`
- `NotificationService`

### 更新

- `SimulatorDevice`
  - `N` 热键改为发出 `NotificationSample`
- `Application`
  - 新增 `NotificationService`
  - 不再直接构造 mock 消息通知

### 保持不变

- `NotificationsPage`
- `NotificationWakePage`
- `NotificationToastOverlay`
- `DataCenter::NotificationCenterModel`
- 低电量通知仍由 `BatteryPowerService` 负责

## 当前语义

### 平台层

当前 `N` 热键不再代表“让 Application 塞一条假通知”，而是代表：

- 模拟器收到一条消息类通知样本

它仍然是 simulator-only，但已经更像真实平台语义。

### service 层

`NotificationService` 当前职责很窄：

- 接收 `NotificationSample`
- 生成 `NotificationItem`
- 分配最小通知 id
- 发布到 `DataCenter`

它现在还不负责：

- 分类规则
- 多来源去重
- 未读状态
- 持久化历史
- 通知动作

### UI 层

`NotificationsPage` 当前已经是共享模型消费者。

本轮的重点不是改它，而是让它消费的数据来源更合理。

## 验收标准

- 项目可构建
- 启动模拟器后按 `N`
- `Notifications` 列表页能出现新增消息通知
- 通知 toast 仍然出现
- `NotificationWakePreview` 现有行为不被破坏
- `B` 低电量通知仍然正常
- 长续航、电池、Steps 主链路不被这轮改坏

## 后续拆分建议

如果这轮稳定，后面再拆：

1. `v0.7b`
   - 通知预览与壳层联动收口
   - 明确什么情况下弹预览，什么情况下只进列表

2. `v0.7c`
   - 已读 / 清空 / 详情 / 动作按钮

在这之前，不建议把通知做成“大而全”的完整系统。
