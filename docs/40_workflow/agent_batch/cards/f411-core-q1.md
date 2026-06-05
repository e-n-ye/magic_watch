# F411-Q1 最小应用骨架闭环

本卡片包用于把 F411 bring-up 从“硬件事件直接改色块”推进到“编码器语义 -> EventQueue-lite -> ScreenManager -> LCD Debug Screen”的最小应用骨架。

本批次不移植 LVGL，不接 I2C 传感器，不做 USB/FATFS/蓝牙，不搬模拟器 UI。

---

## F411-CORE-1 输入语义收口

- 批次：F411-Q1
- 状态：DONE
- 依赖：无
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - `rg "WATCH_INPUT_BACK|WATCH_INPUT_WAKE" try/my_watch_f411_v2.1/user/app/input try/my_watch_f411_v2.1/user/app/watch_bringup.c`，确认 BACK / WAKE 不进入主应用交互语义
- 建议提交信息：`feat: add f411 input intent layer`
- Doc Impact：small

### 问题定位

当前 F411 基线已经能消费编码器、BACK、WAKE 等硬件事件，但 `watch_bringup.c` 仍直接解释 `watch_input_event_t` 并改屏幕颜色。下一步需要让主交互先沉淀成稳定 `InputIntent`，避免后续 ScreenManager、LVGL、传感器调试都依赖硬件事件枚举。

### 实施方案

1. 新增 `user/app/input/watch_input_intent.h/.c`。
2. 定义 C-first、固定大小的 `watch_input_intent_t` 枚举，只包含后续主交互需要的编码器旋转、确认、长按和 none。
3. 新增从 `watch_input_event_t` 到 `watch_input_intent_t` 的映射函数。
4. `BACK` / `WAKE` 仍可保留在底层 bring-up 输入服务里，但不映射为主应用 intent。
5. `watch_bringup.c` 改为通过 `InputIntent` 决定编码器相关颜色反馈；非主交互事件可保持 bring-up 调试显示或忽略，但不得进入主应用语义。
6. 更新 MDK 工程文件登记新增 `.c`。
7. 在 F411 README 短记当前输入决策：主交互从本轮开始以 `InputIntent` 为应用入口。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/app/input/watch_input_intent.h`
- `try/my_watch_f411_v2.1/user/app/input/watch_input_intent.c`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.h`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
- `try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`
- `try/my_watch_f411_v2.1/README.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/services/input/watch_input_service.h`
- `try/my_watch_f411_v2.1/user/services/input/watch_input_service.c`
- `try/my_watch_f411_v2.1/user/board/input/watch_encoder_hw.*`
- `try/my_watch_f411_v2.1/user/board/input/watch_key_hw.*`
- `try/my_watch_f411_v2.1/Core/Src/freertos.c`

Forbidden changes:

- 禁止修改 CubeMX 非 USER CODE 区。
- 禁止修改 FreeRTOS 任务结构。
- 禁止引入 LVGL。
- 禁止引入 heap、字符串、动态容器或复杂对象所有权。
- 禁止把 BACK / WAKE 设计成后续主交互入口。
- 禁止改编码器硬件扫描和去抖语义。

### 风险

- 如果本轮顺手把事件队列或页面状态机也做了，会导致边界失控。
- 如果把 BACK / WAKE 也纳入主应用 intent，会偏离当前硬件交互决策。

### 验收标准

- `InputIntent` 层存在，并且编码器事件通过该层进入 bring-up 显示逻辑。
- 编码器顺时针、逆时针、短按、长按仍可被真机观察到。
- BACK / WAKE 不进入主应用交互语义。
- 不引入 LVGL、EventQueue、ScreenManager。
- MDK 工程包含新增源文件。
- README 已短记本轮输入语义事实。

### 执行记录

- 已完成：新增 `user/app/input/watch_input_intent.*`，编码器旋转、短按、长按已映射为应用层 `InputIntent`。
- 自检：`rg "WATCH_INPUT_BACK|WATCH_INPUT_WAKE" try/my_watch_f411_v2.1/user/app/input try/my_watch_f411_v2.1/user/app/watch_bringup.c` 无命中；`git diff --check` 通过，仅有 LF/CRLF 提示。
- 编译：本机命令行未找到 `UV4.exe` / `armcc`，Keil / MDK 编译需由用户本地执行。

---

## F411-CORE-2 EventQueue-lite

- 批次：F411-Q1
- 状态：DONE
- 依赖：`F411-CORE-1`
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - `rg "malloc|free|strdup|std::|new |delete " try/my_watch_f411_v2.1/user/core/event try/my_watch_f411_v2.1/user/app/watch_bringup.c` 无命中
- 建议提交信息：`feat: add f411 event queue lite`
- Doc Impact：small

### 问题定位

`InputIntent` 解决了输入语义，但当前 bring-up 仍是同步拉取事件后直接改 UI。下一步需要一个固定数组 `EventQueue-lite`，作为未来 Service、Coordinator、ScreenManager 的共同连接点。

### 实施方案

1. 新增 `user/core/event/watch_event_queue.h/.c`。
2. 定义固定大小 `watch_app_event_t`，优先只承载 `InputIntent` 事件。
3. 实现 init、push、pop、is_empty 或 drop_count 查询。
4. 队列使用静态或 context 内固定数组，不使用 heap。
5. `watch_bringup.c` 将 `InputIntent` 包装成 app event 后再消费，显示效果保持尽量不变。
6. 更新 MDK 工程文件登记新增 `.c`。
7. README 短记：应用层事件入口已经从直接 intent 调用升级为固定队列。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/core/event/watch_event_queue.h`
- `try/my_watch_f411_v2.1/user/core/event/watch_event_queue.c`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.h`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
- `try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`
- `try/my_watch_f411_v2.1/README.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/app/input/watch_input_intent.h`
- `try/my_watch_f411_v2.1/user/app/input/watch_input_intent.c`
- `try/my_watch_f411_v2.1/user/services/input/watch_input_service.*`
- `try/my_watch_f411_v2.1/Core/Src/freertos.c`

Forbidden changes:

- 禁止新增通用复杂 EventBus。
- 禁止使用 heap、字符串、动态容器、函数指针订阅表。
- 禁止引入 LVGL。
- 禁止修改 FreeRTOS 任务结构。
- 禁止改变 `watch_input_service` 的硬件扫描语义。

### 风险

- 队列如果设计得太通用，会提前变成模拟器 `EventBus` 的影子。
- 队列溢出策略必须简单明确，不能悄悄阻塞任务。

### 验收标准

- 存在固定大小 `EventQueue-lite`。
- 输入 intent 通过 app event 进入 bring-up 消费逻辑。
- 队列满时有明确 drop 策略或计数。
- 无 heap / 字符串 / 动态容器。
- MDK 工程包含新增源文件。

### 执行记录

- 已完成：新增 `user/core/event/watch_event_queue.*`，输入 intent 已包装为固定大小 app event 并通过静态队列进入 bring-up 消费逻辑。
- 自检：`rg "malloc|free|strdup|std::|new |delete " try/my_watch_f411_v2.1/user/core/event try/my_watch_f411_v2.1/user/app/watch_bringup.c` 无命中；`git diff --check` 通过，仅有 LF/CRLF 提示。
- 编译：本机命令行未找到 `UV4.exe` / `armcc`，Keil / MDK 编译需由用户本地执行。

---

## F411-CORE-3 最小 ScreenManager

- 批次：F411-Q1
- 状态：DONE
- 依赖：`F411-CORE-2`
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - 真机手动验证：编码器旋转 / 短按 / 长按能驱动 Home / Debug / Menu 或等价最小状态变化
- 建议提交信息：`feat: add f411 minimal screen manager`
- Doc Impact：small

### 问题定位

项目现在已经有 LCD 画色块能力和输入事件能力，但还没有“当前屏幕是谁、输入如何改变屏幕状态”的应用层骨架。下一步需要在不引入 LVGL 的情况下建立最小 ScreenManager。

### 实施方案

1. 新增 `user/app/screen/watch_screen_manager.h/.c`。
2. 定义极简屏幕枚举，例如 Home、Debug、Menu，名称可按实现时更合理调整。
3. ScreenManager 消费 `watch_app_event_t`，更新当前屏幕或屏幕内计数状态。
4. 继续使用 `watch_lcd_*` 直接绘制 Debug Screen，不引入 LVGL。
5. `watch_bringup.c` 从直接显示事件颜色转为初始化 ScreenManager、把事件交给 ScreenManager、由 ScreenManager 输出最小绘制结果。
6. 更新 MDK 工程文件登记新增 `.c`。
7. README 短记：F411-Q1 最小应用骨架已经到 ScreenManager，下一批才考虑 LVGL。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/app/screen/watch_screen_manager.h`
- `try/my_watch_f411_v2.1/user/app/screen/watch_screen_manager.c`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.h`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
- `try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`
- `try/my_watch_f411_v2.1/README.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/core/event/watch_event_queue.*`
- `try/my_watch_f411_v2.1/user/app/input/watch_input_intent.*`
- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.*`
- `try/my_watch_f411_v2.1/Core/Src/freertos.c`

Forbidden changes:

- 禁止移植 LVGL。
- 禁止移植模拟器页面。
- 禁止接传感器、USB、FATFS、蓝牙。
- 禁止修改 FreeRTOS 任务结构。
- 禁止让 ScreenManager 访问 GPIO、I2C、SPI 外设句柄。
- 禁止把页面 UI 细节塞回硬件驱动或输入服务。

### 风险

- ScreenManager 如果一开始设计过大，会复刻模拟器的 PageManager 复杂度。
- LCD Debug Screen 只能作为骨架验证，不应被写成最终 UI。

### 验收标准

- 存在最小 ScreenManager。
- 编码器输入通过 `InputIntent -> EventQueue-lite -> ScreenManager` 到达 LCD Debug Screen。
- 真机可观察至少两种屏幕状态或状态计数变化。
- 不引入 LVGL，不移植模拟器页面。
- MDK 工程包含新增源文件。
- README 明确下一批才考虑 LVGL。

### 执行记录

- 代码已完成：新增 `user/app/screen/watch_screen_manager.*`，编码器输入已按 `InputIntent -> EventQueue-lite -> ScreenManager -> LCD Debug Screen` 链路消费。
- 自检：应用层 BACK / WAKE 扫描无命中；heap / 动态对象扫描无命中；LVGL / 传感器 / USB / FATFS / 蓝牙误触扫描无命中；MDK 工程已登记 `User/app/screen`；`git diff --check` 通过，仅有 LF/CRLF 提示；本轮中文文档乱码哨兵检查通过。
- 编译：本机命令行未找到 `UV4.exe` / `armcc`，Keil / MDK 编译需由用户本地执行。
- 手动回归：用户已确认 `F411-Q1 编译通过，真机回归通过`；编码器旋转、短按、长按能驱动最小屏幕状态变化。
