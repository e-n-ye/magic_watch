# MagicWatch 个人成长计划

> 状态：Current Draft  
> 更新日期：2026-06-07  
> 路由类型：开发指南  
> 适用场景：把 MagicWatch 作为嵌入式系统能力训练场，明确每阶段要学会什么

---

## 1. 一句话结论

MagicWatch 不只是一个手表项目，也是一个训练嵌入式系统设计能力的长期练习场。

本计划的目标不是列教程，而是把工程阶段和个人能力成长绑定起来：每做一张卡，都能回答“我学会了什么，为什么这样设计，怎样判断 AI 写得合格”。

---

## 2. 成长路线总览

| 周期 | 必须掌握 | 延后学习 |
|---|---|---|
| 短期 V0 | C 工程化、LVGL 生命周期、性能指标、状态机、Core / UI / Platform 边界 | Zephyr、BLE、复杂 C++、双芯片协议 |
| 中期 V1-V2 | FreeRTOS 任务划分、低功耗策略、资源预算、截图 / 输入回归、LILYGO 体验验证 | 完整 OTA、复杂文件系统、AI / TinyML |
| 长期 V2-V3 | Nordic / NCS、BLE、功耗测量、DFU、跨芯片协议、C++ 开源项目阅读 | 产品级双固件升级和商业生态 |

V0 阶段不要追求 Zephyr、BLE、高级 C++ 或双芯片协议。V0 的核心是把 C、LVGL、事件、状态机、平台边界和性能分析打牢。

---

## 3. G0：C 工程化基础

对应阶段：

- V0.1 到 V0.5。

要掌握：

- `enum`。
- `struct`。
- `union`。
- `static` / `extern` / `const`。
- `.h` / `.c` 边界。
- 函数指针表。
- 固定数组。
- 环形队列。
- 错误码。
- 状态所有权。
- 模块接口设计。

验收标准：

- 能解释为什么 `PowerState` 用 enum。
- 能解释为什么 `PowerController` 需要保存状态。
- 能解释为什么 `EventQueue` 用固定 ring buffer。
- 能解释为什么 `ScreenManager` 用固定页面表。
- 能解释为什么 Core 不能 include `lvgl.h`。

---

## 4. G1：LVGL 嵌入式 UI 基础

对应阶段：

- V0.2 到 V0.4。

要掌握：

- `lv_obj_t` 父子关系。
- screen 生命周期。
- event callback。
- `lv_timer`。
- display flush。
- draw buffer。
- input device。
- invalidate 区域。
- 样式成本。
- 资源格式。

验收标准：

- 能解释 `lv_timer_handler()` 为什么要周期调用。
- 能解释 `flush_cb` 做什么。
- 能解释为什么大面积 invalidate 会慢。
- 能解释为什么 debug overlay 会影响 FPS。
- 能解释为什么页面退出要清理 timer。
- 能解释为什么 F411 不适合 runtime XML + PNG 文件路径作为主路径。

---

## 5. G2：性能分析能力

对应阶段：

- V0.0 到 V0.2。

要掌握：

- FPS。
- flush calls/s。
- pixels/s。
- last flush ms。
- SPI 传输耗时。
- DMA 完成链路。
- RGB565 转换耗时。
- 绘制面积。
- 调度频率。

验收标准：

- 能区分“调用频率高”和“单次耗时高”。
- 能区分“画得慢”和“传得慢”。
- 能判断 debug probe 是否污染了测试。
- 能用数据说明瓶颈，而不是靠体感。
- 能判断某个优化是否该停止。

---

## 6. G3：状态机能力

对应阶段：

- V0.5。

要掌握：

- 状态。
- 事件。
- 转移。
- 动作。
- 状态所有权。
- 边界条件。

验收标准：

- 能先画状态转移表，再写 `PowerController`。
- 能解释 `ScreenManager` 和 `PowerController` 为什么是两个状态机。
- 能解释事件如何驱动状态变化。
- 能解释哪些动作属于 Core 决策，哪些动作属于 Platform 执行。

示例状态表：

| 当前状态 | 事件 | 下一个状态 | 动作 |
|---|---|---|---|
| ACTIVE | timeout | DIM | 降低亮度 |
| DIM | timeout | SLEEP | 关闭屏幕 |
| SLEEP | input | ACTIVE | 点亮屏幕 |
| SLEEP | wrist raise | ACTIVE | 点亮屏幕 |

---

## 7. G4：FreeRTOS / 任务划分能力

对应阶段：

- V1 以后逐步引入。

V0 阶段不急着多任务化。先建立原则：

```text
不是每个模块都需要一个任务。
```

要掌握：

- 什么时候用任务。
- 什么时候用函数。
- 什么时候用队列。
- 什么时候用软件定时器。
- 什么时候用中断。
- 任务优先级怎么判断。
- LVGL 为什么通常单任务访问。

验收标准：

- 能解释 Input 可以是 ISR + queue。
- 能解释 UI 通常一个任务。
- 能解释 Battery 可以是低频任务或 timer。
- 能解释 PowerController 可以不是任务，只是事件处理模块。
- 能解释 ScreenManager 不需要独立任务。

---

## 8. G5：低功耗系统策略

对应阶段：

- V0.5 到 V2。

要掌握：

- 逻辑电源状态。
- 显示功耗。
- 刷新率功耗。
- 传感器采样频率。
- 外设开关。
- tickless。
- STOP / STANDBY。
- 唤醒源。
- 功耗测量。

验收标准：

- 能解释低功耗不是一个 sleep 函数。
- 能解释 `PowerController` 决定策略。
- 能解释 `platform_apply_power_state()` 执行平台动作。
- 能解释真正低功耗需要外设、电源域、唤醒源、测量闭环。

---

## 9. G6：C++ 阅读能力

对应阶段：

- 长期辅助，不作为 V0 主线。

要掌握：

- `class`。
- 构造 / 析构。
- 引用。
- `const`。
- `namespace`。
- 简单 RAII。
- `virtual`。
- lambda 基础。

暂不要求掌握：

- 模板元编程。
- 复杂 STL。
- 智能指针体系。
- `std::function` 深层实现。
- 异常体系。

验收标准：

- 能读懂 InfiniTime 或其他开源嵌入式 C++ 项目的基本模块。
- 能把 C++ 项目里的架构思想翻译成 MagicWatch 可用的 C 风格边界。
- MagicWatch core 仍坚持 C99 风格，不因为读 C++ 项目而引入复杂运行时依赖。

---

## 10. V0.5 能力分水岭

V0.5 是最重要的学习阶段。

这一阶段不要让 AI 一口气生成完整 core，而要拆成学习型小卡：

- `V0.5-A`：只定义 `ScreenId`、`PowerState`、`InputIntent`。
- `V0.5-B`：只定义 `SystemEvent`。
- `V0.5-C`：实现固定 `EventQueue`。
- `V0.5-D`：实现最小固定页面表。
- `V0.5-E`：实现表驱动 `PowerController`。
- `V0.5-F`：实现 `Coordinator`。
- `V0.5-G`：写架构边界检查文档。

每一步都要能回答：

- 为什么这样拆？
- 状态属于谁？
- 事件从哪里来？
- 谁能消费事件？
- 谁不能直接调用谁？
- AI 有没有越界？

如果这些问题答不上来，先暂停写代码，回到图和表。
