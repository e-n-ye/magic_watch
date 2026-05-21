# T-Watch S3 Plus 参考板核验运行记录

日期：2026-05-21

## 当前状态

第一小闭环“最小显示 + 日志”已通过。

本记录只说明 LILYGO T-Watch S3 Plus 作为参考验证板完成了最小 bring-up，不代表最终主控、屏幕、触摸、IMU 或 PMIC 选型。

## A. 最小系统与显示实验

状态：通过。

已完成：

- 新增独立最小工程：`prototypes/twatch_s3_plus_bringup`。
- 使用 PlatformIO / Arduino 路线，基于 T-Watch S3 官方库裁剪出本地 `LilyGoTWatchS3` bring-up 库。
- 编译通过：`pio run -e twatch-s3 -j 1`。
- 上传通过：`COM9`，芯片识别为 ESP32-S3 rev v0.2，MAC `98:a3:16:f5:65:10`。
- 串口日志通过：可见启动日志和每秒心跳。
- 屏幕通过：240 x 240 LCD 显示最小 LVGL 页面。
- 目视确认：页面方向正确、颜色可读，状态区修正后无文字挤压。

关键串口观察：

- `probe=0x0000007e`
- `rotation=2`
- `brightness=160`
- `free_heap` 约 `354320`
- `psram` 约 `8107935`

解释：

- `probe=0x0000007e` 说明官方库探测到触摸、DRV、PMU、RTC、BMA 和 GPS 路径。
- 本闭环只把这些作为启动探测日志，不把触摸、BMA423、AXP2101 或 GPS 视为已专项验证。

## 本轮边界

本轮未验证：

- FT6336U 触摸点击、拖动、坐标方向和边缘可靠性。
- BMA423 基础加速度、姿态变化或非计步类中断。
- AXP2101 电池、充电、外部供电状态。
- screen off / wake、light sleep、deep sleep 或 AOD。
- Magic Watch 模拟器页面移植。

## 已知问题与修正

- 初版状态卡片在 240 x 240 屏幕上出现文字换行后和 uptime 重叠。
- 已将状态区改为更紧凑的三行布局，并重新编译上传。
- 用户实物照片确认当前已无挤压情况。

## 下一轮建议

下一小闭环建议进入 FT6336U 基础触摸事件验证：

- 读取按下、移动、释放或等价事件。
- 记录坐标范围和旋转映射。
- 初步观察边缘触摸可靠性。
- 不引入复杂手势，不接入 Magic Watch 页面状态机。
