# V0 F411 刷新率诊断与无效刷新优化卡片包

本卡片包承接长期路线中的 `V0.1` 和 `V0.2`。目标是把 F411 刷新慢的问题拆成可观测、可解释的指标，再基于数据减少无效刷新。

本包不继续优化 DMA，不迁移 XML UI，不接真实传感器，不改 CubeMX 生成代码。

---

## V0.1-A 定义性能指标结构

- 批次：V0.1-F411-REFRESH-DIAG
- 状态：TODO
- 依赖：`F411-LVGL-DMA-3`
- 自检：`git status --short -uall`；Keil / MDK 编译，或如本机无法编译则明确记录未执行原因；`git diff --check`；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: add f411 lvgl refresh diagnostic fields`
- Doc Impact：small

### 问题定位

当前 `watch_lvgl_perf_snapshot_t` 已有有效刷新、flush 次数、像素吞吐和 handler 频率，但还不能区分刷新面积、RGB565 转换耗时、阻塞传输、DMA 等待和错误 fallback。继续凭体感优化会把问题重新拉回 DMA。

### 实施方案

1. 扩展 `watch_lvgl_perf_snapshot_t`，增加只读观测字段。
2. 字段只描述已经能在现有路径中采样的数据，不改变 flush 行为。
3. 保持现有 `watch_lvgl_port_get_perf_snapshot()` API 作为统一出口。
4. 在 README 或卡片执行记录中解释新增字段含义。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-refresh-q4.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.*`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/**`
- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/**`

Forbidden changes:

- 禁止修改 DMA 完成链路。
- 禁止修改 SPI / LCD 发送行为。
- 禁止优化刷新频率或刷新面积。
- 禁止迁移 XML UI。

### 风险

- 指标字段过多会污染 debug screen 可读性。本卡只建立结构，不要求全部显示出来。

### 验收标准

- 新增指标字段命名清楚，能支持后续区分面积、转换、传输和 DMA 状态。
- 现有 `refresh_per_sec`、`flush_per_sec`、`pixels_per_sec`、`handler_per_sec`、`last_refresh_ms`、`last_flush_ms` 语义不变。
- 不改变现有真机显示行为。

### 执行记录

- 待执行。

---

## V0.1-B 记录 flush 调用频率和刷新面积

- 批次：V0.1-F411-REFRESH-DIAG
- 状态：TODO
- 依赖：`V0.1-A`
- 自检：`git status --short -uall`；Keil / MDK 编译，或如本机无法编译则明确记录未执行原因；`git diff --check`；真机观察新增面积指标随负载变化；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: measure f411 lvgl dirty area`
- Doc Impact：small

### 问题定位

F411 刷新性能首先要确认每次实际刷了多少像素。若 dirty area 过大，DMA 提升有限，优化方向应转向减少无效刷新。

### 实施方案

1. 在 flush 裁剪后记录最近一次刷新像素数。
2. 记录 1 秒窗口内最大 dirty area。
3. 记录最近一次刷新面积占整屏百分比的整数或千分比。
4. 在 debug label 中只显示必要字段，避免遮挡现有色块和字体检查。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lvgl_debug_screen.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-refresh-q4.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.*`
- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/**`

Forbidden changes:

- 禁止调整 FPS 负载探针。
- 禁止改变 draw buffer 行数。
- 禁止修改 DMA 阈值。

### 风险

- Debug label 自身也会制造刷新面积。本卡必须把这些数字标记为 debug 场景指标，不作为产品 UI 基线。

### 验收标准

- 真机上能看到最近刷新面积或面积占比。
- 黄色 FPS probe、底部性能指标和原有颜色 / 字体检查仍可用。
- 可判断当前常规 dirty area 是否超过路线图中的 `<= 10%` 产品目标或 `<= 50%` probe 目标。

### 执行记录

- 待执行。

---

## V0.1-C 记录 RGB565 转换耗时

- 批次：V0.1-F411-REFRESH-DIAG
- 状态：TODO
- 依赖：`V0.1-B`
- 自检：`git status --short -uall`；Keil / MDK 编译，或如本机无法编译则明确记录未执行原因；`git diff --check`；真机观察转换耗时字段稳定可读；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: measure f411 rgb565 conversion time`
- Doc Impact：small

### 问题定位

当前 LVGL flush 会把 `lv_color_t` 裁剪并转换成 LCD 需要的 RGB565 高字节优先 byte stream。若转换耗时接近或超过传输耗时，继续追 DMA 的收益会很小。

### 实施方案

1. 用 `lv_tick_get()` / `lv_tick_elaps()` 包住 `watch_lvgl_prepare_flush_bytes()`。
2. 记录最近一次转换耗时和 1 秒窗口内最大转换耗时。
3. 不引入高精度定时器，不修改 HAL tick，不改转换算法。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lvgl_debug_screen.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-refresh-q4.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.*`
- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/**`

Forbidden changes:

- 禁止改 RGB565 字节序。
- 禁止把转换 buffer 改成动态内存。
- 禁止引入 DWT 或硬件 timer 作为本卡要求。

### 风险

- `lv_tick` 毫秒精度可能让小面积转换显示为 0ms。若出现这种情况，如实记录，不用为了数字好看引入复杂计时器。

### 验收标准

- 能看到最近一次转换耗时或明确记录其小于当前 tick 分辨率。
- 不改变颜色、字体、DMA 和现有 flush 结果。

### 执行记录

- 待执行。

---

## V0.1-D 记录 SPI / DMA 传输耗时和路径次数

- 批次：V0.1-F411-REFRESH-DIAG
- 状态：TODO
- 依赖：`V0.1-C`
- 自检：`git status --short -uall`；Keil / MDK 编译，或如本机无法编译则明确记录未执行原因；`git diff --check`；真机观察 DMA / blocking / fallback 计数随阈值和面积变化；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: measure f411 lcd transfer path stats`
- Doc Impact：small

### 问题定位

DMA 已接通，但还不知道每秒有多少次走 DMA、多少次走阻塞、DMA 等待占多少端到端 flush 耗时、是否发生 error fallback。

### 实施方案

1. 在 LVGL port 层根据 `watch_lcd_draw_rgb565_bytes()` 返回值累计 DMA / blocking / failed 次数。
2. 记录 DMA pending 的端到端等待耗时。
3. 记录 DMA error fallback 次数。
4. 不改 `LCD_DMA_MIN_BYTES`，不改 HAL callback。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.h`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lvgl_debug_screen.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-refresh-q4.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/board/display/watch_lcd.*`
- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/**`

Forbidden changes:

- 禁止修改 `LCD_DMA_MIN_BYTES`。
- 禁止修改 `HAL_SPI_TxCpltCallback()` 或 `HAL_SPI_ErrorCallback()`。
- 禁止把 LVGL ready 调用移回中断。

### 风险

- 端到端 DMA 等待耗时包含 `defaultTask` 调度和 LVGL wait_cb 消费，不等于纯 SPI 线上时间；文档必须说明口径。

### 验收标准

- 能区分 DMA 次数、阻塞次数和 error fallback 次数。
- 能记录最近一次 DMA 等待耗时。
- 未改变 DMA 完成链路。

### 执行记录

- 待执行。

---

## V0.1-E 输出 F411 刷新瓶颈判断文档

- 批次：V0.1-F411-REFRESH-DIAG
- 状态：TODO
- 依赖：`V0.1-D`
- 自检：`git status --short -uall`；`git diff --check`；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`docs: record f411 lvgl refresh diagnostic`
- Doc Impact：required

### 问题定位

指标只有被解释后才有价值。V0.1 的结束条件不是“加了更多数字”，而是能判断慢在绘制、转换、传输、刷新面积还是调度。

### 实施方案

1. 新增 `docs/30_testing/f411_lvgl_refresh_diagnostic.md`。
2. 记录静态 debug screen、FPS probe、输入压力三个场景。
3. 记录已验证项、未验证项和下一步是否进入 V0.2。
4. 不把未烧录、未人工观察的结果写成通过。

### 涉及位置

Allowed files:

- `docs/30_testing/f411_lvgl_refresh_diagnostic.md`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-refresh-q4.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/**`
- `try/my_watch_f411_v2.1/Core/**`
- `ui/lvgl_pro/**`
- `watch_core/**`

Forbidden changes:

- 禁止改代码。
- 禁止宣称未执行的 Keil / 真机验证通过。
- 禁止直接规划 V0.3 上板实现。

### 风险

- 若指标不可复现，本卡必须把 V0.1 标记为 BLOCKED，而不是继续进入 V0.2。

### 验收标准

- 文档能用数据判断主要瓶颈类别。
- 文档明确 V0.2 应先减少哪类无效刷新。
- `V0.1-F411-REFRESH-DIAG` 队列项可被标记为 DONE 或 BLOCKED。

### 执行记录

- 待执行。

---

## V0.2-A 隔离 debug FPS 负载和真实 UI 负载

- 批次：V0.2-F411-REFRESH-OPT
- 状态：TODO
- 依赖：`V0.1-E`
- 自检：`git status --short -uall`；Keil / MDK 编译，或如本机无法编译则明确记录未执行原因；`git diff --check`；真机确认 FPS probe 可单独关闭；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: gate f411 fps load probe`
- Doc Impact：small

### 问题定位

当前黄色 FPS 负载 probe 会主动刷新 220x24 区域，它适合制造稳定负载，但会污染静态 UI 性能判断。

### 实施方案

1. 增加编译期或运行期 probe 开关，默认保持当前 bring-up 行为，允许单独关闭。
2. 关闭 probe 后保留颜色、字体和基本指标显示。
3. README 记录 probe 场景和真实静态场景的区别。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lvgl_debug_screen.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-refresh-q4.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/**`
- `try/my_watch_f411_v2.1/user/board/display/**`
- `try/my_watch_f411_v2.1/Core/**`

Forbidden changes:

- 禁止修改 flush / DMA / SPI 路径。
- 禁止改变输入语义。
- 禁止把 probe 关闭后的指标写成产品最终 FPS。

### 风险

- 若默认关闭 probe，可能让用户误以为刷新停止。本卡必须保留清晰说明。

### 验收标准

- FPS probe 可单独关闭。
- 打开 probe 时行为与当前一致。
- 关闭 probe 后可观察静态 UI 的刷新指标。

### 执行记录

- 待执行。

---

## V0.2-B 收敛 debug overlay 更新频率和刷新面积

- 批次：V0.2-F411-REFRESH-OPT
- 状态：TODO
- 依赖：`V0.2-A`
- 自检：`git status --short -uall`；Keil / MDK 编译，或如本机无法编译则明确记录未执行原因；`git diff --check`；真机观察常规指标更新频率不超过 1Hz；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`perf: reduce f411 debug overlay refresh`
- Doc Impact：small

### 问题定位

路线图要求 debug overlay 常规指标更新频率 `<= 1Hz`。当前 debug screen 每 500ms 更新一次 label，容易让调试界面自身成为刷新负载。

### 实施方案

1. 将常规指标 label 更新频率收敛到 `<= 1Hz`。
2. 尽量只刷新指标区域，不扩大到全屏。
3. 记录调整前后刷新面积、`fps`、`calls/s` 和 `last ms`。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lvgl_debug_screen.c`
- `try/my_watch_f411_v2.1/README.md`
- `docs/30_testing/f411_lvgl_refresh_diagnostic.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-refresh-q4.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/ui/lvgl_port/**`
- `try/my_watch_f411_v2.1/user/board/display/**`
- `try/my_watch_f411_v2.1/Core/**`

Forbidden changes:

- 禁止调大 draw buffer。
- 禁止修改 DMA 阈值。
- 禁止引入新 UI 页面。

### 风险

- 降低 overlay 频率可能让指标反馈变慢，但这正是区分调试负载和产品负载所需的代价。

### 验收标准

- 常规指标更新频率 `<= 1Hz`。
- 记录刷新面积和关键指标变化。
- 同场景无花屏、卡死或输入永久卡住。

### 执行记录

- 待执行。

---

## V0.2-C 评估 draw buffer 行数、刷新周期和静态 UI 常刷

- 批次：V0.2-F411-REFRESH-OPT
- 状态：TODO
- 依赖：`V0.2-B`
- 自检：`git status --short -uall`；`git diff --check`；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`docs: evaluate f411 lvgl refresh budget`
- Doc Impact：required

### 问题定位

V0 不追 60fps，但需要知道当前 `WATCH_LVGL_DRAW_BUF_LINES`、刷新周期和静态 UI 是否处在可解释预算内。

### 实施方案

1. 在诊断文档中记录 `WATCH_LVGL_DRAW_BUF_LINES` 当前值和 RAM 影响。
2. 记录静态 UI 是否存在周期性全屏刷新。
3. 评估是否需要后续调整 draw buffer；没有数据不得调大。
4. 若需要大改 LVGL / driver，按路线图停止，不在本卡继续。

### 涉及位置

Allowed files:

- `docs/30_testing/f411_lvgl_refresh_diagnostic.md`
- `try/my_watch_f411_v2.1/README.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-refresh-q4.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/user/**`
- `try/my_watch_f411_v2.1/Core/**`
- `try/my_watch_f411_v2.1/MDK-ARM/**`

Forbidden changes:

- 禁止改代码。
- 禁止无数据调整 `WATCH_LVGL_DRAW_BUF_LINES`。
- 禁止继续追 DMA。

### 风险

- 如果 V0.1 的指标不可复现，本卡不能给出优化结论，应回到诊断阶段。

### 验收标准

- 文档明确当前 draw buffer、刷新周期和静态刷新预算。
- 能判断是否进入 `V0.3-F411-XML-PROBE`。
- `V0.2-F411-REFRESH-OPT` 队列项可被标记为 DONE 或 BLOCKED。

### 执行记录

- 待执行。
