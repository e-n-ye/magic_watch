# Watch Architecture Learning Stage 01

## 卡片 ARCH-LAB-01 Console 直接调用基线

- ID：`ARCH-LAB-01`
- 标题：Console 直接调用基线
- 批次：架构冲突实验场
- 状态：DONE
- 依赖：`ARCH-LAB-00` DONE，提交 `c32957b`

### 问题定位

Stage 00 暴露出用户尚未形成清晰 UI 系统蓝图，同时 LVGL API、触摸激活和 View 对象会分散对状态所有权的注意力。本卡移除 LVGL 壳层，用终端命令重新实现相同行为，观察显式页面状态、命令决策和输出表现如何连接。

### 实施方案

1. 新增不依赖 LVGL/SDL 的 Console-only 构建 preset。
2. 建立 Stage 01 标准输入循环与命令帮助。
3. 保留 `page_t`、`handle_command()`、`render_current_page()` 三个用户练习区域。
4. 用户填写事前预测并亲自接通 Heart、Steps、Back、Show。
5. 运行后记录状态所有权、直接调用收益和实际修改压力。

### 涉及位置

Allowed files

- `watch_arch_learning/CMakeLists.txt`
- `watch_arch_learning/CMakePresets.json`
- `watch_arch_learning/README.md`
- `watch_arch_learning/stage_01_console_direct/**`
- `docs/20_guides/watch_arch_learning_lab.md`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage01-q14.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_arch_learning/stage_00_direct/**`

Forbidden changes

- 不修改或复制 Stage 00 实现。
- 不修改 Magic Watch 产品代码、PC XML UI、F411 或 `watch_core`。
- 不引入 LVGL、SDL、真实串口、Intent、Core、Renderer 接口、EventQueue、PageManager 或页面生命周期。
- 不创建 Stage 02 或后续目录和卡片。
- 不由 Agent 完成三个用户 `LEARN` 区域。

### 风险

- Console 版本会移除触摸与 View 对象压力，不能用它证明最终产品不需要 View 层。
- 如果一开始拆成 Input/Core/Renderer，会再次变成复刻预设架构。
- 当前终端命令只模拟输入，不代表真实 UART 的缓冲、并发和错误模型。

### 自检

- `cmake --preset console-debug`
- `cmake --build --preset console-debug`
- 运行程序确认输入循环、`help`、`quit` 和练习占位可达
- `git diff --check`
- 扫描 Stage 01 是否引用 LVGL、SDL 和预设系统抽象
- 对本轮中文 Markdown 做乱码哨兵检查

### 验收标准

- Console-only 配置不查找 SDL，不构建 LVGL。
- 工程可独立构建运行。
- 输入循环、`help`、`quit` 正常。
- 三个用户练习区域明确且尚未被 Agent 完成。
- README 已提供事前预测和行为问题。
- 没有提前建立系统级抽象。

### Doc Impact

`required`

### 建议提交信息

`learn: scaffold console direct stage`

### 执行记录

- 2026-06-14：用户确认从 Stage 01 开始采用无 LVGL 的 Console 直接实现；本轮只建立独立构建与用户练习骨架。
- 2026-06-14：`console-debug` 配置与构建通过，构建树未登记 SDL、LVGL、Stage 00 或 runtime；短运行验证 `help`、输入循环、练习占位和 `quit` 均可达。三个 `LEARN` 区域仍未实现，等待用户完成。
- 2026-06-14：用户完成 `page_t`、`handle_command()` 和 `render_current_page()`；Heart、Steps、Back、Show 与非法命令均实际运行。首页 Back 已明确为合法 no-op，页面输出重复已收敛到单一 `render_current_page()`。Stage 01 完成复盘并冻结。

### Stop policy

- 基础骨架构建运行后停止，等待用户填写事前预测和三个 `LEARN` 区域。
- 用户练习验收后冻结并停止，不自动实现 Stage 02。
