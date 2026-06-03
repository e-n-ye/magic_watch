# v0.6 Steps / Activity service 小闭环

日期：2026-05-23

## 目标

本轮只做一条最小真实数据链：

- 模拟器产生步数样本
- `StepsActivityService` 解释样本
- `DataCenter` 发布 `StepsModel`
- 长续航表盘消费 `StepsModel`

本轮不做：

- Steps 详情页改造
- 历史趋势
- 目标设置
- 卡路里 / 距离推导
- 真实传感器接入
- 跨天清零策略

## 为什么这样拆

如果这轮同时去碰 Steps app、主页快捷卡、健康设置联动和真机传感器，范围会立刻跨过多个页面层级、多个模型和多个输入入口，回归面会明显变大。

当前最稳的推进方式，是先把“步数不再是页面本地假数据”这件事做成一个独立闭环。这样我们练到的是系统边界，而不是一次把业务铺满。

## 本轮边界

### 新增

- `hal::ActivitySample`
- `StepsModel`
- `StepsActivityService`

### 更新

- `Application`：接收 `EventKind::ActivityUpdated`
- `DataCenter`：保存并发布 `StepsModel`
- `SimulatorDevice`：增加步数热键样本注入
- `LongBatteryWatchfacePage`：从订阅模型改掉原来的步数占位 `0`

## 当前语义

### 平台层

模拟器当前不直接让页面改数字，而是：

- 按一次 `S`
- `SimulatorDevice` 把今日步数增加 `100`
- 发出一个新的 `hal::ActivitySample`

这代表的是“平台样本变化”，不是“页面调试按钮”。

### 服务层

`StepsActivityService` 当前职责很窄：

- 接收 `ActivitySample`
- 做最小规范化
- 发布 `StepsModel`

它现在还不负责：

- 步数算法
- 跨天归零
- 多源合并
- 卡路里换算

### UI 层

当前这条真实链路已经有两个消费者：

- 长续航表盘
- `StepsAppPage`

这意味着：

- 长续航表盘里的步数不再是写死占位
- Steps 页里的关键步数数字也开始认共享模型
- 但 Steps 页中的卡路里、活跃分钟、说明页内容仍保留 mock

这是刻意保留的边界，不是遗漏。

## 热键约定

- `S`：模拟增加 100 步

当前不提供减少、清零或自动增长热键，避免把本轮又做大。

## 验收标准

- 项目可构建
- 启动模拟器后，长续航表盘默认显示步数模型值
- 在正常模式或长续航模式下按 `S`，再次进入或停留在长续航表盘时，步数会按 `100` 递增
- 页面本身不直接持有“步数业务状态”
- `Application` 不直接解释步数 UI

## 当前进展

2026-05-23 第二小轮补充：

- `StepsAppPage` 已开始消费共享 `StepsModel`
- 当前已接入的字段仅限：
  - 概览中的步数环
  - 概览中的步数数字
  - 步数卡中的大数字
- 目标值 `6000`、卡路里、活跃分钟与说明页仍保留 mock

## 后续建议

如果这条链继续稳定，下一轮优先考虑两种方向之一：

1. 明确 `StepsModel` 与未来真机 BMA / Activity 数据的映射关系
2. 再决定是否让主页快捷卡或更多 Steps 细节页面消费同一个模型

在这之前，不建议继续扩展步数页面视觉细节或联动更多健康页面。
# 2026-05-23 补充：主页快捷卡接入共享 StepsModel

- 主页环天气/睡眠/步数快捷面（`HomeRingHostPage` 的 `surface[4]`）里的步数小卡已经开始消费共享 `StepsModel`
- 当前共享 `StepsModel` 的消费者共有三个：
  - `LongBatteryWatchfacePage`
  - `StepsAppPage`
  - `HomeRingHostPage` 的 `surface[4]`
- 本轮仍然只接入“步数数字”本身，不扩展睡眠卡、天气卡、卡路里或活跃分钟的数据模型

补充说明：

- 文档最初记录这一轮时，代码里还存在独立 `WeatherShortcutPage` 资产。
- 该独立页已在后续清理中删除；当前有效消费者应以上面的 `HomeRingHostPage` 内嵌快捷面为准。
