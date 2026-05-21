# 硬件候选短名单 v1

日期：2026-05-21

## 目的

本文是硬件前哨阶段的第三个小闭环：在 `docs/hardware_candidate_constraints.md` 的约束表基础上，收集第一批具体候选方向。

本轮仍然不是购买决定，也不是最终硬件选型。短名单只回答：

- 哪些硬件方向值得进入下一轮规格核验。
- 每个候选最适合验证哪些架构假设。
- 哪些风险需要在买板、接线或写 bring-up 代码前先查清。

## 初筛结论

当前建议保留 3 个候选方向：

| 候选 | 类别 | 本轮定位 | 初筛结论 |
| --- | --- | --- | --- |
| M5Stack CoreS3 | ESP32 类快速验证板 | 桌面 bring-up / LVGL / 输入 / IMU / 电源路径验证 | 最适合作为“低摩擦第一验证板”候选。 |
| LILYGO T-Watch S3 Plus | 已有参考板 / 高集成手表开发板 | 手表形态、触摸、IMU、电池、PMU、真实佩戴限制验证 | 适合作为“接近手表形态”的参考验证板，但不能默认最终绑定。 |
| PineTime | 低功耗 MCU / 开源手表参考 | nRF52 低功耗路线、FreeRTOS + LVGL 手表软件参考 | 适合作为长期低功耗对照，不建议在开发版可获得性未确认前作为第一验证板。 |

如果下一轮只允许选择一个第一验证板，默认优先级是：

1. 已经在手边且可烧录的硬件。
2. M5Stack CoreS3。
3. LILYGO T-Watch S3 Plus。
4. PineTime 开发版。

原因：第一块验证板的目标是降低未知，而不是一步到位接近最终产品。资料成熟、烧录简单、调试容易，比形态完美更重要。

## 候选 1：M5Stack CoreS3

官方资料：

- [M5Stack CoreS3 documentation](https://docs.m5stack.com/en/core/CoreS3)
- [Zephyr CoreS3 board documentation](https://docs.zephyrproject.org/latest/boards/m5stack/m5stack_cores3/doc/index.html)

### 约束评分

| 维度 | 评分 | 依据 |
| --- | --- | --- |
| 显示 / LVGL | 2 | 2.0 寸 320 x 240 电容触摸 IPS 屏，资料和示例生态成熟。 |
| 触摸 / 输入 | 2 | 具备电容触摸、独立 POWER / RESET 按键，可验证触摸和按键输入路径。 |
| IMU | 2 | 板载 BMI270 六轴 IMU，另有 BMM150 磁力计。 |
| 电源 / 电池 | 2 | 板载 AXP2101 PMU，内置 500mAh 电池，官方资料包含电源管理说明。 |
| 调试生态 | 2 | 支持 Arduino IDE、ESP-IDF、PlatformIO、USB-C 烧录和日志路径。 |
| 手表形态 | 0 | 不是手表形态，不能验证佩戴、边框、安全区和真实外壳体验。 |

### 适合验证

- 最小 LVGL bring-up。
- 触摸事件到 `TouchSample` 的映射可行性。
- 按键到 `CrownPress` 或 wake 输入的替代映射。
- IMU 数据读取和姿态事件路径。
- AXP2101 PMU 下 screen off / wake 的基础实验。
- ESP32-S3 + LVGL 的资源压力和页面渲染压力。

### 主要风险

- 不是可穿戴形态，不能代表最终手表体验。
- 屏幕是 320 x 240，而当前模拟器主要靶心是 240 x 296 圆角矩形，UI 适配意义有限。
- ESP32-S3 路线可能不是最终低功耗目标。

### 下一轮必须核验

- 是否已有可直接复用的 LVGL 最小工程。
- AXP2101 示例能否控制屏幕、背光和外设电源域。
- BMI270 是否能产生低功耗中断，还是只能做运行态姿态数据验证。
- 电池供电下是否容易测量 screen off / wake 行为。

## 候选 2：LILYGO T-Watch S3 Plus

官方资料：

- [LILYGO T-Watch S3 Plus Wiki](https://wiki.lilygo.cc/get_started/en/Wearable/T-Watch-S3-PLUS/T-Watch-S3-PLUS.html)
- [LILYGO T-Watch S3 Plus Store](https://lilygo.cc/en-us/products/t-watch-s3-plus)

### 约束评分

| 维度 | 评分 | 依据 |
| --- | --- | --- |
| 显示 / LVGL | 2 | 240 x 240 LCD，SPI，ST7789V3，手表形态明确。 |
| 触摸 / 输入 | 2 | FT6336U 电容触摸，资料中列出触摸中断引脚；POWER / BOOT / RST 可作为物理输入。 |
| IMU | 2 | BMA423 三轴传感器，资料中列出传感器中断引脚。 |
| 电源 / 电池 | 2 | AXP2101 PMU，商店页标注 BAT-940mAh 和 power detection。 |
| 调试生态 | 2 | 支持 Arduino IDE、ESP-IDF、VS Code、MicroPython，官方 wiki 有快速开始和库。 |
| 手表形态 | 2 | 高集成手表开发板，适合验证佩戴形态、边框和真实屏幕限制。 |

### 适合验证

- 当前表盘中心模型在 240 x 240 手表屏上的压力。
- 触摸点击 / 滑动 / 边缘手势在真实小屏上的可用性。
- BMA423 到 `RaiseToWake` 路径的可行性。
- AXP2101 + 电池供电下 screen off / wake 的限制。
- 高集成手表硬件是否会把架构过早绑定到 ESP32-S3。

### 主要风险

- ESP32-S3 可能偏向快速验证，而不是最终低功耗路线。
- 240 x 240 与当前 240 x 296 模拟器安全区不同，可能需要专门记录 UI 偏差。
- LoRa / GPS / 音频等外设丰富，容易把验证范围带偏。
- 当前没有真实表冠旋转输入，`CrownSample::RotateCW / RotateCCW` 仍需替代验证。

### 下一轮必须核验

- 用户手头是否已有这块硬件，具体版本和 LoRa 频段是什么。
- 官方示例是否能快速点亮屏幕、触摸、BMA423、AXP2101。
- FT6336U 是否支持低功耗触摸唤醒，还是只有运行态触摸中断。
- BMA423 是否能配置为低功耗抬腕 / 运动中断。
- 当前模拟器 240 x 296 UI 是否需要先做 240 x 240 观察，而不是立即适配。

## 候选 3：PineTime

官方资料：

- [PineTime specifications](https://pine64.org/documentation/PineTime/Further_information/Specifications/)
- [PineTime InfiniTime documentation](https://pine64.org/documentation/PineTime/Software/InfiniTime/)
- [PineTime FAQ](https://pine64.org/documentation/PineTime/FAQ/)
- [PINE64 Store](https://pine64.com/shop/)

### 约束评分

| 维度 | 评分 | 依据 |
| --- | --- | --- |
| 显示 / LVGL | 2 | 1.3 寸 240 x 240 IPS 电容触摸屏，ST7789；InfiniTime 基于 LVGL。 |
| 触摸 / 输入 | 1 | 有电容触摸和手表按键，但开发调试依赖具体版本和开放后盖。 |
| IMU | 1 | 规格列出 accelerometer，但低功耗中断能力和具体驱动需进一步核验。 |
| 电源 / 电池 | 1 | 170-180mAh 电池，适合低功耗对照；电源管理可观察性需核验。 |
| 调试生态 | 1 | 开源生态强，但开发版 / sealed 版本差异会影响烧录和调试。 |
| 手表形态 | 2 | 真正手表形态，适合长期参考。 |

### 适合验证

- nRF52 低功耗手表路线的现实约束。
- FreeRTOS + LVGL + NimBLE 的成熟开源参考。
- 240 x 240 小屏上的 UI 密度和手表应用组织方式。
- 与 ESP32-S3 候选形成低功耗路线对照。

### 主要风险

- nRF52832 的 64KB RAM 和 512KB 内置 Flash 明显小于 ESP32-S3 候选，不能直接承载当前模拟器资源规模。
- sealed 版本不适合作为第一块开发验证板，开发版可获得性需要确认。
- 调试需要 SWD 和对应连接方式，bring-up 摩擦比 CoreS3 / T-Watch 更高。
- 它更适合“低功耗参考”，不适合直接承接当前 LVGL 模拟器全量页面。

### 下一轮必须核验

- 当前是否能买到 PineTime 开发版，还是只有 sealed 版本。
- 开发调试需要哪些 SWD 工具和连接线。
- InfiniTime 当前 LVGL 版本、资源规模和页面组织方式。
- 加速度计型号、驱动和低功耗中断能力。
- 是否适合只作为架构参考，而不是第一块 bring-up 板。

## 第一验证板建议

如果用户手头已经有 T-Watch S3 Plus 或等价 TTGO_TWatch：

- 第一块参考验证板优先用已有硬件。
- 目标限制为：屏幕、触摸、BMA423、AXP2101、电池、screen off / wake 观察。
- 不把它宣布为最终平台，也不把 ESP32-S3、240 x 240 屏、FT6336U、BMA423、AXP2101 宣布为最终器件选型。
- 分立主控、屏幕、触摸、IMU、PMIC 的正式选型，应进入 `docs/hardware_component_selection_strategy.md` 描述的第二层验证。

如果当前没有现成手表硬件：

- 第一验证板建议优先考虑 M5Stack CoreS3。
- 它不验证手表形态，但能最快验证 LVGL、触摸、IMU、电源管理和调试链路。
- T-Watch S3 Plus 可作为第二块“手表形态验证板”。

PineTime 暂不建议作为第一验证板，除非已经确认能获得开发版并准备好 SWD 调试条件。

## 路线校正

本短名单的候选对象主要是“参考板 / 开发板”，用于快速降低未知。它不是完整的最终器件选型表。

正式硬件选型需要拆开评估：

- 主控。
- 屏幕和显示驱动。
- 触摸控制器。
- IMU / 姿态传感器。
- PMIC、充电和电量检测。

这一层路线记录在：

- `docs/hardware_component_selection_strategy.md`

## 下一轮建议

原建议为：

`硬件候选规格核验 v1`

由于用户已确认拥有 LILYGO T-Watch S3 Plus，下一轮更适合先进入：

`T-Watch S3 Plus 参考板核验计划`

目标是先用现成硬件验证真实屏幕、触摸、IMU、PMIC、screen off / wake 对当前模拟器假设的影响，再进入分立器件选型表。后续规格核验仍可保留，但不应替代正式器件选型。

后续规格核验建议顺序：

1. 用户手头已有硬件清单和具体版本。
2. CoreS3 是否能最快完成最小 LVGL + 触摸 + IMU bring-up。
3. T-Watch S3 Plus 的 FT6336U / BMA423 / AXP2101 示例是否能直接运行。
4. PineTime 开发版是否可获得，以及 SWD 调试成本是否可接受。
