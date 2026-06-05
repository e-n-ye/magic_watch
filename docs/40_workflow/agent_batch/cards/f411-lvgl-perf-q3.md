# F411-Q3 LVGL 性能基线与 SPI DMA 准备

本卡片包用于在 F411 最小 LVGL 链路已经点亮后，先建立可观测性能基线，再分步验证颜色 / 字体质量，并为后续 CubeMX 配置 SPI DMA 后的用户层接入做准备。

本批次不迁移模拟器 UI，不接传感器、USB、FATFS、蓝牙，不手写 CubeMX DMA 初始化。

---

## F411-LVGL-PERF-1 显示 LVGL flush 性能基线

- 批次：F411-Q3
- 状态：DONE
- 依赖：`F411-LVGL-5`
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - 真机观察：debug label 能显示 `flush/s`、`px/s`、`last ms`、`handler/s`
- 建议提交信息：`feat: show f411 lvgl flush metrics`
- Doc Impact：small

### 问题定位

当前 LVGL flush 仍是逐行 RGB565 转换和阻塞 SPI 写入。进入 SPI DMA 前，需要先有稳定的基线指标，否则无法判断 DMA 是否真的提升了刷新链路。

### 实施方案

1. 在 `watch_lvgl_port` 中统计每秒 flush 次数、每秒刷新像素数、最近一次 flush 耗时和 `lv_timer_handler()` 调用频率。
2. 在最小 LVGL debug screen 中显示这些指标。
3. 不启用 LVGL 内建 perf monitor，避免扩大 LVGL 配置和源文件登记范围。
4. 不改 SPI 发送路径，不改 DMA，不改 CubeMX 配置。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lvgl_debug_screen.h`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lvgl_debug_screen.c`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-q2.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-perf-q3.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.*`
- `try/my_watch_f411_v2.1/user/third_party/lvgl/**`
- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`

Forbidden changes:

- 禁止修改 SPI / LCD 发送实现。
- 禁止修改 CubeMX 生成代码。
- 禁止修改 MDK 工程登记。
- 禁止启用 DMA。
- 禁止启用 LVGL 内建 perf monitor。
- 禁止迁移模拟器 UI。
- 禁止接传感器、USB、FATFS、蓝牙。

### 风险

- `flush/s` 不是完整 UI FPS；它是当前 flush 调用频率。后续对比 DMA 时必须保持相同口径。
- debug label 自身刷新也会带来额外 flush，适合作为 bring-up 性能基线，不代表最终 UI 帧率。

### 验收标准

- debug label 显示 `flush/s`、`px/s`、`last ms`、`handler/s`。
- 不改变现有阻塞 SPI flush 路径。
- 构建通过。
- 真机验证结果如实记录。

### 执行记录

- 已完成：已在 `watch_lvgl_port` 统计 flush 次数、刷新像素数、最近一次 flush 耗时和 handler 调用频率；已在 LVGL debug label 中周期显示性能基线。
- 自检：`git diff --check` 通过，仅有 LF/CRLF 提示；本轮实际改动中文文档乱码哨兵检查通过；定向扫描确认未修改 `watch_lcd`、CubeMX 生成代码或 MDK 工程，未启用 `LV_USE_PERF_MONITOR`，未引入 DMA。
- 编译与真机：本机命令行无法执行 Keil / MDK 编译，需要用户本地编译并真机验证性能指标显示。

---

## F411-LVGL-PERF-2 验证 RGB565 字节序和字体显示质量

- 批次：F411-Q3
- 状态：TODO
- 依赖：`F411-LVGL-PERF-1`
- 自检：`git status --short -uall`；Keil / MDK 编译；`git diff --check`；真机观察红绿蓝白黑与小字/大字显示
- 建议提交信息：`test: add f411 lvgl color and font check screen`
- Doc Impact：small

### 问题定位

当前字体边缘略糊，需要区分是字号 / 抗锯齿观感、RGB565 字节序混色问题，还是拍照放大带来的观感问题。

### 实施方案

1. 在 debug screen 或单独测试 screen 中显示红、绿、蓝、白、黑色块。
2. 显示不同字号的英文测试文字。
3. 必要时只做字节序 A/B 验证，不改 DMA。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/app/lvgl_demo/**`
- `try/my_watch_f411_v2.1/user/third_party/lvgl/lv_conf.h`
- `try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-perf-q3.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.*`
- `try/my_watch_f411_v2.1/Core/**`

Forbidden changes:

- 禁止修改 SPI / DMA。
- 禁止迁移模拟器 UI。
- 禁止接传感器、USB、FATFS、蓝牙。

### 风险

- 字体显示质量可能需要增加字体源登记，必须控制在验证范围内，不能一次性引入大字体全集。

### 验收标准

- 真机能区分红绿蓝白黑。
- 字体显示测试能帮助判断是否需要调整字号或字节序。
- 不接 DMA。

### 执行记录

- 待执行。

---

## F411-LVGL-DMA-1 增加 watch_lcd SPI DMA fallback 接口

- 批次：F411-Q3
- 状态：TODO
- 依赖：`F411-LVGL-DMA-PREP`
- 自检：`git status --short -uall`；Keil / MDK 编译；`git diff --check`
- 建议提交信息：`feat: add f411 lcd spi dma fallback path`
- Doc Impact：small

### 问题定位

CubeMX 配置 SPI1 TX DMA 后，需要先在 `watch_lcd` 层增加可控的 DMA 发送入口，同时保留阻塞 SPI fallback，不能直接把 LVGL flush 改成异步。

### 实施方案

1. 在 `user/board/display/watch_lcd.*` 增加 DMA 可用性和异步发送接口。
2. DMA 不可用或忙时保留阻塞路径。
3. 不移动 `lv_disp_flush_ready()`。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.h`
- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-perf-q3.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/**`
- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/**`

Forbidden changes:

- 禁止手写 CubeMX DMA 初始化。
- 禁止修改 LVGL flush 异步语义。
- 禁止删除阻塞 fallback。

### 风险

- DMA buffer 生命周期如果没锁清楚，会导致花屏或数据被覆盖。本轮只做 LCD 层接口，不让 LVGL 直接依赖它。

### 验收标准

- `watch_lcd` 提供 DMA fallback 接口。
- 阻塞路径仍可用。
- 构建通过。

### 执行记录

- 待执行。

---

## F411-LVGL-DMA-2 让 LVGL flush 使用异步 DMA

- 批次：F411-Q3
- 状态：TODO
- 依赖：`F411-LVGL-DMA-1`
- 自检：`git status --short -uall`；Keil / MDK 编译；`git diff --check`；真机观察无明显花屏或卡死
- 建议提交信息：`feat: use spi dma for f411 lvgl flush`
- Doc Impact：small

### 问题定位

当前 `lv_disp_flush_ready()` 在阻塞 flush 结束后立即调用。切到 DMA 后，必须等 DMA 完成回调或完成轮询确认后再通知 LVGL，否则 LVGL draw buffer 可能被提前复用。

### 实施方案

1. 在 `watch_lvgl_port` 中改用 `watch_lcd` DMA 发送入口。
2. 将 `lv_disp_flush_ready()` 移到 DMA 完成路径。
3. 保留 DMA 不可用时的阻塞 fallback。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.h`
- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-perf-q3.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/**`

Forbidden changes:

- 禁止手写 CubeMX DMA 初始化。
- 禁止新增复杂 UI。
- 禁止迁移模拟器 UI。

### 风险

- 完成回调上下文和 LVGL 线程安全需要谨慎处理；如果不能安全直接回调 LVGL，应改成标志位由 defaultTask 消费。

### 验收标准

- DMA flush 后再调用 `lv_disp_flush_ready()`。
- DMA fallback 正常。
- 真机无明显花屏、卡死。

### 执行记录

- 待执行。

---

## F411-LVGL-DMA-3 对比 DMA 前后性能和稳定性

- 批次：F411-Q3
- 状态：TODO
- 依赖：`F411-LVGL-DMA-2`
- 自检：记录 DMA 前后 `flush/s`、`px/s`、`last ms`、`handler/s`；真机连续运行观察
- 建议提交信息：`docs: record f411 lvgl dma performance result`
- Doc Impact：small

### 问题定位

DMA 接入完成不等于性能目标完成，必须用同一套指标记录前后差异，并观察旋钮响应、撕裂、花屏和长时间运行稳定性。

### 实施方案

1. 用 `F411-LVGL-PERF-1` 指标记录阻塞路径和 DMA 路径表现。
2. 记录旋钮响应、是否撕裂、是否花屏、是否卡死。
3. 不继续扩展 UI。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-perf-q3.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/**`
- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/**`

Forbidden changes:

- 禁止改代码。
- 禁止宣称未执行的真机稳定性结论。

### 风险

- 只有 FPS 数字提升但出现撕裂或卡死，不应算 DMA 优化通过。

### 验收标准

- 文档记录 DMA 前后指标。
- 明确手动观察结果。
- 未验证项如实保留。

### 执行记录

- 待执行。

---

## F411-LVGL-DMA-PREP CubeMX 配置 SPI DMA 准备

- 批次：F411-Q3
- 状态：TODO
- 依赖：`F411-LVGL-PERF-2`
- 自检：用户在 CubeMX 中完成 SPI1 TX DMA 配置并重新生成工程；现有阻塞刷屏仍可编译运行
- 建议提交信息：`chore: prepare f411 spi dma cubemx config`
- Doc Impact：small

### 问题定位

SPI DMA 的底层初始化必须由 CubeMX 生成，避免 AI 手写或篡改 `MX_DMA_Init()` / MSP 初始化导致工程状态不可控。

### 实施方案

1. 本卡只记录 CubeMX 配置前置清单。
2. 用户在 CubeMX 中配置 SPI1 TX DMA，并重新生成工程。
3. Agent 后续只在 `user/` 层接入 DMA fallback 接口。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-perf-q3.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/**`
- `try/my_watch_f411_v2.1/*.ioc`

Forbidden changes:

- 禁止 Agent 手写 DMA 初始化。
- 禁止 Agent 修改 CubeMX 非 USER CODE 区。
- 禁止接 LVGL 异步 flush。

### 风险

- CubeMX 重新生成可能改动工程文件和 HAL 初始化；必须单独验收，不和用户层 DMA 接口混在一轮。

### 验收标准

- 用户完成 CubeMX SPI1 TX DMA 配置。
- Keil 编译通过。
- 现有 LVGL 阻塞 flush 仍正常。

### 执行记录

- 待执行。
