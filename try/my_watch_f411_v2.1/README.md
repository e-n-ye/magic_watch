# my_watch_f411_v2.1 bring-up baseline

这是 STM32F411 手表项目的新基线工程。当前目标不是一次性移植完整 Magic Watch，而是先建立一个自己可掌控、可逐步生长的 FreeRTOS + C-first 硬件骨架。

## 当前硬件验收

- 屏幕背光正常，启动后进入最小 Home 屏幕。
- 编码器顺时针事件驱动最小屏幕状态正向切换。
- 编码器逆时针事件驱动最小屏幕状态反向切换。
- 编码器短按事件进入 Debug 屏幕。
- 编码器长按事件回到 Home 屏幕。
- BACK / WAKE 在早期 bring-up 中已验证可读；当前不映射到应用层主交互。
- 顶部黑条长度随应用事件次数变化，用于确认 `InputIntent -> EventQueue-lite -> ScreenManager` 链路被消费。

## 当前交互决策

- 应用层主交互从 `InputIntent` 进入，当前只映射编码器旋转、编码器短按和编码器长按。
- BACK 和 WAKE 当前只作为 bring-up 验证输入保留，不作为后续主要产品交互。
- WAKE 按键需要接入电池后才有正常上拉，且物理位置不适合作为高频交互入口。

## 当前代码边界

- 手写代码放在 `user/` 下，按 `config`、`board`、`services`、`app` 分层。
- CubeMX 生成代码只在 USER CODE 区插入最小调用接口。
- MDK 工程文件已收窄到当前 bring-up 所需的最小用户代码和必要外设。
- 应用层事件入口使用固定数组 `EventQueue-lite`，当前只承载输入语义事件，不使用 heap 或复杂订阅机制。
- 当前已存在最小 `ScreenManager`，只使用 LCD 直绘做 Debug Screen 验证；下一批才考虑接入 LVGL。
- LVGL 已按最小 label bring-up 目标收窄配置并登记到 MDK 工程；当前已有项目自有 display flush port，flush 复用 `watch_lcd_draw_rgb565()`；`lv_timer_handler()` 由 FreeRTOS defaultTask 通过 `watch_bringup_task()` 单任务驱动；LVGL encoder indev 只消费编码器 intent，尚未创建 LVGL screen。

## 下一步方向

下一轮不急着移植模拟器 UI。建议先用最小 LVGL bring-up 验证 tick、flush、handler 和编码器输入设备。

## 后续优化记录

- 当前 LCD flush 走逐行 RGB565 转换和阻塞 SPI 写入，适合作为安全 bring-up 基线；后续需要单独规划 SPI DMA / 大块刷屏优化，不能混入当前最小 LVGL 接入卡片。
