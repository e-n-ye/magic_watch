# H9-Q2 阶段 9 真实硬件桥接代码批次

本文件承载阶段 9 第一轮真实硬件桥接代码卡片。执行顺序以 `agent-queue.md` 为准。

---

## H9-BRIDGE-2A T-Watch AXP2101 底层读数闭环

- 批次：H9-Q2
- 状态：DONE
- 依赖：无
- 自检：
  - `git status --short -uall`
  - `pio run -e twatch-s3 -j 1`
  - `git diff --check`
- 建议提交信息：`feat: stabilize twatch battery baseline readout`
- Doc Impact：required

### 问题定位

阶段 9 不能从 PC replay 开始。第一步必须先确认 T-Watch S3 Plus 上已有 prototype 能稳定读取 AXP2101 / BatteryPowerStatus，并把后续架构下放所需的最小观测量跑出来。

### 实施方案

- 在 `prototypes/twatch_s3_plus_bringup` 中稳定读取 AXP2101 电池状态。
- 串口输出电池电压、电量百分比、充电状态、外部供电状态与 `free_heap`。
- 若现有 prototype 输出缺项，优先补齐观测字段，不提前引入 Magic Watch 上层架构。
- 同步最小必要文档，记录这一步只证明板级读数闭环，不等于架构子集已下放。

### 涉及位置

Allowed files:

- `prototypes/twatch_s3_plus_bringup/src/**`
- `prototypes/twatch_s3_plus_bringup/platformio.ini`
- `prototypes/twatch_s3_plus_bringup/README.md`
- `docs/70_hardware_reference/stage9_hardware_bridge_plan.md`
- `docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `docs/10_architecture/hardware_boundary.md`
- `docs/00_current/current_architecture.md`
- `sim/lv_port_pc_vscode/src/hal/**`
- `sim/lv_port_pc_vscode/src/App/Services/BatteryPowerService.*`
- `sim/lv_port_pc_vscode/src/App/Common/DataCenter.*`
- `sim/lv_port_pc_vscode/src/App/Common/EventBus.*`
- `docs/70_hardware_reference/twatch_s3_plus_reference_validation_run.md`

Forbidden changes:

- 禁止修改 `sim/**`。
- 禁止引入完整 Magic Watch UI。
- 禁止把串口 PMU log 描述成阶段 9 硬件闭环完成。
- 禁止为了快速通过而改写阶段 9 验收口径。

### 风险

- 如果读数仍不稳定，后续架构下放会在错误数据源上继续叠复杂度。
- 如果本轮提前混入 `DataCenter` / `EventBus`，容易把底层 bring-up 问题和架构问题混成一轮。

### 验收标准

- `pio run -e twatch-s3 -j 1` 通过。
- 串口输出稳定包含电压、百分比、充电、外部供电、`free_heap`。
- 文档明确这一步只是板级读数闭环，不是最终阶段 9 闭环。

### 执行记录

- 完成时间：2026-06-04 17:32
- 实际改动文件：`prototypes/twatch_s3_plus_bringup/src/main.cpp`、`prototypes/twatch_s3_plus_bringup/README.md`、`docs/70_hardware_reference/stage9_hardware_bridge_plan.md`、`docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`、`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`
- 自检结果：`git status --short -uall` 仅出现本轮允许文件；`C:\Users\13984\.platformio\penv\Scripts\pio.exe run -e twatch-s3 -j 1` 两次通过；`git diff --check` 通过（仅 LF/CRLF 提示，无 diff 格式错误）；本轮实际改动中文文档乱码哨兵检查通过
- 提交状态：待提交
- 风险回应：本轮只确认 T-Watch S3 Plus 板级 Battery / PowerStatus 读数与资源观测基线，不把现有 bring-up 日志误写成最小架构子集已下放；`H9-BRIDGE-2B` 之后才进入 `BatterySample` / `BatteryPowerService` / `DataCenter` / simplified `EventBus`。

---

## H9-BRIDGE-2B 最小架构子集下放

- 批次：H9-Q2
- 状态：DONE
- 依赖：`H9-BRIDGE-2A`
- 自检：
  - `git status --short -uall`
  - `pio run -e twatch-s3 -j 1`
  - `git diff --check`
- 建议提交信息：`feat: port minimal battery architecture slice to twatch`
- Doc Impact：required

### 问题定位

只会读 PMU 数据还不够。阶段 9 的核心验收是 Magic Watch 的最小架构子集能下放到真实 MCU，而不是只在 PC 上保留 `hal::BatterySample`、`BatteryPowerService`、`DataCenter` 与 `EventBus`。

### 实施方案

- 以 `Battery / PowerStatus` 为唯一垂直切片，把最小化的 `hal::BatterySample`、`BatteryPowerService`、`DataCenter` 与简化 `EventBus` 移植进 T-Watch prototype。
- 只保留完成本切片所需的最小类型和接口，不追求一步复用完整模拟器实现。
- 记录并修复 Xtensa / Arduino / FreeRTOS 下的编译差异。
- 同步最小必要文档，说明哪些类型是“兼容下放子集”，哪些 PC-only 依赖仍保留在模拟器。

### 涉及位置

Allowed files:

- `prototypes/twatch_s3_plus_bringup/src/**`
- `prototypes/twatch_s3_plus_bringup/platformio.ini`
- `prototypes/twatch_s3_plus_bringup/README.md`
- `docs/70_hardware_reference/stage9_hardware_bridge_plan.md`
- `docs/00_current/current_architecture.md`
- `docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `docs/10_architecture/hardware_boundary.md`
- `sim/lv_port_pc_vscode/src/hal/**`
- `sim/lv_port_pc_vscode/src/App/Services/BatteryPowerService.*`
- `sim/lv_port_pc_vscode/src/App/Common/DataCenter.*`
- `sim/lv_port_pc_vscode/src/App/Common/EventBus.*`
- `sim/lv_port_pc_vscode/src/App/State/PowerController.*`

Forbidden changes:

- 禁止修改 `sim/**` 以迁就 prototype。
- 禁止复用完整 UI、页面、控制器树。
- 禁止把 `std::function` / `std::vector` / `std::string` 的适配问题留成隐性债务而不记录。
- 禁止直接宣称“Magic Watch 已移植到真机”。

### 风险

- 如果直接照搬模拟器实现，可能会把动态分配、同步分发和 PC-only 依赖一并带入 MCU。
- 如果没有记录编译差异，后续同类服务下放会重复踩坑。

### 验收标准

- prototype 中存在可编译的最小 `BatterySample` / `BatteryPowerService` / `DataCenter` / simplified `EventBus` 子集。
- `pio run -e twatch-s3 -j 1` 通过。
- 文档明确哪些实现是“本轮最小兼容子集”，不是完整复用证明。

### 执行记录

- 完成时间：2026-06-04 18:05
- 实际改动文件：`prototypes/twatch_s3_plus_bringup/src/main.cpp`、`prototypes/twatch_s3_plus_bringup/src/mwbridge/BatteryTypes.h`、`prototypes/twatch_s3_plus_bringup/src/mwbridge/EventBus.h`、`prototypes/twatch_s3_plus_bringup/src/mwbridge/EventBus.cpp`、`prototypes/twatch_s3_plus_bringup/src/mwbridge/DataCenter.h`、`prototypes/twatch_s3_plus_bringup/src/mwbridge/DataCenter.cpp`、`prototypes/twatch_s3_plus_bringup/src/mwbridge/BatteryPowerService.h`、`prototypes/twatch_s3_plus_bringup/src/mwbridge/BatteryPowerService.cpp`、`prototypes/twatch_s3_plus_bringup/README.md`、`docs/70_hardware_reference/stage9_hardware_bridge_plan.md`、`docs/00_current/current_architecture.md`、`docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`、`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`
- 自检结果：`git status --short -uall` 仅出现本轮允许文件；`C:\Users\13984\.platformio\penv\Scripts\pio.exe run -e twatch-s3 -j 1` 通过；`git diff --check` 通过（仅 LF/CRLF 提示，无 diff 格式错误）；本轮实际改动中文文档乱码哨兵检查通过
- 提交状态：待提交
- 风险回应：本轮下放的是 Battery-only 最小兼容子集，不是模拟器全量 `DataCenter` / `EventBus`；当前 `EventBus` 采用固定槽位和函数指针回调，`DataCenter` 只保留 `BatteryChanged` 最后快照，`BatteryPowerService` 只负责样本转模型和同步发布，真正 `Power_Task` 化和串口事件观测继续留给 `H9-BRIDGE-2C` / `H9-BRIDGE-2D`。

---

## H9-BRIDGE-2C 真实 FreeRTOS Power_Task 闭环

- 批次：H9-Q2
- 状态：DONE
- 依赖：`H9-BRIDGE-2B`
- 自检：
  - `git status --short -uall`
  - `pio run -e twatch-s3 -j 1`
  - `git diff --check`
- 建议提交信息：`feat: run battery power task on twatch`
- Doc Impact：required

### 问题定位

阶段 9 需要回答的不只是“代码能编译”，还包括最小架构子集是否真的在 FreeRTOS 任务上下文中按节奏运行并把样本推入板载数据中心。

### 实施方案

- 在 T-Watch prototype 中创建真实 FreeRTOS `Power_Task`。
- 以 1Hz 轮询 AXP2101，把读数封装为 `BatterySample` 并写入板载 `DataCenter`。
- 保持任务职责单一，不同时扩展到其它传感器或 UI 驱动。
- 同步最小必要文档，说明线程归属、轮询频率和当前仍采用的简化同步边界。

### 涉及位置

Allowed files:

- `prototypes/twatch_s3_plus_bringup/src/**`
- `prototypes/twatch_s3_plus_bringup/platformio.ini`
- `prototypes/twatch_s3_plus_bringup/README.md`
- `docs/70_hardware_reference/stage9_hardware_bridge_plan.md`
- `docs/10_architecture/hardware_boundary.md`
- `docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `docs/00_current/current_architecture.md`
- `sim/lv_port_pc_vscode/src/App/Common/DataCenter.*`
- `sim/lv_port_pc_vscode/src/App/Common/EventBus.*`
- `sim/lv_port_pc_vscode/src/App/Services/BatteryPowerService.*`

Forbidden changes:

- 禁止接入完整 UI 或页面刷新链路。
- 禁止同时引入其它传感器任务。
- 禁止把 1Hz 任务轮询描述成低功耗已经验证完成。

### 风险

- 如果任务栈、频率或生命周期边界不清，后面观测到的事件结果无法归因。
- 如果本轮顺手接入其它传感器，会让问题域膨胀到超出阶段 9 第一闭环。

### 验收标准

- `Power_Task` 在真机固件中以 1Hz 运行。
- `BatterySample` 能从任务流入板载 `DataCenter`。
- `pio run -e twatch-s3 -j 1` 通过。
- 文档明确当前线程/同步边界仍是阶段性简化实现。

### 执行记录

- 完成时间：2026-06-04 18:40
- 实际改动文件：`prototypes/twatch_s3_plus_bringup/src/main.cpp`、`prototypes/twatch_s3_plus_bringup/README.md`、`docs/70_hardware_reference/stage9_hardware_bridge_plan.md`、`docs/10_architecture/hardware_boundary.md`、`docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`、`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`
- 自检结果：`git status --short -uall` 仅出现本轮允许文件；`C:\Users\13984\.platformio\penv\Scripts\pio.exe run -e twatch-s3 -j 1` 通过；`git diff --check` 通过（仅 LF/CRLF 提示，无 diff 格式错误）；本轮实际改动中文文档乱码哨兵检查通过
- 提交状态：待提交
- 风险回应：本轮已把 AXP2101 周期采样迁入真实同核 FreeRTOS `Power_Task`，并让页面/PMU 日志改读任务共享快照；但当前仍是“同核任务 + 同步 EventBus + 简化共享快照”边界，不代表已经证明通用多任务队列或串口 BatteryChanged 观测闭环，后者继续留给 `H9-BRIDGE-2D`。

---

## H9-BRIDGE-2D 串口 BatteryChanged 事件观测

- 批次：H9-Q2
- 状态：DONE
- 依赖：`H9-BRIDGE-2C`
- 自检：
  - `git status --short -uall`
  - `pio run -e twatch-s3 -j 1`
  - `git diff --check`
- 建议提交信息：`feat: observe battery events on twatch serial`
- Doc Impact：required

### 问题定位

前面三张卡解决的是“读到数据”和“任务在跑”。真正接近阶段 9 第一验收线的是：板载 `EventBus` 能把 `BatteryChanged` 或等价事件发出来，并让串口看到模型快照与资源观测指标。

### 实施方案

- 让 prototype 串口订阅或观察 `BatteryChanged` / 等价电池变化事件。
- 输出当前电池模型、`free_heap` 与 `Power_Task` high water mark。
- 若事件模型与模拟器不同，必须在文档中明确差异，而不是隐性偏离。
- 保持观测链路只用于验证架构闭环，不提前扩成通用日志系统。

### 涉及位置

Allowed files:

- `prototypes/twatch_s3_plus_bringup/src/**`
- `prototypes/twatch_s3_plus_bringup/platformio.ini`
- `prototypes/twatch_s3_plus_bringup/README.md`
- `docs/70_hardware_reference/stage9_hardware_bridge_plan.md`
- `docs/00_current/current_architecture.md`
- `docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `docs/10_architecture/hardware_boundary.md`
- `sim/lv_port_pc_vscode/src/App/Common/EventBus.*`
- `sim/lv_port_pc_vscode/src/App/Common/DataCenter.*`
- `sim/lv_port_pc_vscode/src/App/Services/BatteryPowerService.*`

Forbidden changes:

- 禁止把串口观测扩展成新的通用调试框架。
- 禁止以 PC replay 替代真机事件观测。
- 禁止把 `free_heap` / high water mark 缺失的结果写成已达标。

### 风险

- 如果事件观察只是打印原始 PMU 数据，仍然没有证明板载架构子集在发事件。
- 如果缺少 heap / stack 指标，后续无法判断最小架构是否具备继续下放的资源余量。

### 验收标准

- 真机串口能看到板载 `EventBus` 发出的 `BatteryChanged` 或等价事件。
- 输出中包含电池模型、`free_heap`、`Power_Task` high water mark。
- `pio run -e twatch-s3 -j 1` 通过。
- 文档明确未验证项仍包括完整 UI 真机移植、真实低功耗电流、触摸 / BMA / 心率 / 血氧接入。

### 执行记录

- 完成时间：2026-06-04 19:12
- 实际改动文件：`prototypes/twatch_s3_plus_bringup/src/main.cpp`、`prototypes/twatch_s3_plus_bringup/README.md`、`docs/70_hardware_reference/stage9_hardware_bridge_plan.md`、`docs/00_current/current_architecture.md`、`docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`、`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`
- 自检结果：`git status --short -uall` 仅出现本轮允许文件；`C:\Users\13984\.platformio\penv\Scripts\pio.exe run -e twatch-s3 -j 1` 先因重复定义失败，清理重复块后通过；`git diff --check` 通过（仅 LF/CRLF 提示，无 diff 格式错误）；本轮实际改动中文文档乱码哨兵检查通过
- 提交状态：待提交
- 风险回应：本轮已新增真正的 `BatteryChanged` 串口观察回调，并输出电池模型、`free_heap`、`Power_Task` high water mark；`[bringup-pmu]` 只保留为板级采样摘要并增加 `bridge_evt` 对照字段，不再被误写成事件闭环本身。完整 UI 真机移植、真实低功耗电流、触摸 / BMA / 心率 / 血氧接入仍未验证。

---

## H9-BRIDGE-2E 复用边界记录

- 批次：H9-Q2
- 状态：DONE
- 依赖：`H9-BRIDGE-2D`
- 自检：
  - `git status --short -uall`
  - `git diff --check`
  - `rg "std::function|std::vector|std::string|EventBus|UI" docs/70_hardware_reference/stage9_hardware_bridge_plan.md docs/10_architecture/hardware_boundary.md`
- 建议提交信息：`docs: record twatch bridge reuse boundaries`
- Doc Impact：required

### 问题定位

阶段 9 第一闭环完成后，如果不把“哪些上层代码可直接复用、哪些暂不能下放”明确记录下来，后续窗口仍容易重新滑回“全量移植 UI”或“为了过编译去修改模拟器”的错误方向。

### 实施方案

- 基于 `H9-BRIDGE-2A` 到 `H9-BRIDGE-2D` 的实际结果，记录哪些 Magic Watch 上层代码可以直接复用。
- 记录哪些代码因 `std::function`、`std::vector`、`std::string`、同步 `EventBus` 或 UI 依赖暂不能直接下放。
- 将复用边界沉入阶段 9 主入口或相关架构文档，作为后续桥接卡的前置事实。
- 不在本轮直接解决所有不可复用项，只做证据化记录和后续路由。

### 涉及位置

Allowed files:

- `docs/70_hardware_reference/stage9_hardware_bridge_plan.md`
- `docs/10_architecture/hardware_boundary.md`
- `docs/00_current/current_architecture.md`
- `docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `prototypes/twatch_s3_plus_bringup/src/**`
- `sim/lv_port_pc_vscode/src/hal/**`
- `sim/lv_port_pc_vscode/src/App/Common/**`
- `sim/lv_port_pc_vscode/src/App/Services/**`
- `sim/lv_port_pc_vscode/src/App/UI/**`

Forbidden changes:

- 禁止修改 prototype 或 `sim/**` 代码来“顺带修掉”记录中的问题。
- 禁止把复用边界文档写成完整移植承诺。
- 禁止省略暂不能下放的原因。

### 风险

- 如果只写“能复用很多”，却不点明受限点，后续窗口会再次高估直接下放比例。
- 如果把复用边界和下一轮实现混在一起，本轮就无法作为稳定的路线约束。

### 验收标准

- 文档明确列出可直接复用项与暂不能直接下放项。
- `std::function`、`std::vector`、`std::string`、同步 `EventBus`、UI 依赖是否构成阻碍都有明确说明。
- 阶段 9 后续路线不再把“全量 UI 迁移”误认为下一步。

### 执行记录

- 完成时间：2026-06-04 19:30
- 实际改动文件：`docs/70_hardware_reference/stage9_hardware_bridge_plan.md`、`docs/10_architecture/hardware_boundary.md`、`docs/00_current/current_architecture.md`、`docs/40_workflow/agent_batch/cards/h9-hardware-bridge-q2.md`、`docs/40_workflow/agent_batch/agent-queue.md`、`docs/40_workflow/agent_batch/agent-progress.md`
- 自检结果：`git status --short -uall` 仅出现本轮允许文件；`git diff --check` 通过；`rg "std::function|std::vector|std::string|EventBus|UI" docs/70_hardware_reference/stage9_hardware_bridge_plan.md docs/10_architecture/hardware_boundary.md` 命中均处于复用边界说明语境；本轮实际改动中文文档乱码哨兵检查通过
- 提交状态：待提交
- 风险回应：本轮只把“哪些概念能复用、哪些实现不能直接整搬”沉入文档，不修改 prototype 或 `sim/**` 代码；后续若继续桥接，应优先复用 Battery 数据形状和职责链，谨慎适配 `std::function` / `std::vector` / `std::string`、全量 `DataCenter`、通知中心副作用和 UI 依赖，而不是回改模拟器实现来迁就 MCU。
