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

## B. FT6336U 基础触摸事件实验

日期：2026-05-22

状态：通过。

本轮边界：

- 只验证 FT6336U 经官方 `LV_Helper` 接入 LVGL 后的基础指针事件。
- 只观察按下、拖动、释放、坐标方向和边缘可靠性。
- 不实现复杂手势，不接入 Magic Watch 页面状态机，不扩展到 BMA423、AXP2101 或 screen off / wake。

已完成：

- 在 `prototypes/twatch_s3_plus_bringup/src/main.cpp` 中加入触摸状态显示。
- 页面显示最近触摸状态：`Touch idle`、`Touch press x,y` 或 `Touch release x,y`。
- 页面显示触摸事件计数，用于确认按下和释放边沿被捕获。
- 串口加入 `[bringup-touch] press/move/release` 日志，move 日志做了节流，避免拖动时刷屏。
- 编译通过：`pio run -e twatch-s3 -j 1`。
- 上传通过：`COM9`。
- 上传后启动日志和心跳正常。

关键串口观察：

- `probe=0x0000007e`
- `rotation=2`
- `brightness=160`
- `free_heap` 约 `354036`
- `psram` 约 `8107935`

待人工观察：

- 更细的边缘可靠性统计，例如多次点击四边和四角的丢点率。
- 是否需要增加原始 FT6336U 坐标记录，用于和 LVGL 旋转后坐标对照。

实物观察：

- 按下和释放均能被页面反映。
- `Events` 计数会随按下、释放边沿增加。
- `Touch release` 会实时更新最后一次触摸坐标。
- 屏幕左上角对应坐标约 `0,0`。
- 屏幕右下角对应坐标约 `240,240`。
- 当前 `rotation=2` 下，坐标方向与视觉方向一致。

结论：

- FT6336U 通过官方 `LV_Helper` 接入 LVGL pointer 的基础路径可用。
- 本轮只证明基础触摸点、边沿事件和坐标方向可用，不代表复杂手势、滚动物理、触摸唤醒或最终触摸器件选型已经完成。

## C. AXP2101 基础供电状态实验

日期：2026-05-22

状态：通过。

本轮边界：

- 只读取 AXP2101 的基础供电状态和电压值。
- 只观察 USB 接入、充电、放电、电池电压、VBUS 电压、系统电压和电量百分比。
- 不修改供电轨，不调整充电策略，不处理 PMU 中断、PEK 按键、关机策略或 sleep/wake。

已完成：

- 在 `prototypes/twatch_s3_plus_bringup/src/main.cpp` 中加入 `PmuSnapshot` 基础读数。
- 页面显示 USB、充电、放电、充电阶段、电池电压、电量百分比和系统电压。
- 串口每两秒输出 `[bringup-pmu]` 状态日志。
- 编译通过：`pio run -e twatch-s3 -j 1`。
- 上传通过：`COM9`。
- 上传后启动日志、心跳和 PMU 状态日志正常。

关键串口观察：

- `usb=Y`
- `charging=Y`
- `discharging=N`
- `chg=cc`
- `batt` 约 `3538` 到 `3541mV`
- `vbus` 约 `4757` 到 `4759mV`
- `sys` 约 `4745` 到 `4753mV`
- `percent=4`
- 心跳观察：`free_heap` 约 `353740`，`psram` 约 `8107935`

实物观察：

- USB 接入时，显示 `usb=Y`、`charging=Y`、`discharging=N`。
- 拔掉 USB 后，显示 `usb=N`、`charging=N`、`discharging=Y`。
- 重新插入 USB 后，状态恢复为 USB 接入和充电路径。

结论：

- AXP2101 基础状态读取路径可用，USB 插入、拔出、充电和放电状态变化能被页面和串口观察到。
- 本轮只证明基础读数可用，不代表 PMU 中断、PEK 按键、充电策略、低功耗策略或最终 PMIC 选型已经完成。

## D. BMA423 基础加速度与姿态实验

日期：2026-05-22

状态：通过。

本轮边界：

- 只读取 BMA423 基础 X/Y/Z 加速度和官方库 `direction()` 结果。
- 只观察平放、竖起、翻转时数值是否明显变化。
- 不启用计步，不接入 Steps/Pedometer，不把计步中断作为验收项。
- 不处理 BMA 中断、RaiseToWake 策略落地、screen off / wake 或 sleep。

已完成：

- 在 `prototypes/twatch_s3_plus_bringup/src/main.cpp` 中加入 `BmaSnapshot` 基础读数。
- 调用 `watch.configAccelerometer()` 和 `watch.enableAccelerometer()`。
- 页面显示 BMA423 X/Y/Z 加速度和方向判断。
- 串口每 0.5 秒输出 `[bringup-bma]` 状态日志。
- 编译通过：`pio run -e twatch-s3 -j 1`。
- 上传通过：`COM9`。
- 上传后启动日志、心跳、PMU 状态日志和 BMA 状态日志正常。

关键串口观察：

- `probe=0x0000007e`
- `rotation=2`
- `free_heap` 约 `353452`
- `psram` 约 `8107935`
- BMA 静置示例：`x=30`、`y=-19`、`z=-519`、`dir=bottom`

实物观察：

- 平放、竖起、翻转时，X/Y/Z 主导轴和 `dir` 均有明显变化。
- 平放时：`z=-520`、`dir=bottom`。
- 倒扣置桌面时：`z=520`、`dir=top`。
- 竖立时：`x=520`、`dir=top_left`。
- 屏幕左边缘贴地时：`y=500`、`dir=top_right`。

结论：

- BMA423 基础加速度读取路径可用，当前静置读数和姿态变化都能被页面和串口观察到。
- 主导轴和符号随姿态变化明显，后续可作为 RaiseToWake 替代路径的输入线索继续评估。
- 本轮仍未证明计步可用；这与已知“BMA423 计步中断疑似硬件问题”的边界保持一致。

## E. Screen Off / Wake 第一层观察

日期：2026-05-22

状态：通过。

本轮边界：

- 只验证显示 sleep / 背光关闭与恢复。
- 只观察 PMU 短按侧键切换 screen off/on，以及 screen off 后触摸恢复显示。
- 不进入 light sleep 或 deep sleep。
- 不评估真实低功耗电流，不验证 RTC/PMU/BMA/Touch 作为 deep sleep 唤醒源的完整路径。

已完成：

- 在 `prototypes/twatch_s3_plus_bringup/src/main.cpp` 中加入 `g_screen_on` 和 `g_screen_toggles`。
- 使用 `watch.setBrightness(0)` 关闭显示和背光。
- 使用 `watch.setBrightness(160)` 恢复显示和背光。
- 接入 `watch.attachPMU()`，PMU 短按侧键触发 screen off/on 切换。
- screen off 状态下检测到触摸时恢复显示。
- 串口输出 `[bringup-screen]`，记录状态、原因、次数和亮度。
- 编译通过：`pio run -e twatch-s3 -j 1`。
- 上传通过：`COM9`。
- 上传后启动日志、心跳、PMU 状态日志和 BMA 状态日志正常。

关键串口观察：

- `probe=0x0000007e`
- `rotation=2`
- `brightness=160`
- `free_heap` 约 `352196`
- `psram` 约 `8107935`

实物观察：

- 短按侧键后，屏幕稳定变黑。
- 黑屏后再次短按侧键，屏幕稳定恢复。
- 黑屏后触摸屏幕，屏幕稳定恢复。

结论：

- screen off/on 第一层代码路径可用，运行态显示关闭和恢复能通过侧键与触摸完成。
- 本轮结果不能外推到 light sleep/deep sleep 低功耗唤醒能力。

## F. Deep Sleep Timer 第一层观察

日期：2026-05-22

状态：通过，有启动耗时边界。

本轮边界：

- 只验证 ESP32-S3 timer deep sleep 的最小进入与自动唤醒路径。
- 只观察长按侧键触发 15 秒 deep sleep、屏幕熄灭、定时唤醒后重启、LVGL 页面重新显示和串口 wake cause。
- 不验证 PMU 侧键 deep sleep 唤醒。
- 不验证触摸、BMA423、RTC 作为 deep sleep 唤醒源。
- 不评估真实低功耗电流。

已完成：

- 在 `prototypes/twatch_s3_plus_bringup/src/main.cpp` 中加入启动 wake cause 读取和 RTC boot 计数。
- 页面状态区显示 `Wake ...` 与 `Boot ...`。
- 保留短按侧键的 screen off/on 行为。
- 长按侧键触发 3 秒倒计时。
- 串口加入 `[bringup-sleep]` 日志，用于记录进入 deep sleep 前和唤醒后的状态。
- 编译通过：`pio run -e twatch-s3 -j 1`。
- 上传通过：`COM9`。
- 第一次实物观察未通过：长按后倒计时正常，结束后停在 `Deep sleep now` 页面，没有出现黑屏；按下复位键后屏幕黑屏，过一段时间后屏幕亮起。
- 修正方向：进入 sleep 前显式调用 `setBrightness(0)`，去掉 `Serial.flush()`，改用 `esp_sleep_enable_timer_wakeup()` 与 `esp_deep_sleep_start()` 直接进入 ESP-IDF deep sleep。
- 复位观察新增线索：无论处于什么状态，按下复位后都需要约 15 秒才亮屏，屏幕刚亮时 `uptime` 已约 15 秒。
- 解释假设：启动黑屏等待更像 `watch.begin()` 期间的参考库初始化耗时，而不是 LVGL 页面本身的问题；当前 bring-up 不验证 GPS，因此默认跳过 I2C 扫描和 GPS 探测以缩短复位到亮屏时间。
- 影响：后续 `probe` 不再把 GPS 位作为当前闭环观察项；这不代表 GPS 硬件不可用，只表示本闭环主动不测 GPS。
- 修正后编译通过：`pio run -e twatch-s3 -j 1`。
- 修正后上传通过：`COM9`。

实物观察：

- 长按侧键后，屏幕显示 deep sleep 倒计时。
- 进入 deep sleep 后，屏幕稳定变黑。
- 约 23 秒后，设备自动亮回 LVGL 页面。
- 按复位键后，页面约 7 秒后亮起，已明显短于跳过扫描/GPS 前的约 15 秒。

结论：

- timer deep sleep 的最小进入、黑屏和自动重启式恢复路径可用。
- 约 23 秒黑屏到亮回可解释为 15 秒 timer deep sleep 加约 7 秒启动到首屏耗时。
- 当前首屏启动仍有约 7 秒延迟，后续可单独做“启动耗时收敛”闭环。
- 本闭环不代表运行态恢复，也不代表 PMU/触摸/BMA 作为真实低功耗唤醒源已经可用。

## G. 启动耗时收敛观察

日期：2026-05-22

状态：通过。

本轮边界：

- 只收敛 T-Watch S3 Plus bring-up 工程的复位到 LVGL 首屏耗时。
- 只跳过当前闭环不需要的参考库初始化项。
- 不改变最终产品硬件选型判断。
- 不验证 GPS、Radio、DRV2605、FFat 文件系统或音频路径。

已完成：

- 在 `prototypes/twatch_s3_plus_bringup/platformio.ini` 中加入 bring-up 快速启动宏。
- 默认跳过 I2C 扫描、GPS 探测、FFat 挂载、DRV2605 初始化、Radio SPI Bus 初始化和 PMU 大段供电轨日志。
- 移除 `watch.begin()` 尾部固定 1 秒等待。
- 将 `setup()` 开头等待从 300ms 降为 50ms。
- 加入 `[bringup-boot] watch.begin_ms=...` 和 `[bringup-boot] first_screen_ms=...` 日志，用于后续继续定位。
- 编译通过：`pio run -e twatch-s3 -j 1`。
- 上传通过：`COM9`。

实物观察：

- 修正前，复位到亮屏约 7 秒。
- 快速启动修正后，复位到亮屏约 2 到 3 秒。

结论：

- 当前 bring-up 工程的复位到首屏耗时已从约 7 秒收敛到约 2 到 3 秒。
- 2 到 3 秒对当前硬件前哨验证已可接受，后续若要继续优化应单独拆为更细的启动阶段剖析闭环。
- 本轮跳过 GPS、Radio、DRV2605 和 FFat 是 bring-up 边界收敛，不代表这些外设在最终平台或后续实验中不需要。
