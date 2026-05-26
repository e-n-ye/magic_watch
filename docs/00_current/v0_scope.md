# Magic Watch Current v0 Scope

日期：2026-05-26

本文件是当前阶段使用的 v0 范围摘要。完整历史版本仍在 `docs/v0_scope.md`。

## 当前 v0 目标

v0 的目标不是做完整智能手表产品，而是在 PC LVGL 模拟器中验证一套可解释、可维护、可扩展、未来可接硬件的手表应用架构。

## 当前必须保留的能力

- `HomeRingHost` 作为默认主页。
- 表盘中心交互、Launcher、Notifications、QuickSettings、PowerMenu、ScreenOff、LongBattery 等系统壳层仍可回归。
- 输入路径保留 `HAL -> InputIntentRouter -> DataCenter/EventBus -> AppStateMachine`。
- 平台样本保留 `HAL sample -> Service -> DataCenter -> Pages`。
- 页面导航仍由 `AppStateMachine` 决策，`PageManager` 执行显示。

## 当前暂停扩张的内容

- 暂停继续新增健康、通知、设置功能页。
- 暂停真实血氧、心率、睡眠、NFC、支付、蓝牙等业务集成。
- 暂停继续把页面占位内容做细，除非它服务于架构收口验证。

## 当前验收标准

- 新会话能用 3-5 个当前文档理解项目方向。
- 状态变量和事件路径可以被解释。
- 新增电源策略时，不需要修改输入、通知、主页等无关领域。
- Controller 不直接操作 UI 或 `PageManager`。
- UI 页面生命周期规则清楚。
- 硬件接入边界逐步明确。
