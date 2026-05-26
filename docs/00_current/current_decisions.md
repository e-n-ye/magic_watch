# Magic Watch Current Decisions

日期：2026-05-26

本文件只保留当前仍有效、会影响下一轮开发的决策。历史原因查 `docs/decision_log.md`，但它不再作为默认阅读文档。

## D001: 当前阶段进入架构收口

当前优先目标不是继续新增功能页，而是恢复系统解释力、拆清职责边界、压缩默认文档入口。

## D002: `HomeRingHost` 是当前默认主页

旧 `Watchface` 仍可保留实现，但当前主路径默认根页面是 `HomeRingHost`。

## D003: `InputIntentRouter` 只做输入语义转换

它负责把 HAL 层触摸、表冠、按键、调试输入转换成应用层 `InputCommand`。它不直接改页面、不直接改状态机状态、不触碰硬件以外的业务规则。

## D004: Service 层负责平台样本到应用模型

`BatteryPowerService`、`NotificationService`、`StepsActivityService` 这类模块负责解释、过滤、聚合平台样本，并通过 `DataCenter` 发布应用模型。

## D005: `DataCenter + EventBus` 是 v0 共享模型入口

当前仍使用轻量同步事件分发。新增模型必须说明领域归属，避免把 `DataCenter` 继续膨胀成无边界全局对象。

## D006: `AppStateMachine` 要退回 Coordinator

`AppStateMachine` 当前仍承担很多领域细节。目标方向是让它接收事件、调用子控制器、汇总 Action、统一调用 `PageManager` 并处理跨域冲突。

## D007: 子 Controller 不直接操作 UI

`PowerController`、`HomeRingController`、`ShellNavigationController`、`NotificationFlowController` 等未来子控制器不得直接调用 `PageManager`、不得创建 LVGL 对象、不得访问页面内部。

## D008: Action/Event 必须适合嵌入式环境

Controller 返回的 Action 应固定大小、可按值传递、无动态资源。不在 Action/Event 中使用 `std::string`、`std::vector`、heap allocation 或复杂对象所有权。

## D009: `PageManager` 只负责页面显示与栈

`PageManager` 管理 `set_root`、`push`、`pop`、临时页面和页面加载动画，不负责解释业务原因、系统状态或输入语义。

## D010: 文档入口必须短

默认新会话只读 `AGENTS.md`、`docs/document_map.md` 和 `docs/00_current/` 下的当前入口文档。`decision_log.md`、`page_reachability_audit.md`、旧 `project_charter.md` 都不再默认阅读。

## D011: 每轮必须 Scope Lock

每一轮实施前必须明确 `Allowed files`、`Forbidden files` 和 `Forbidden changes`。结束后检查 `git status`，实际修改文件必须落在允许范围内；若出现越权 diff，本轮判定失败，不在违规 diff 上继续修补。

## D012: UI 巨石拆分前必须先建立 LVGL 生命周期契约

拆 `SettingsPages.cpp` 或 `ShellPages.cpp` 前，必须先明确 LVGL root object、timer、EventBus subscription、临时壳层和页面状态的生命周期责任。不得在未处理 `LV_EVENT_DELETE` 和双重释放风险前复制页面拆分模式。
