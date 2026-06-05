# F411-Q2 最小 LVGL 移植闭环

本卡片包用于把 F411 bring-up 从 LCD 直绘 Debug Screen 推进到“LVGL 能编译、能 flush、能由 FreeRTOS 驱动 handler、能消费编码器、能显示最小 label”的最小闭环。

本批次不移植模拟器页面，不接 I2C 传感器，不做 USB/FATFS/蓝牙，不做文件系统，不引入复杂 UI 框架。

---

## F411-LVGL-1 注册最小 LVGL 编译源

- 批次：F411-Q2
- 状态：DONE
- 依赖：`F411-Q1`
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - 定向确认 MDK 工程只登记最小 LVGL 源与必要 include path，不登记 SDL、GPU、demo、examples、旧 porting 大包
- 建议提交信息：`build: register minimal f411 lvgl sources`
- Doc Impact：small

### 问题定位

`user/third_party/lvgl` 已存在，但当前 MDK 工程尚未把 LVGL 作为最小可编译 UI 运行时登记。第一步只解决“能编译进工程”，不碰 flush、不碰输入、不创建界面。

### 实施方案

1. 只在 `MDK-ARM/my_watch_f411.uvprojx` 增加必要 LVGL include path 和最小源文件分组。
2. 优先登记 LVGL core、hal、misc、draw、draw/sw、font、label 所需源文件。
3. 不登记 `draw/sdl`、`gpu`、`demo`、`examples`、`porting` 里的旧工程移植代码。
4. 如发现 `lv_conf.h` 配置不满足最小编译，可只做最小配置修正并记录原因。
5. README 短记：LVGL 源码已进入 MDK 编译登记，但尚未接显示 flush 和输入。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`
- `try/my_watch_f411_v2.1/user/third_party/lvgl/lv_conf.h`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/third_party/lvgl/lvgl/**`
- `try/my_watch_f411_v2.1/user/third_party/lvgl/porting/**`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.*`
- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.*`
- `try/my_watch_f411_v2.1/Core/Src/freertos.c`

Forbidden changes:

- 禁止修改 CubeMX 非 USER CODE 区。
- 禁止修改 `watch_lcd`。
- 禁止创建 LVGL screen 或 label。
- 禁止接 display flush。
- 禁止接 encoder indev。
- 禁止迁移模拟器 UI 页面。
- 禁止接传感器、USB、FATFS、蓝牙。

### 风险

- LVGL 源文件依赖较多，如果一次登记过宽，会把 SDL/GPU/demo 等无关源带入 MDK。
- 如果本轮顺手接 flush 或 UI，会导致编译问题和显示问题混在一起。

### 验收标准

- MDK 工程存在明确的 LVGL 最小源文件分组。
- LVGL include path 足够支持 `#include "lvgl.h"`。
- 不登记 SDL、GPU、demo、examples、旧 porting 大包。
- 本轮不改变真机显示行为。
- README 明确本轮只完成编译登记。

### 执行记录

- 已完成：已将 `lv_conf.h` 从旧项目 UI 配置收窄到最小 label bring-up 配置，关闭未用字体、控件、extra component、默认主题和 flex；已在 MDK 工程登记 LVGL core / draw / draw_sw / font / hal / misc / label 最小源组与 include path；未接 display flush、handler、encoder indev 或 LVGL screen。
- 补充修复：用户本地 MDK 编译暴露 `lv_obj.o` 引用 `lv_extra_init` 未解析；LVGL 8.2 的 `lv_obj.c` 无条件调用 `lv_extra_init()`，因此已补登记 `src/extra/lv_extra.c`。当前 `lv_conf.h` 关闭 flex/grid/fs/png 等 extra 功能，`lv_extra.c` 只作为最小初始化入口，不引入 extra 大包。
- 自检：`git diff --check` 通过，仅有 LF/CRLF 提示；定向扫描确认 MDK 工程未登记 SDL、GPU、demo、examples、old porting 或旧控件源；定向扫描确认 `lv_conf.h` 未残留启用旧控件和大字体。
- 编译：用户本地 MDK 编译先暴露 `lv_extra_init` 未解析；补登记 `lv_extra.c` 后用户已确认 `F411-LVGL-1` 编译成功。

---

## F411-LVGL-2 实现 display flush，复用当前 watch_lcd

- 批次：F411-Q2
- 状态：DONE
- 依赖：`F411-LVGL-1`
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - 真机观察：如本轮只实现 flush 而不创建界面，则不要求 label 出现；不得破坏现有 LCD 初始化和背光
- 建议提交信息：`feat: add f411 lvgl display flush`
- Doc Impact：small

### 问题定位

LVGL 编译进工程后，需要一个项目自己的显示 port，把 LVGL draw buffer 刷到当前已验证可工作的 `watch_lcd`。这一层应复用现有 LCD 驱动，不复制旧项目 porting 里的硬件细节。

### 实施方案

1. 新增 `user/ui/lvgl_port/watch_lvgl_port.h/.c`。
2. 在 port 中调用 `lv_init()`，注册 display buffer、display driver 和 flush callback。
3. 在 `watch_lcd.h/.c` 增加一个最小 RGB565 矩形像素写入 API，供 flush callback 调用。
4. flush callback 只负责把 LVGL 像素区域交给 `watch_lcd`，完成后调用 `lv_disp_flush_ready()`。
5. README 短记：display flush 已接入当前 `watch_lcd`，但尚未接 handler 和输入。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.h`
- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.c`
- `try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/third_party/lvgl/**`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.*`
- `try/my_watch_f411_v2.1/Core/Src/freertos.c`

Forbidden changes:

- 禁止复制旧 `porting/lv_port_disp.c` 作为本轮实现主体。
- 禁止改 LCD 初始化时序，除非编译暴露出必要的声明问题。
- 禁止接 encoder indev。
- 禁止创建最终 UI 页面。
- 禁止迁移模拟器 UI。
- 禁止接传感器、USB、FATFS、蓝牙。

### 风险

- RGB565 字节序可能与 LCD 发送顺序不一致，需要在 `watch_lcd` API 内明确处理。
- flush buffer 过大可能挤占 SRAM，应使用小行缓冲或合理 draw buffer。

### 验收标准

- 存在项目自有 `watch_lvgl_port`。
- display flush 只依赖 `watch_lcd`，不直接操作 SPI/GPIO 句柄。
- `watch_lcd` 对外提供最小 RGB565 block 写入能力。
- 构建通过。
- 不创建实际 LVGL 界面。

### 执行记录

- 已完成：已新增 `user/ui/lvgl_port/watch_lvgl_port.*`，注册 LVGL display driver 和 draw buffer；已在 `watch_lcd` 暴露 `watch_lcd_draw_rgb565()`，flush 只通过该 API 写入 LCD，不直接操作 SPI/GPIO；MDK 工程已登记 `User/ui/lvgl_port` 分组；尚未接 `lv_timer_handler()`、encoder indev 或 LVGL screen。
- 自检：`git diff --check` 通过，仅有 LF/CRLF 提示；定向扫描确认本轮未调用 `lv_timer_handler`、未注册 indev、未创建 label/screen；定向扫描确认 `watch_lvgl_port` 未直接引用 SPI/GPIO/HAL 句柄。
- 编译：本机命令行无法执行 Keil / MDK 编译，需要用户本地编译验证。

---

## F411-LVGL-3 接 lv_tick / lv_timer_handler 到 FreeRTOS

- 批次：F411-Q2
- 状态：TODO
- 依赖：`F411-LVGL-2`
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - 确认所有 LVGL API 仍只在 defaultTask / bring-up task 语境调用
- 建议提交信息：`feat: drive f411 lvgl handler from freertos`
- Doc Impact：small

### 问题定位

display flush 存在后，LVGL 还需要稳定 tick 和周期性 `lv_timer_handler()`。当前工程已有 FreeRTOS default task 调用 `watch_bringup_task()`，本轮应优先复用这条主循环，而不是新建复杂 UI task。

### 实施方案

1. 核对 `lv_conf.h` 当前 tick 策略；若已使用 `HAL_GetTick()`，记录为 custom tick，不额外调用 `lv_tick_inc()`。
2. 在 `watch_bringup_init()` 初始化 LVGL port。
3. 在 `watch_bringup_task()` 里按固定周期调用 `lv_timer_handler()`。
4. 如需节流，使用简单时间间隔判断，不新增复杂 timer 抽象。
5. README 短记：LVGL handler 由当前 FreeRTOS defaultTask 驱动，LVGL API 暂时保持单任务调用。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/app/watch_bringup.h`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/user/third_party/lvgl/lv_conf.h`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/Core/Src/freertos.c`
- `try/my_watch_f411_v2.1/user/services/input/**`
- `try/my_watch_f411_v2.1/user/app/input/**`
- `try/my_watch_f411_v2.1/user/app/screen/**`

Forbidden changes:

- 禁止新增 FreeRTOS task。
- 禁止修改 CubeMX 非 USER CODE 区。
- 禁止让多个 task 调用 LVGL API。
- 禁止接 encoder indev。
- 禁止创建最终 UI 页面。
- 禁止迁移模拟器 UI。

### 风险

- 如果 `lv_tick_inc()` 和 `LV_TICK_CUSTOM` 同时启用，可能形成重复 tick 语义。
- 如果 `lv_timer_handler()` 调用太频繁，会影响输入扫描；调用太少则刷新不及时。

### 验收标准

- LVGL tick 策略明确：`HAL_GetTick()` custom tick 或显式 `lv_tick_inc()` 二选一。
- `lv_timer_handler()` 由现有 FreeRTOS defaultTask 路径驱动。
- 不新增 UI task。
- 构建通过。
- README 明确 LVGL 单任务调用约束。

### 执行记录

- 待执行。

---

## F411-LVGL-4 接 encoder indev

- 批次：F411-Q2
- 状态：TODO
- 依赖：`F411-LVGL-3`
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - 真机观察：编码器旋转和短按能被 LVGL indev 消费，至少可通过 debug 计数或后续 label 变化验证
- 建议提交信息：`feat: add f411 lvgl encoder input`
- Doc Impact：small

### 问题定位

模拟器主交互只有表冠旋转和按下。F411 项目也已决定 BACK / WAKE 不作为后续主交互入口，因此 LVGL 输入设备应先只接编码器，不把所有按键都塞进 UI。

### 实施方案

1. 在 `watch_lvgl_port` 中新增 encoder indev 初始化。
2. indev read callback 消费当前输入服务或应用 intent 中的编码器旋转、短按、长按语义。
3. 旋转映射为 `enc_diff`，短按映射为最小 press/release 语义。
4. BACK / WAKE 不映射到 LVGL indev。
5. README 短记：LVGL 当前只接 encoder indev，BACK / WAKE 仍不作为主 UI 输入。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/services/input/watch_input_service.*`
- `try/my_watch_f411_v2.1/user/app/input/watch_input_intent.*`
- `try/my_watch_f411_v2.1/user/core/event/watch_event_queue.*`
- `try/my_watch_f411_v2.1/user/board/input/**`

Forbidden changes:

- 禁止改变编码器硬件扫描和消抖语义。
- 禁止把 BACK / WAKE 接入 LVGL 主 UI。
- 禁止新增触摸输入。
- 禁止移植模拟器页面。
- 禁止接传感器、USB、FATFS、蓝牙。

### 风险

- 当前输入服务以事件为主，不是持续按压状态；短按到 LVGL press/release 需要明确最小语义，不能假装已经有完整按压状态机。
- 如果 LVGL indev 和 ScreenManager 同时消费同一输入源，可能出现事件被抢走或重复消费。

### 验收标准

- LVGL 注册 encoder indev。
- 编码器旋转能进入 LVGL `enc_diff`。
- 编码器短按有明确的最小 press/release 处理。
- BACK / WAKE 不进入 LVGL 主 UI。
- 不改变底层输入扫描与消抖。

### 执行记录

- 待执行。

---

## F411-LVGL-5 显示一个最小 label/debug screen

- 批次：F411-Q2
- 状态：TODO
- 依赖：`F411-LVGL-4`
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - 真机手动验证：屏幕显示最小 LVGL label/debug screen；编码器旋转或短按能改变可观察文本、计数或焦点状态
- 建议提交信息：`feat: show f411 lvgl debug screen`
- Doc Impact：small

### 问题定位

编译、flush、handler、encoder 都接通后，需要一个可见的最小 LVGL screen 来证明链路闭环。这个 screen 只用于 debug，不代表开始迁移模拟器 UI。

### 实施方案

1. 新增 `user/app/lvgl_demo/watch_lvgl_debug_screen.h/.c` 或等价小模块。
2. 创建一个 LVGL label，显示固定标题、事件计数或当前输入状态。
3. 将 debug screen 初始化挂到 `watch_bringup_init()` 中。
4. 编码器事件可通过 LVGL indev 或简单 debug 计数反映到 label。
5. README 短记：F411-Q2 最小 LVGL 闭环完成，但模拟器 UI 尚未迁移。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lvgl_debug_screen.h`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lvgl_debug_screen.c`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.h`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/app/screen/watch_screen_manager.*`
- `try/my_watch_f411_v2.1/user/services/input/**`
- `try/my_watch_f411_v2.1/user/core/event/**`
- `try/my_watch_f411_v2.1/Core/Src/freertos.c`

Forbidden changes:

- 禁止迁移模拟器页面。
- 禁止引入复杂 PageManager。
- 禁止接传感器、USB、FATFS、蓝牙。
- 禁止删除现有 LCD Debug Screen 代码，除非用户另行确认。
- 禁止把构建通过写成真机 UI 验证通过。

### 风险

- label/debug screen 容易被继续扩成正式 UI；本轮必须克制，只证明 LVGL 链路。
- 如果本轮还去改 ScreenManager 结构，会把 Q1 骨架和 Q2 LVGL bring-up 混在一起。

### 验收标准

- 真机显示一个最小 LVGL label/debug screen。
- 编码器旋转或短按能产生可观察变化。
- README 明确这只是 LVGL bring-up，不是模拟器 UI 移植。
- 构建通过。
- 手动真机验证结果如实记录。

### 执行记录

- 待执行。
