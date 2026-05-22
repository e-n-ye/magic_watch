# T-Watch S3 Plus 最小 Bring-Up

本目录是 Magic Watch 硬件前哨阶段的 T-Watch S3 Plus 参考板 bring-up 工程。

第一小闭环只验证：

- 可编译、可烧录、可串口输出日志。
- 240 x 240 屏幕可点亮。
- 最小 LVGL 页面可稳定刷新。

第二小闭环只验证：

- FT6336U 基础触摸按下、拖动、释放是否能被 LVGL 输入设备观测。
- 屏幕上显示最近触摸坐标和事件计数。
- 串口输出 `[bringup-touch] press/move/release`，用于记录坐标方向和边缘可靠性。

第三小闭环只验证：

- AXP2101 基础供电状态是否能被读取。
- 屏幕上显示 USB、充电、放电、电池电压、电量和系统电压摘要。
- 串口输出 `[bringup-pmu]`，用于记录 USB 插入、充电状态和电压变化。

本目录不代表最终硬件选型，不接入 Magic Watch 模拟器页面，不做真实计步、触摸完整手势、AXP2101 深度电源策略或 screen off / wake。

## 依赖来源

- 官方参考库：`D:\MY_Desk\watch\exam\TTGO_TWatch_Library-t-watch-s3`
- 本地裁剪库：`lib\LilyGoTWatchS3`
- 工程入口：`src/main.cpp`
- 本地 TFT_eSPI 配置：`extras/Setup212_LilyGo_T_Watch_S3.h`
- 本地板定义：`board/LilyGoWatch-S3/LilyGoWatch-S3.json`

`lib\LilyGoTWatchS3` 只复制官方库中的 `LilyGoLib`、`LV_Helper` 和必要头文件，避免把官方 `src\main.cpp` 或大型 demo 编进本闭环。旧工程 `TWatch_MY`、`en_t_watch` 仅作为参考，不被本闭环复用。

## 常用命令

在本目录执行：

```powershell
pio run -e twatch-s3
pio run -e twatch-s3 -t upload
pio device monitor -b 115200
```

如果上传失败，先确认设备管理器里出现 T-Watch 的 COM 口，并确认手表已经开机。官方 README 提到，必要时可按住手表侧键约一秒让端口出现。

## 屏幕验收记录

运行后应观察到：

- 串口出现 `[bringup] Magic Watch T-Watch S3 Plus minimal bring-up`。
- 串口每秒输出一次 `[bringup] uptime=...` 心跳。
- 屏幕显示 `Magic Watch`、`T-Watch S3 Plus bring-up` 和 `uptime ... s`。
- 页面显示 `Touch idle/press/release`、最近触摸坐标和事件计数。
- 页面显示 AXP2101 基础供电摘要。
- 页面底部显示 `touch + AXP2101 monitor`。
- 点按或拖动屏幕后，串口应出现 `[bringup-touch] press`、节流后的 `[bringup-touch] move` 和 `[bringup-touch] release`。
- 串口每两秒输出一次 `[bringup-pmu] usb=... charging=... batt=... vbus=... sys=... percent=...`。

## 2026-05-21 首次验证

- 编译：通过，命令为 `pio run -e twatch-s3 -j 1`。
- 固件大小：RAM 约 7.0%，Flash 约 19.2%。
- 上传：通过，端口 `COM9`，芯片识别为 ESP32-S3 rev v0.2，MAC `98:a3:16:f5:65:10`。
- 串口：通过，可见 `[bringup]` 启动日志和每秒心跳。
- 观测到的心跳示例：`uptime=14s probe=0x0000007e rotation=2 brightness=160 free_heap=354316 psram=8107935`。
- `probe=0x0000007e` 表示官方库探测到触摸、DRV、PMU、RTC、BMA 和 GPS 路径；本闭环只把它作为启动日志，不把这些外设视为已完成专项验证。
- 实物目视确认：屏幕显示 `Magic Watch` 页面，方向正确，颜色可读，状态区修正后无文字挤压。
- 已知观察：当前页面位于可视 LCD 区域内；未在本闭环评估触摸、BMA423、AXP2101 或 screen off / wake。

## 2026-05-22 触摸观测闭环验证

- 编译：通过，命令为 `pio run -e twatch-s3 -j 1`。
- 固件大小：RAM 约 7.0%，Flash 约 19.0%。
- 上传：通过，端口 `COM9`。
- 串口启动：通过，可见 `[bringup]` 启动日志和每秒心跳。
- 页面已加入 FT6336U 基础触摸观测：最近状态、坐标和事件计数。
- 实物确认：按下、释放、`Events` 计数均有效。
- 实物确认：`Touch release` 会实时显示最后一次触摸坐标。
- 坐标方向：屏幕左上角约为 `0,0`，右下角约为 `240,240`，当前 `rotation=2` 下坐标方向符合视觉方向。
- 本闭环仍不验证复杂手势、滚动物理、触摸唤醒或最终触摸器件选型。

## 2026-05-22 AXP2101 状态观测闭环验证

- 编译：通过，命令为 `pio run -e twatch-s3 -j 1`。
- 固件大小：RAM 约 7.0%，Flash 约 19.1%。
- 上传：通过，端口 `COM9`。
- 串口启动：通过，可见 `[bringup]` 启动日志、每秒心跳和 `[bringup-pmu]` 状态日志。
- USB 接入状态观察：`usb=Y`、`charging=Y`、`discharging=N`、`chg=cc`。
- 电压观察示例：`batt=3540mV`、`vbus=4758mV`、`sys=4747mV`、`percent=4`。
- 实物拔插确认：拔掉 USB 后显示 `usb=N`、`charging=N`、`discharging=Y`，插回 USB 后恢复为 USB 接入充电状态。
- 本闭环仍不验证 PMU 中断、PEK 按键事件、充电策略、关机策略或 sleep/wake。
