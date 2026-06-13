# Watch Architecture Learning Stage 00

## 卡片 ARCH-LAB-00 直接调用基线

- ID：`ARCH-LAB-00`
- 标题：直接调用基线
- 批次：架构冲突实验场
- 状态：DONE
- 依赖：架构冲突实验场总纲 `cfca90c`

### 问题定位

在只有两张卡、单 View、鼠标输入和同步执行时，直接 LVGL callback 是否已经足够。当前不能预设需要事件、队列、Coordinator 或 Adapter。

### 实施方案

1. Agent 建立独立 PC LVGL + SDL runtime 和 Stage 00 基础 UI。
2. 用户填写事前预测。
3. 用户亲自接通 `show_home()`、`show_detail()` 和三个 callback。
4. 构建并实际运行 Heart、Steps、Back。
5. 记录实际修改、状态所有权和是否出现真实维护压力。
6. 满足停止门后冻结 Stage 00。

### 涉及位置

Allowed files

- `watch_arch_learning/**`
- `docs/20_guides/watch_arch_learning_lab.md`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage00-q13.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `sim/lv_port_pc_vscode/lvgl/**`
- `sim/lv_port_pc_vscode/lv_conf.h`

Forbidden changes

- 不修改 Magic Watch 产品代码、PC XML UI、F411 或 `watch_core`。
- 不引入 EventQueue、Coordinator、Adapter、PageState、PageIntent、Snapshot 或 XML。
- 不创建 Stage 01 或后续目录、卡片和预留接口。
- 不由 Agent 代写用户负责的五个直接行为函数。

### 自检

- `cmake --preset mingw-debug`
- `cmake --build --preset mingw-debug`
- `git diff --check`
- 检查产品代码无修改
- 对本轮中文 Markdown 做乱码哨兵检查

### 验收标准

- 基础工程可配置和构建。
- 初始首页能显示 Heart、Steps 两张卡。
- 用户完成五个直接行为函数后，两张卡和 Back 可交互。
- README 记录事前预测和实际观察。
- 没有系统级抽象和后续 Stage 预留。

### Doc Impact

`required`

### 建议提交信息

`learn: add direct lvgl stage zero`

### 执行记录

- 2026-06-13：进入 Stage 00；Agent 创建 runtime、基础 UI 和用户练习位置，等待配置构建与用户接通直接行为。
- 2026-06-13：`mingw-debug` preset 配置和构建通过；可执行文件短暂启动后进程正常存活。首次尝试直接复用完整 PC `lv_conf.h` 暴露无关 FS/ThorVG/demo 依赖，已改为实验场独立最小配置。五个直接行为函数仍保持 `LEARN` 空实现，等待用户完成。
- 2026-06-13：用户已接通五个直接行为函数并人工验证 Heart、Steps、Back。事前预测显示用户倾向提前假设全局页面状态与 `current_feature`，而当前行为没有读取这些持久状态；已在 Stage README 中记录预测与事实差异。人工验证同时发现 pointer 按下后拖走再松开仍可能激活，当前归类为输入激活策略问题，并提供接受默认语义、View 内局部 tap guard、通用输入层三种不同抽象半径，等待用户先作选择。
- 2026-06-13：用户选择方案 B。当前只在 Stage 00 View 内为 Heart、Steps、Back 增加局部 tap guard，使用按下点、移动阈值和 `PRESS_LOST` 过滤激活，不提前建立通用输入策略层。
- 2026-06-13：局部 tap guard 编译通过；等待用户人工验证正常轻点、轻微抖动、按钮内明显拖动和拖出按钮四类行为。
- 2026-06-13：用户指出首版按控件分配三份手势状态与单 pointer 并发事实不匹配；保留方案 B 边界，将动态状态收敛为一个 View 级 tap guard，各控件只保留各自页面 callback。
- 2026-06-13：用户最终确认正常轻点、少量移动、明显拖动和拖出松开行为均符合预期。复盘保留两项未消失事实：当前 callback 可能漏接 `tap_guard_accept()`；用户尚未形成完整 UI 系统蓝图。当前结论是冻结直接调用与局部 guard，延迟更大输入层和页面状态抽象。

### Stop policy

- 基础框架构建后停止，由用户完成事前预测和五个直接行为函数；未实际运行前不得冻结 Stage 00。
- Stage 00 完成后冻结并停止，不自动创建 Stage 01。
