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
- 当前已存在最小 `ScreenManager`，保留 LCD 直绘 Debug Screen 骨架；LVGL 已作为当前可见 UI bring-up 链路接入。
- LVGL 已按最小 label bring-up 目标收窄配置并登记到 MDK 工程；当前已有项目自有 display flush port，flush 复用 `watch_lcd_draw_rgb565()`；`lv_timer_handler()` 由 FreeRTOS defaultTask 通过 `watch_bringup_task()` 单任务驱动；LVGL encoder indev 只消费编码器 intent；当前显示一个最小 LVGL debug label，用于验证输入计数、最后一次输入语义和当前 flush 性能基线。
- LVGL debug label 中的 `pulse` 是无输入时的可控刷新源；`calls/s` 是 flush 调用次数，不等于 FPS；`full/s` 是按全屏像素量折算出的等效全屏刷新率；`pixels/s` 是实际刷出的像素吞吐；`last ms` 是最近一次 flush 耗时；`lvgl/s` 是 `lv_timer_handler()` 调用频率。
- LVGL debug screen 顶部显示红、绿、蓝、白、黑 5 个色块，中部显示 `font 10`、`font 14`、`font 20` 三组英文样例，用于在接入 XML UI 前验证 RGB565 字节序和字体显示质量。
- `font 20` 下方的细条是 FPS 负载探针：黄色 marker 约每 33ms 移动一次，并主动刷新大于 5000 像素的区域，用于形成稳定可观察的刷新负载。

## 下一步方向

下一轮不急着移植模拟器 UI。建议先用 LVGL debug label 建立阻塞 SPI flush 性能基线，再验证 RGB565 / 字体显示质量，之后由 CubeMX 配置 SPI DMA，最后在 `user/` 层接入异步 DMA flush。

## 后续优化记录

- 当前 LCD flush 走逐行 RGB565 转换和阻塞 SPI 写入，适合作为安全 bring-up 基线；debug label 已显示 `calls/s`、`full/s`、`pixels/s`、`last ms` 和 `lvgl/s`，后续 SPI DMA / 大块刷屏优化必须用这些指标做前后对比。
- 右下角 `fps / ms` 徽标来自 LVGL display `monitor_cb`：`fps` 是按 1 秒墙钟窗口统计的有效刷新周期数，`ms` 是最近一次 LVGL 刷新周期耗时；`calls/s`、`full/s`、`pixels/s`、`last ms` 仍是底层 flush 诊断指标，不能互相替代。
- LVGL 自带 perf monitor 已关闭：它在当前负载下会按渲染耗时折算理论 FPS，并可能显示 `100 FPS` 这类假高值，不适合作为 DMA 前后主对比指标。

## LVGL 颜色和字体验收

- 真机顶部色块应依次显示红、绿、蓝、白、黑；若红蓝互换或白色明显偏色，优先检查 RGB565 字节序和颜色转换。
- `font 10`、`font 14`、`font 20` 三行英文应可读，且不应出现裁切、乱码或缺字；若颜色正常但文字观感差，优先调整字号或字体策略。
- 当前 LVGL 端关闭 `LV_COLOR_16_SWAP`，因为 `watch_lcd_draw_rgb565()` 已经按 RGB565 高字节优先发送到 LCD。
- 黄色 FPS 负载 marker 应持续移动；右下角 `fps / ms` 徽标应稳定显示真实刷新范围，不应频繁空窗跳到 `100`。
- 本验收不接 DMA，不迁移 PC XML UI，不代表后续健康四卡真机显示已经通过。
