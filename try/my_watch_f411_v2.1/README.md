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
- LVGL 已按最小 label bring-up 目标收窄配置并登记到 MDK 工程；当前已有项目自有 display flush port，flush 会先把 `lv_color_t` 裁剪并转换成 RGB565 高字节优先 byte stream，再优先走 `watch_lcd_draw_rgb565_bytes()` 的 SPI DMA fallback 接口；DMA 真正完成后不在中断里直接调用 LVGL，而是由 FreeRTOS `defaultTask` 中的 LVGL `wait_cb` / `watch_bringup_task()` 路径消费完成状态并调用 `lv_disp_flush_ready()`；若 DMA error callback 触发，则由 `defaultTask` 用同一份 byte stream 做一次阻塞补发后再结束 flush；LVGL encoder indev 只消费编码器 intent；当前显示一个最小 LVGL debug label，用于验证输入计数、最后一次输入语义和当前 flush 性能基线。
- LVGL debug label 中的 `pulse` 是无输入时的可控刷新源；`calls/s` 是 flush 调用次数，不等于 FPS；`full/s` 是按全屏像素量折算出的等效全屏刷新率；`pixels/s` 是实际刷出的像素吞吐；`last ms` 是最近一次 flush 耗时；`lvgl/s` 是 `lv_timer_handler()` 调用频率。
- LVGL debug screen 顶部显示红、绿、蓝、白、黑 5 个色块，中部显示 `font 10`、`font 14`、`font 20` 三组英文样例，用于在接入 XML UI 前验证 RGB565 字节序和字体显示质量。
- `font 20` 下方的细条是 FPS 负载探针：黄色 marker 约每 33ms 移动一次，并主动刷新大于 5000 像素的区域，用于形成稳定可观察的刷新负载。

## 下一步方向

下一轮不急着移植模拟器 UI。建议先在真机上继续观察当前异步 DMA flush 的稳定性和性能，再记录 DMA 前后对比结果，然后才考虑把更复杂的 UI 迁移进来。

## 后续优化记录

- 当前 LCD flush 已接到 `watch_lcd` 的 SPI DMA fallback 接口：LVGL 侧会先处理部分越界区域裁剪，并把像素转换成 LCD 需要的 RGB565 高字节优先 byte stream；大块 byte stream 优先尝试 DMA，小块或 DMA 不可用时仍走阻塞路径；DMA 完成只更新底层 busy 状态，`lv_disp_flush_ready()` 统一留给 `defaultTask` 消费，避免在中断上下文直接调用 LVGL；若 DMA 中途出错，则由 `defaultTask` 复用同一份 byte stream 做阻塞补发，保证本次 flush 最终仍有完整像素落屏。
- 右下角 `fps / ms` 徽标来自 LVGL display `monitor_cb`：`fps` 是按 1 秒墙钟窗口统计的有效刷新周期数，`ms` 是最近一次 LVGL 刷新周期耗时；`calls/s`、`full/s`、`pixels/s`、`last ms` 仍是底层 flush 诊断指标，不能互相替代。
- LVGL 自带 perf monitor 已关闭：它在当前负载下会按渲染耗时折算理论 FPS，并可能显示 `100 FPS` 这类假高值，不适合作为 DMA 前后主对比指标。

## V0.0 DMA 收口手测步骤

- 本轮目标只记录 DMA 前后基线，不继续改 DMA 代码。
- DMA 前后对比必须尽量保持同一口径：同一块板、同一块屏、同一份工程、同一 debug screen、同一观察时长、同一手法。
- `last ms` 当前口径是“从 flush 发起到 `defaultTask` 完成通知”的端到端耗时，不等于纯 SPI DMA 线上传输时间。
- 建议把“DMA 前”固定为 `F411-LVGL-DMA-1` 已验收、但 `F411-LVGL-DMA-2` 尚未接入时记录过的阻塞路径数据；如果当时没有留下数字，就如实记为“未记录”，不要补猜。

建议按下面 3 组场景手测并记录：

1. 静态观察 60 秒
   - 上电进入当前 LVGL debug screen，不操作旋钮。
   - 观察 `pulse` 是否持续递增，记录一组稳定读数：`calls/s`、`full/s`、`pixels/s`、`last ms`、`lvgl/s`、右下角 `fps/ms`。
   - 观察是否出现花屏、撕裂、黑块、停更或整机卡死。
2. 输入压力 120 秒
   - 连续快速旋转旋钮，并间歇按下，尽量覆盖快转、反向、停顿后再转三种节奏。
   - 在压力过程中记录一组读数，并主观判断输入响应是否比 DMA 前明显变差。
   - 重点观察是否出现明显输入迟滞、界面停止响应、花屏、撕裂或卡死。
3. 长稳观察 10 分钟
   - 回到无操作状态，持续点亮观察至少 10 分钟。
   - 期间只记录稳定性，不要求持续抄表；若中途出现一次异常，也要记下大致时间点和现象。

建议按下面模板回填：

```text
DMA 基线记录

测试日期：
测试固件：
硬件版本：

一、DMA 前基线
- 来源版本：
- 静态 60 秒：calls/s= ，full/s= ，pixels/s= ，last ms= ，lvgl/s= ，fps/ms=
- 输入压力 120 秒：calls/s= ，full/s= ，pixels/s= ，last ms= ，lvgl/s= ，fps/ms=
- 稳定性：花屏=有/无/未验证，撕裂=有/无/未验证，卡死=有/无/未验证
- 输入响应：正常/略差/明显变差/未验证
- 备注：

二、DMA 后基线
- 来源版本：当前 `F411-LVGL-DMA-2` 验收后固件
- 静态 60 秒：calls/s= ，full/s= ，pixels/s= ，last ms= ，lvgl/s= ，fps/ms=
- 输入压力 120 秒：calls/s= ，full/s= ，pixels/s= ，last ms= ，lvgl/s= ，fps/ms=
- 长稳 10 分钟：正常/异常/未验证
- 稳定性：花屏=有/无/未验证，撕裂=有/无/未验证，卡死=有/无/未验证
- 输入响应：正常/略差/明显变差/未验证
- 备注：

三、结论
- DMA 前后数字变化：
- 体感变化：
- 未验证项：
```

当前已回填结果（2026-06-08，DMA 后固件）：

- DMA 前阻塞路径补测方法：将 `LCD_DMA_MIN_BYTES` 临时改为 `128000000U` 后重新烧录。由于当前发送逻辑在 `byte_count < LCD_DMA_MIN_BYTES` 时直接走阻塞路径，这次补测可视为当前场景下的“强制不走 DMA”基线。
- DMA 前静态 60 秒：`calls/s 60`、`full/s 2.5`、`pixels/s 174060`、`last ms 0`、`refr ms 7`、`lvgl/s 140`、右下角 `26fps 7ms`
- DMA 前输入压力 120 秒：`calls/s 53`、`full/s 2.6`、`pixels/s 177370`、`last ms 2`、`refr ms 74`、`lvgl/s 55`、右下角 `3fps 74ms`
- DMA 前长稳 10 分钟：无花屏、撕裂、卡死、停更
- 静态 60 秒：`calls/s 63`、`full/s 2.7`、`pixels/s 183154`、`last ms 0`、`refr ms 6`、`lvgl/s 144`、右下角 `25fps 6ms`
- 输入压力 120 秒：`calls/s 31`、`full/s 1.6`、`pixels/s 112209`、`last ms 2`、`refr ms 65`、`lvgl/s 3`、右下角 `3fps 65ms`
- 长稳 10 分钟：无花屏、撕裂、卡死、停更
- 主观体感：旋钮/输入响应和强制阻塞路径相比差不多，未感知到明显变差

说明：

- 本工程当前右下角读数中的 `ms` 是 `refr ms`，表示最近一次 LVGL 刷新周期耗时；底部 `last ms` 仍是 flush 链路最近一次端到端耗时。
- 当前这组 DMA 前数据来自“人为抬高 `LCD_DMA_MIN_BYTES` 阈值以强制阻塞路径”的补测，不是历史原始留档；但对当前 debug screen 场景来说，它比补猜历史数字更可信。

## LVGL 颜色和字体验收

- 真机顶部色块应依次显示红、绿、蓝、白、黑；若红蓝互换或白色明显偏色，优先检查 RGB565 字节序和颜色转换。
- `font 10`、`font 14`、`font 20` 三行英文应可读，且不应出现裁切、乱码或缺字；若颜色正常但文字观感差，优先调整字号或字体策略。
- 当前 LVGL 端关闭 `LV_COLOR_16_SWAP`，因为 `watch_lcd_draw_rgb565()` 已经按 RGB565 高字节优先发送到 LCD。
- 黄色 FPS 负载 marker 应持续移动；右下角 `fps / ms` 徽标应稳定显示真实刷新范围，不应频繁空窗跳到 `100`。
- 本验收不接 DMA，不迁移 PC XML UI，不代表后续健康四卡真机显示已经通过。
