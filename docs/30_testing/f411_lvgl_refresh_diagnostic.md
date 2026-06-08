# F411 LVGL Refresh Diagnostic

## 1. 范围与口径

本文档收口 `V0.1-F411-REFRESH-DIAG`，基于当前 F411 LVGL debug screen 的真机观测结果，判断刷新瓶颈更可能落在哪一段。

当前口径说明：

- 场景仍是 debug screen，不是最终产品 UI。
- 黄色 FPS probe 会主动制造刷新负载，因此面积与刷新频率数字带有 debug 负载成分。
- `last ms` 是 flush 链路最近一次端到端耗时。
- `refr ms` 是最近一次 LVGL refresh 周期耗时。
- `conv ms` 是 `watch_lvgl_prepare_flush_bytes()` 的 RGB565 转换耗时。
- `dma wait` 是 DMA pending 从 flush 发起到 `defaultTask` 完成收口的端到端等待耗时，不是纯 SPI 线上传输时间。

## 2. 观测场景

本轮主要基于以下 3 组真机场景：

1. 静态 debug screen
2. 输入压力场景
3. DMA 前强制阻塞路径补测

## 3. 关键证据

### 3.1 DMA 前后基线

DMA 前强制阻塞路径：

- 静态：`calls/s 60`，`full/s 2.5`，`pixels/s 174060`，`last ms 0`，`refr ms 7`，`lvgl/s 140`
- 输入压力：`calls/s 53`，`full/s 2.6`，`pixels/s 177370`，`last ms 2`，`refr ms 74`，`lvgl/s 55`

DMA 后当前路径：

- 静态：`calls/s 63`，`full/s 2.7`，`pixels/s 183154`，`last ms 0`，`refr ms 6`，`lvgl/s 144`
- 输入压力：`calls/s 31`，`full/s 1.6`，`pixels/s 112209`，`last ms 2`，`refr ms 65`，`lvgl/s 3`

稳定性：

- DMA 前后都未观察到花屏、撕裂、卡死、停更。
- 旋钮/输入主观体感：DMA 后与强制阻塞路径相比差不多，没有明显变差。

### 3.2 刷新面积

静态场景：

- `area px 660`
- `area % 0.9`
- `max area px 4800`
- `max area % 7.1`

输入压力场景：

- `area px 4320`
- `area % 6.4`
- `max area px 4800`
- `max area % 7.1`

判断：

- 当前 debug screen 下，最近一次刷新面积静态约 `0.9%`，输入压力约 `6.4%`。
- 即使算上 1 秒窗口峰值，当前观测也只有 `7.1%`，低于路线图中常规 `<= 10%` 的目标。
- 因此“dirty area 过大”暂时不像当前压力场景退化的主因。

### 3.3 RGB565 转换耗时

静态场景：

- `conv ms 1`，偶尔 `0`
- `max 1`

输入压力场景：

- 读数与静态场景基本相同

判断：

- 当前毫秒级计时下，RGB565 转换耗时大多落在 `0~1ms`。
- 因此“转换耗时本身过重”暂时不像主要瓶颈。

### 3.4 SPI / DMA 路径与等待

静态场景：

- `path b/d/f 0/59/0`
- `fb 0`
- `dma wait 0 max 16`

输入压力场景：

- `path b/d/f 0/13/0`
- `fb 0`
- `dma wait 0 max 15`

判断：

- 当前观测窗口内 flush 基本都走 DMA。
- 未看到 `failed` 或 `fb`，说明当前 DMA 链路没有落回错误补发路径。
- `last dma wait` 多数为 `0ms`，但窗口峰值可到 `15~16ms`，说明端到端等待并非完全没有，但现阶段更像调度/收口节奏问题，而不是“DMA 根本没启用”。

## 4. 瓶颈判断

当前最可能的结论是：

1. 不是 DMA 未生效。
2. 不是 RGB565 转换耗时过重。
3. 不是当前 debug screen 场景下 dirty area 明显超预算。
4. 更值得优先怀疑的是 debug 负载与整体刷新调度节奏。

支撑这一判断的核心现象：

- 输入压力时 `refr ms` 从 `74` 降到 `65`，但 `calls/s`、`pixels/s`、尤其 `lvgl/s` 却明显下降。
- 面积没有显著超预算。
- 转换耗时只有 `0~1ms`。
- DMA 路径基本全走通，没有 failed / fallback。

因此，当前“慢”的更可能来源不是单次 flush 太重，而是：

- debug FPS probe 持续制造刷新负载
- debug overlay 自身刷新也在参与负载
- 输入压力下 `lv_timer_handler()` 节奏下降，导致整体刷新次数下来了

## 5. 已验证与未验证

已验证：

- DMA 路径在当前 debug screen 场景下可稳定运行。
- 当前观测窗口内 flush 基本都走 DMA。
- 当前未观察到 failed / fallback。
- 当前 dirty area 未明显超出 `10%` 常规目标。
- 当前 RGB565 转换耗时大多为 `0~1ms`。

未验证：

- 关闭 FPS probe 后的真实静态 UI 刷新面积与节奏。
- 收敛 debug overlay 更新频率后的指标变化。
- 非 debug screen、接近真实产品 UI 场景下的面积与调度表现。
- 纯 SPI 线上传输时间。

## 6. 对 V0.2 的建议

建议按既定顺序进入 `V0.2-F411-REFRESH-OPT`，优先级如下：

1. `V0.2-A`：先隔离 debug FPS probe 与真实 UI 负载
2. `V0.2-B`：再收敛 debug overlay 更新频率和刷新面积
3. `V0.2-C`：最后再评估 draw buffer 行数、刷新周期和静态 UI 常刷

不建议的方向：

- 继续追 DMA 阈值
- 继续改 DMA 完成链路
- 在没有隔离 debug 负载前，直接把当前 debug screen 数字当成产品 UI 结论

## 7. 结论

`V0.1-F411-REFRESH-DIAG` 可以视为完成。

当前最合理的下一步不是继续追 DMA，而是进入 `V0.2`，先减少 debug 负载对观测的污染，再判断真实 UI 负载下还剩多少刷新问题。
