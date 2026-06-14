# Watch Architecture Learning Stage 02

## 卡片 ARCH-LAB-02 上下文相关菜单导航

- ID：`ARCH-LAB-02`
- 标题：上下文相关菜单导航
- 批次：架构冲突实验场
- 状态：TODO
- 依赖：`ARCH-LAB-01` DONE，提交 `6ce2806`

### 问题定位

Stage 01 只有一个页面状态和全局命令，`handle_command()` 虽然职责集中，但尚未出现必须依据页面上下文解释输入的压力。

Stage 02 不通过机械增加十个页面制造代码量，而是增加一个应用菜单和两条进入详情的路径，观察：

- 相同命令在不同页面是否具有不同语义。
- 只保存 `current_page` 是否还能描述返回目标。
- 菜单选择状态与页面状态如何配合。
- 直接 `if/switch` 何时从简单写法变成难以核对的导航规则。

### 新增需求

在复制并保留 Stage 01 行为的基础上新增：

- `menu`：从 Home 进入 App Menu。
- App Menu 包含 Heart、Steps、About 三项。
- `next` / `prev`：只在 App Menu 中移动选中项，首尾循环。
- `enter`：从 App Menu 进入当前选中的详情页。
- 保留 `heart` / `steps` 从 Home 直接进入详情的快捷命令。
- 从菜单进入详情后，`back` 返回 App Menu，并保留原选中项。
- 从 Home 快捷命令进入详情后，`back` 返回 Home。
- App Menu 中 `back` 返回 Home。
- Home 中 `back` 仍是合法 no-op。
- `show` 只重新输出当前页面和必要的菜单选择，不修改状态。

### 当前要面对的挑战

1. `back` 不再只由当前详情页决定，还受到进入路径影响。
2. `next`、`prev`、`enter` 是合法命令，但只在 App Menu 中有意义。
3. 同一个 Heart/Steps 详情存在快捷入口和菜单入口，不能依靠页面类型推断来源。
4. 页面状态与菜单选中项是两个不同事实，但它们是否都应长期保存需要由行为证明。
5. `handle_command()` 可能同时出现“按命令分支”和“按页面分支”两种组织方式，需要比较哪一种更容易核对。
6. 输出内容开始依赖 `current_page + selected_app`，但这仍不自动证明需要 Renderer 接口。

### 实施方案

本卡分两个停止点，不能一轮完成：

1. 用户先填写状态预测和导航表，不写代码。
2. 预测确认后，复制冻结的 Stage 01 为独立 Stage 02 工程，继续使用单文件直接结构完成需求。

第一版实现必须故意保留：

- 单个 `main.c`。
- 字符串命令直接判断。
- 全局静态状态。
- `handle_command()` 直接修改状态。
- `render_current_page()` 直接打印。

只有实际运行后，才能比较保持现状、局部 helper、显式动作语义或独立渲染边界。

### 用户事前预测

实现前必须回答：

- 仅有 `current_page` 能否支持两种详情入口？如果不能，还缺什么事实？
- 菜单选中项离开菜单后是否需要保留？
- `back` 的规则应该按命令集中写，还是分散到各页面处理中？
- 合法但当前页面不可用的命令应当 no-op、报错还是提示不可用？
- 增加 About 后，哪些位置必须修改？
- 预计最容易漏掉哪一条导航规则？
- 哪种实际重复出现后，才考虑 Intent？
- 哪种实际重复出现后，才考虑 Renderer 边界？

### 涉及位置

Allowed files

- `watch_arch_learning/CMakeLists.txt`
- `watch_arch_learning/README.md`
- `watch_arch_learning/stage_02_context_menu/**`
- `docs/20_guides/watch_arch_learning_lab.md`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage02-q15.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_arch_learning/stage_00_direct/**`
- `watch_arch_learning/stage_01_console_direct/**`

Forbidden changes

- 不修改 Stage 00 或 Stage 01。
- 不修改 Magic Watch 产品代码、PC XML UI、F411 或 `watch_core`。
- 不引入第二种输入、异步事件、线程、真实串口、LVGL 或 SDL。
- 不引入 Intent、Core、Renderer 接口、EventQueue、PageManager、页面栈或生命周期框架。
- 不创建 Stage 03 或后续目录和卡片。
- 在用户完成事前预测前，不创建 Stage 02 代码目录。

### 为什么不能同时加入第二输入

菜单导航已经会同时改变页面上下文、返回规则和选择状态。如果再加入按键或第二套命令映射，将无法判断后续压力来自“导航组合”还是“多输入语义重复”。第二输入必须留到独立 Stage。

### 风险

- 需求故意保留两种详情入口，可能暴露“来源状态”问题，但不得直接预设页面栈是答案。
- 单文件会明显增长，但文件长度本身不是引入系统抽象的充分证据。
- 如果 Agent 提前提供状态结构或转移表答案，会污染用户判断。

### 自检

- 规划轮执行 `git diff --check`。
- 检查未创建 `stage_02_context_menu/`。
- 检查卡片只定义一个主要压力：上下文相关导航。
- 对本轮中文 Markdown 做乱码哨兵检查。

实现轮另需：

- Console-only 配置与构建通过。
- 命令序列覆盖两种详情入口、菜单选择保持、各页面 Back、非法命令与 `show`。
- 记录实际修改位置和导航规则遗漏。

### 验收标准

规划轮：

- 新需求、状态问题和导航挑战明确。
- 用户事前预测问题完整。
- 未实现代码，未提前指定架构答案。

实现轮：

- 所有命令行为实际运行。
- 能指出页面状态、菜单选择和进入来源分别由谁拥有。
- 能根据真实代码判断 `handle_command()` 的压力来自代码长度还是规则组合。
- 明确保留、局部整理或进一步抽象的决定。

### Doc Impact

`required`

### 建议提交信息

规划：`docs: plan contextual console navigation stage`

实现：`learn: add contextual console navigation stage`

### 执行记录

- 2026-06-14：Stage 01 已冻结；Stage 02 只规划上下文相关菜单导航，不创建代码目录。

### Stop policy

- 规划卡落地后停止，等待用户填写事前预测；不得直接进入实现。
