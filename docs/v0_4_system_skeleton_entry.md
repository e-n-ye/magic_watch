# Magic Watch v0.4 系统骨架与平台契约收口

日期：2026-05-22

## 阶段判断

Magic Watch 当前从“模拟器壳层预研”和“T-Watch S3 Plus 硬件前哨验证”之间重新收口，进入：

`v0.4 系统骨架与平台契约收口`

这个阶段的重点不是继续增加页面，也不是立刻切换到真实平台开发，而是把已经形成的交互壳、显示策略、输入模型和硬件前哨经验整理成能继续长大的系统骨架。

当前最高价值问题是：

- 哪些对象长期存在，哪些只是页面或临时壳层。
- 哪些状态属于系统状态机，哪些属于页面栈、显示策略、服务模型或平台采样。
- 模拟器输入和真实硬件输入如何映射到同一套高层意图。
- 第一批真实 service 边界应该如何出现，而不是让页面继续持有业务解释逻辑。

## 为什么暂停硬件前哨

T-Watch S3 Plus 作为参考板已经完成了足够的第一阶段核验：

- 最小 LVGL 画面和串口日志可用。
- FT6336U 基础触摸事件、坐标方向和边缘输入路径可观察。
- AXP2101 基础供电、电池、充电和外部供电状态可读。
- BMA423 基础加速度和姿态变化可读。
- 运行态 screen off / on 可由侧键和触摸恢复。
- timer deep sleep 进入和自动重启式恢复路径可用。
- PMU / PEK 侧键 deep sleep 唤醒路径可用。
- bring-up 首屏耗时已收敛到当前可接受的 2 到 3 秒级。

这些结果已经回答了“真实硬件是否会立刻推翻当前架构假设”的第一层问题。继续沿 T-Watch 深挖触摸唤醒、IMU 中断、GPS、Radio、DRV2605、FFat、音频或真实功耗，会快速进入板级细节；短期内它们对系统骨架训练的收益低于回到主线收口。

因此，T-Watch S3 Plus 暂时只保留为参考验证板和后续对照组，不作为最终平台，不继续随机扩展硬件实验。

## 模拟器接下来解决什么

模拟器仍然是主线工具，但它的任务从“继续堆 UI 页面”调整为“验证系统结构是否清楚”。

接下来模拟器应优先解决：

- 输入意图边界：原始触摸、按键、表冠、姿态模拟如何变成高层 intent。
- 系统状态边界：Running、ScreenOff、PoweredOff、overlay、shell surface、page stack 分别归谁管。
- 导航边界：主页环、普通页面栈、临时系统壳层和弹窗流不要互相吞并。
- 显示策略边界：策略规则、状态切换、页面渲染和设置 UI 各自保持清楚。
- 服务边界：时间、电池、电源状态、传感器、通知等领域能力不要长在页面里。
- 回归能力：每次修改后能用现有手工矩阵或更小目标矩阵确认主路径没有漂移。

## 本阶段不做什么

v0.4 不做以下事情：

- 不继续优先增加 Weather、Steps、Sleep、Health、NFC、支付等 mock 页面。
- 不采购或切换 STM32H7、STM32F407、ESP32-S3 最小系统板。
- 不迁移或修复旧 F411 手表项目，除非后续目标明确变成“旧项目修复”。
- 不把 T-Watch S3 Plus、ESP32-S3、FT6336U、BMA423 或 AXP2101 宣布为最终选型。
- 不启动正式 BSP、完整 RTOS 任务划分、完整 QPC 或通用 service framework。
- 不为了“看起来更架构化”提前抽象没有第二个消费者的模块。

## 过度设计判断

本阶段采用保守抽象原则：

- 只有当一个抽象已经有两个真实消费者，或至少有“模拟器 + 参考硬件”两个明确映射点时，才考虑抽出独立模块。
- 只为已经出现的重复判断、状态归属冲突或迁移边界抽象，不为想象中的未来完整性抽象。
- 优先写清职责和数据流，再决定是否改代码。
- 先做一个可解释的小闭环，再扩展第二个领域。

典型例子：

- `DisplayPolicyRules` 已经有多个调用点，抽出纯规则层是合理的。
- `DisplayPolicyCoordinator` 目前仍不急，因为计时器、页面渲染、设置确认流还没有稳定到需要统一搬迁。
- `Battery / PowerStatus service` 值得作为第一批 service 候选，因为模拟器已有电池模型，T-Watch 已验证 PMU 基础状态，且页面不应长期解释硬件供电语义。

## 建议推进顺序

### 第一轮：阶段复盘与路线收口

目标：

- 新增本阶段入口文档。
- 更新 `docs/decision_log.md`，记录 v0.4 阶段决策。

验收：

- 新会话读完文档后能明确知道：不继续堆页面，不继续随机硬件实验，下一步看系统边界。

### 第二轮：当前模拟器系统边界审计

目标：

- 审计 `Application`、`InputIntentRouter`、`AppStateMachine`、`PageManager`、`DataCenter`、`DisplayPolicyRules`、`SimulatorDevice` 的职责。
- 明确输入意图、系统状态、页面栈、显示策略、平台采样、mock / service 数据的归属。

验收：

- 未来新增 Battery、Time、Sensor、Notification service 时，能判断代码应该进入哪个层。

输出：

- `docs/v0_4_simulator_system_boundary_audit.md`

### 第三轮：Platform / HAL 事件契约收口

目标：

- 对齐 T-Watch 已验证能力和当前 HAL 事件模型。
- 明确 `TouchSample`、`MotionSample`、`BatterySample`、`CrownSample` / 按键替代、screen off / wake 的映射边界。
- 优先文档化，只有发现现有类型明显表达不了时才扩展类型。

验收：

- 能说明模拟器输入和 T-Watch 输入分别映射到哪些高层 intent，哪些仍只是调试替代。

输出：

- `docs/v0_4_platform_hal_event_contract.md`

### 第四轮：选择第一个真实 service 边界

目标：

- 推荐先做 `Battery / PowerStatus service` 边界。
- 不做真实天气、计步、通知或健康服务。

验收：

- 页面只订阅模型。
- 平台层提供样本。
- service 层解释供电、电量和充电状态。
- 状态机只消费必要事件，不直接解释底层 PMU 细节。

输出：

- `docs/v0_4_battery_power_service_boundary.md`

## 验证方式

- 文档闭环的验证标准是：下一轮可直接执行，不留下路线空白。
- 涉及模拟器代码时，运行 `sim/lv_port_pc_vscode` 的构建命令。
- 涉及 shell、输入、显示策略或页面恢复时，按 `docs/simulator_manual_regression_matrix.md` 做目标回归。
- 涉及 HAL / 平台契约时，只做最小 T-Watch 对照验证，不恢复大规模硬件实验。

## 当前结论

Magic Watch 不应该回到“继续做更多页面”的惯性，也不应该被 T-Watch 前哨验证拖进板级实验。下一阶段最合适的主线是：让模拟器中的系统骨架、平台契约和第一批 service 边界变得可解释、可验证、可迁移。

## 阶段收口

v0.4 已完成当前目标并收口。

收口文档：

- `docs/v0_4_stage_closure.md`

下一阶段入口：

- `docs/v0_5_power_status_page_entry.md`
