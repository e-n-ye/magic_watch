# MAINLINE-DATA-01 动态电池最小闭环

本文件只登记阶段 1 的当前执行入口及四张执行拆分卡。
不得从本文件扩展阶段 2-4 卡片。

## DATA-01-A Core 最小 BatteryState

- 批次：`MAINLINE-DATA-01`
- 状态：DONE
- 依赖：无
- 自检：
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
  - `git diff --check`
  - Core 平台依赖与动态内存定向扫描
- 建议提交信息：`core: add minimal battery snapshot state`
- Doc Impact：`required`

### 问题定位

当前 `WatchCoreUiModelSnapshot` 只有健康卡文本，PC Simulator 的电池样本没有
进入当前 `watch_core` 主线。本卡只建立固定大小、纯 C 的最小电池状态合同。

### 实施方案

- 新增 `WatchCoreBatteryState`：`present`、`charging`、`percent`。
- `WatchCoreUiModelSnapshot` 增加 `battery`。
- 新增 `watch_core_set_battery_state()`。
- 初始状态为 absent。
- `present == true && percent <= 100` 时接受更新。
- `percent > 100` 时拒绝并保持旧状态。
- `present == false` 时规范化为 `charging=false, percent=0`。
- 合同测试只覆盖初始 absent、有效值表和非法值保持旧状态三组。

### 涉及位置

Allowed files

- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`
- `watch_core/tests/watch_core_contract_test.c`
- `docs/40_workflow/agent_batch/cards/mainline-data-q21.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `sim/lv_port_pc_vscode/**`
- `try/my_watch_f411_v2.1/**`
- `docs/00_current/magicwatch_capability_growth_roadmap.md`

Forbidden changes

- 不修改 PC/F411 Adapter、XML、generated C 或平台代码。
- 不新增 Battery 事件、Service、EventBus、Queue 或动态内存。
- 不重命名 `WatchCoreUiModelSnapshot`。
- 不开始 `DATA-01-B`。

### 风险

- 把 BatteryState 扩成完整 PMIC 模型。
- 用 clamp 掩盖非法采样。
- 把 UI 格式化文本存成 Core 权威状态。

### 验收标准

- BatteryState 可通过公开 API 写入并从 Snapshot 读取。
- 101% 被拒绝且旧状态不变。
- 原有合同测试继续通过。
- PC 与 F411 现有代码仍可编译。
- `watch_core` 继续无 LVGL、SDL、HAL 和动态内存依赖。

### 执行记录

- 2026-06-16：已完成 `watch_core` 最小 BatteryState 合同与合同测试。
- 实际改动：`watch_core/include/watch_core/watch_core.h`、
  `watch_core/src/watch_core.c`、`watch_core/tests/watch_core_contract_test.c`、
  `docs/40_workflow/agent_batch/cards/mainline-data-q21.md`、
  `docs/40_workflow/agent_batch/agent-progress.md`
- 自检：
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe` 通过
  - `git diff --check` 通过（仅 LF/CRLF 提示，无 diff 错误）
  - `rg -n "lvgl|SDL|HAL|CubeMX|malloc|free|new|delete|std::" watch_core` 仅命中
    既有测试名中的 `new` 子串，无平台依赖或动态内存命中
- 风险回应：只增加最小 `BatteryState` 共享合同与 Snapshot 读取，不扩成 PMIC
  模型，不使用 clamp，不修改 Adapter、XML、generated C、Simulator，不进入
  `DATA-01-B`。

## DATA-01-B Simulator Battery 进入 Core

- 批次：`MAINLINE-DATA-01`
- 状态：DONE
- 依赖：`DATA-01-A` DONE
- 自检：
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
  - 人工运行 `magic_watch_xml_sim`，观察初始 82% 与每 5 秒变化日志
  - `git diff --check`
- 建议提交信息：`pc: feed simulator battery into watch core`
- Doc Impact：`required`

### 问题定位

`SimulatorDevice::tick()` 已产生 BatteryChanged，但 XML 主线没有注册 callback。
本卡只证明 Simulator 样本能够进入 Core，不刷新 UI。

### 实施方案

- 在 `xml_sim_main.cpp` 注册现有 Device event callback。
- 只消费 `BatteryChanged`，其他事件显式忽略。
- 将 `BatterySample` 转成 `WatchCoreBatteryState` 并调用 Core setter。
- `dirty` 只能是 `xml_sim_main.cpp` 的局部状态。
- callback 更新 Core 后只设置 dirty，不调用 Adapter 或 LVGL。
- 主循环在 `device->tick()` 返回后消费 dirty：读取 Snapshot、打印一条电池日志、
  清除 dirty。

### 涉及位置

Allowed files

- `sim/lv_port_pc_vscode/src/xml_sim_main.cpp`
- `docs/40_workflow/agent_batch/cards/mainline-data-q21.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_core/**`
- `sim/lv_port_pc_vscode/src/hal/**`
- `sim/lv_port_pc_vscode/src/Platform/Simulator/**`
- `sim/lv_port_pc_vscode/src/XmlUi/**`
- `ui/lvgl_pro/**`

Forbidden changes

- 不修改 XML、generated C、Adapter 或 F411。
- 不让 callback 直接调用 LVGL。
- 不新增第二个 dirty 状态或消费点。
- 不新增 Queue、Service、EventBus 或线程同步抽象。
- 不开始 `DATA-01-C`。

### 风险

- 把当前同线程 callback 误写成通用线程安全结论。
- 顺手接入 Time、Activity、Touch、Crown 或 Debug 事件。
- 为一条日志建立通用 dispatcher。

### 验收标准

- 启动日志能读到初始 82%。
- 之后每 5 秒日志发生变化。
- 日志值来自最新 Core Snapshot。
- 原导航构建和 Core 合同测试不退化。

### 执行记录

- 2026-06-16：已完成最小 Battery callback -> Core -> 单一 dirty 日志链。
- 实际改动：`sim/lv_port_pc_vscode/src/xml_sim_main.cpp`、
  `docs/40_workflow/agent_batch/cards/mainline-data-q21.md`、
  `docs/40_workflow/agent_batch/agent-progress.md`
- 自检：
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe` 通过
  - `magic_watch_xml_sim` 短时运行日志先输出 `82`，约 5 秒后输出 `81`
  - `git diff --check` 通过（仅 LF/CRLF 提示，无 diff 错误）
- 风险回应：只消费 `BatteryChanged`，其他事件显式忽略；callback 只写 Core
  并设置局部 `dirty`；主循环保留单一日志消费点；未修改 Adapter、XML、
  generated C、F411，未进入 `DATA-01-C`。

## DATA-01-C XML 电池显示替换日志消费

- 批次：`MAINLINE-DATA-01`
- 状态：DONE
- 依赖：`DATA-01-B` DONE
- 自检：
  - 按 `lvgl-xml-workflow` 核对 XML 与本地参考
  - LVGL Editor preview 或正式生成检查
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
  - 人工回归动态电池、四卡、详情与 Back
  - `git diff --check`
- 建议提交信息：`pc: show dynamic battery snapshot in xml ui`
- Doc Impact：`required`

### 问题定位

`DATA-01-B` 只用日志消费 dirty。本卡将同一个消费点替换为正式 Adapter/UI 同步，
让动态电池在 XML 首页可见。

### 实施方案

- 在首页 XML 增加一个简单电池标签。
- 通过 LVGL Editor 正式生成 generated C，禁止手改生成文件。
- UI helper 增加稳定的电池标签访问入口。
- Adapter 增加电池文本 Subject，并公开
  `watch_core_ui_adapter_sync_snapshot()`。
- 该函数只同步 Snapshot 到 Subject，不创建、加载或重建 screen。
- `xml_sim_main.cpp` 原 dirty 消费点从日志改为调用该同步函数，不能保留第二个
  dirty 消费点。
- 首页初始化、重新创建或从详情返回首页时，必须从当前 Snapshot 同步一次。
- 显示规则：absent 为 `--%`，普通为 `82%`，充电为 `82% +`。

### 涉及位置

Allowed files

- `ui/lvgl_pro/screens/screen_health_shortcuts.xml`
- `ui/lvgl_pro/screens/screen_health_shortcuts_gen.c`
- `ui/lvgl_pro/screens/screen_health_shortcuts_gen.h`
- `ui/lvgl_pro/magic_watch_ui.c`
- `ui/lvgl_pro/magic_watch_ui.h`
- `sim/lv_port_pc_vscode/src/xml_sim_main.cpp`
- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.c`
- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.h`
- `docs/40_workflow/agent_batch/cards/mainline-data-q21.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_core/**`
- `sim/lv_port_pc_vscode/src/Platform/Simulator/**`
- `try/my_watch_f411_v2.1/**`
- `docs/00_current/magicwatch_capability_growth_roadmap.md`

Forbidden changes

- 不手改 generated C；它只能由 XML Editor 正式生成。
- 不用 Adapter 临时创建 label 绕过 XML。
- 不新增第二个 dirty 状态或消费点。
- 不让 Snapshot 同步重建 screen。
- 不新增页面、Service、EventBus、Queue 或 Power 行为。
- XML 生成阻塞时必须保留日志验证并标记 BLOCKED。

### 风险

- XML 工具链问题吞掉动态数据闭环。
- Adapter 同步函数顺手承担页面导航。
- 返回首页后显示初始化值而不是最新 Snapshot。

### 验收标准

- 首页显示动态电池且每 5 秒变化。
- 详情页期间 Core 继续更新，返回首页显示最新值。
- 电池同步不重建当前 screen。
- 四卡、详情和 Back 正常。
- generated C 与 XML 一致，且没有手改绕过。

### 执行记录

- 2026-06-16：执行前检查完成，依赖 `DATA-01-B` 已满足。
- 已读现状：`screen_health_shortcuts.xml`、`screen_health_shortcuts_gen.c/.h`、
  `magic_watch_ui.c/.h`、`watch_core_ui_adapter.c/.h`、`xml_sim_main.cpp`。
- 已按 `lvgl-xml-workflow` 核对本地参考：
  - `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\tutorials\6_data_binding\components\sliderbox.xml`
    证明 `lv_label` 可用 `bind_text` 绑定 subject。
  - `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\docs\cli.mdx`
    说明正式生成路径应为 `lved-cli.js generate <project-path>`。
- 2026-06-16：用户已提供可用 CLI 路径
  `D:\lvgl_pro_cli\LVGL_Pro_CLI-1.2.1-windows\lved-cli.js`，本卡恢复执行。
- 2026-06-16：已完成 XML 电池显示、正式生成、Adapter 同步链与主循环消费点替换。
- 实际改动：`ui/lvgl_pro/screens/screen_health_shortcuts.xml`、
  `ui/lvgl_pro/screens/screen_health_shortcuts_gen.c`、
  `ui/lvgl_pro/magic_watch_ui.c`、`ui/lvgl_pro/magic_watch_ui.h`、
  `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.c`、
  `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.h`、
  `sim/lv_port_pc_vscode/src/xml_sim_main.cpp`、
  `docs/40_workflow/agent_batch/cards/mainline-data-q21.md`、
  `docs/40_workflow/agent_batch/agent-progress.md`
- 自检：
  - `node D:\lvgl_pro_cli\LVGL_Pro_CLI-1.2.1-windows\lved-cli.js generate D:\MY_Desk\watch\magic_watch\ui\lvgl_pro` 通过
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe` 通过
  - `magic_watch_xml_sim.exe` 7 秒运行态冒烟通过
  - 用户人工确认：首页电池动态、点 Heart 进详情、Back 返回首页且电池继续更新
  - `git diff --check` 通过（仅 LF/CRLF 提示，无 diff 错误）
- 风险回应：generated C 仅通过 CLI 正式生成；未手改 `_gen.c/.h`；同步函数只更新
  subject，不重建 screen；`xml_sim_main.cpp` 只保留一个 dirty 消费点；未新增
  页面、Service、EventBus、Queue 或 Power 行为。

## DATA-01-D 最小回归、演示和记录

- 批次：`MAINLINE-DATA-01`
- 状态：TODO
- 依赖：`DATA-01-C` DONE
- 自检：
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
  - PC 人工回归动态电池、四卡、详情与 Back
  - `git diff --check`
  - 本轮中文文档乱码哨兵检查
- 建议提交信息：`docs: close dynamic battery mainline slice`
- Doc Impact：`required`

### 问题定位

动态电池链需要以可运行证据和简短事实记录收口，不能继续扩成新的架构论文。

### 实施方案

- 记录动态电池完整调用链和最小人工回归结果。
- 在现有 PC 架构审计中补充已接通事实。
- 更新队列和进度状态。
- 记录一条三分钟演示路径或录屏位置；没有录制时如实标记未执行。
- F411 Keil 编译和真机回归只记录为未执行，不作为本卡阻塞项。

### 涉及位置

Allowed files

- `docs/30_testing/current_watch_architecture_map_and_necessity_audit.md`
- `docs/00_current/magicwatch_capability_growth_roadmap.md`
- `docs/40_workflow/agent_batch/cards/mainline-data-q21.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `sim/lv_port_pc_vscode/README.md`

Read-only files

- `watch_core/**`
- `sim/lv_port_pc_vscode/src/**`
- `ui/lvgl_pro/**`
- `try/my_watch_f411_v2.1/**`

Forbidden changes

- 不修改产品代码。
- 不开始 PC Power executor。
- 不创建阶段 2-4 卡片。
- 不把未执行的 F411 或真机检查写成通过。
- 不新增长篇阶段报告。

### 风险

- 把构建通过写成手动 UI 通过。
- 收口时扩写无关路线或整理 docs。
- 录屏未完成却把阶段写成已有演示证据。

### 验收标准

- PC 构建和 Core 合同测试通过。
- 动态电池、四卡、详情和 Back 已人工回归。
- 调用链与当前代码一致。
- 未验证项如实保留。
- `MAINLINE-DATA-01` 完成后停止，等待用户决定是否规划 PC Power executor。

### 执行记录

- 尚未执行。
