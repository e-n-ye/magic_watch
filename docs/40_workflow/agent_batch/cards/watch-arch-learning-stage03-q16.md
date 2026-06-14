# Watch Architecture Learning Stage 03

## 卡片 ARCH-LAB-03 第二种输入表示

- ID：`ARCH-LAB-03`
- 标题：第二种输入表示
- 批次：架构冲突实验场
- 状态：DONE
- 依赖：`ARCH-LAB-02` DONE，提交 `ee08027`

### 问题定位

Stage 02 的所有业务行为都由终端语义命令触发，例如 `next`、`enter` 和 `back`。当前尚未证明：

- 第二种输入是否会复制同一套导航规则。
- 硬件名称是否会泄漏到页面业务判断。
- 是否真的需要把不同输入统一成内部动作语义。

Stage 03 只增加模拟实体按键输入，不增加页面、导航深度、异步来源或业务状态。

### 新增需求

保留 Stage 02 的全部终端命令，并增加四个模拟按键输入：

- `key_next`
- `key_prev`
- `key_select`
- `key_back`

行为映射：

| 当前页面 | 模拟按键 | 等价终端行为 |
| --- | --- | --- |
| Home | `key_select` | `menu` |
| Home | `key_back` | `back` |
| App Menu | `key_next` | `next` |
| App Menu | `key_prev` | `prev` |
| App Menu | `key_select` | `enter` |
| App Menu | `key_back` | `back` |
| Detail | `key_back` | `back` |

限制：

- `heart` / `steps` 快捷命令仍只属于终端输入。
- `show` / `help` / `quit` 仍是终端程序控制，不模拟硬件按键。
- 其它页面与按键组合输出 `Input unavailable on this page.`。

### 本阶段真正要观察的冲突

1. 如果直接在页面分支中加入：

```c
strcmp(input, "next") == 0 ||
strcmp(input, "key_next") == 0
```

同一业务动作是否开始出现重复输入判断？

2. `key_select` 在 Home 表示打开菜单，在 App Menu 表示进入选中项。输入层是否应该知道当前页面？
3. 如果把 `key_select` 直接翻译成 `"menu"` 或 `"enter"`，翻译逻辑由谁决定上下文？
4. 如果页面规则直接认识 `key_*`，硬件名称是否已经泄漏进业务导航？
5. 终端命令和按键输入能力不完全相同，统一语义是否意味着必须强行抹平差异？

### 实施方案

本卡拆成三个停止点：

1. 用户先填写事前预测并选择第一版直接接法。
2. 复制 Stage 02 为独立 Stage 03，继续单文件实现两套输入。
3. 运行后统计重复判断与修改扩散，再决定是否引入最小动作枚举。

第一版必须保持：

- 单个 `main.c`。
- 单一 `handle_command()` 或等价直接入口。
- 字符串输入直接判断。
- 现有页面状态和渲染不变。

第一版禁止直接引入：

- `Intent` / `Action` 枚举。
- Input Adapter 或 Input Port。
- EventQueue。
- 第二份导航状态机。
- 多线程或异步输入。

### 用户事前预测

实现前回答：

- 你会选择哪一种第一版接法，为什么？
  - A：在现有页面分支中用 `||` 同时判断终端命令和 `key_*`。
  - B：先把 `key_*` 翻译成现有终端命令，再调用原导航逻辑。
我会选择B,选择a后续继续的维护成本会越来越高，增加新页面又要重写一遍对于多输入的转化
如果选择b,只需要把按键指令转化为串口语义，对于之前的handle_command来说，他什么都不知道，他还是解析之前的命令

- 如果选择 B，`key_select` 在 Home 和 App Menu 中如何翻译？谁必须知道当前页面？
在进入handle_command之前，有个中介层知道当前页面，他负责翻译

- 哪些代码属于“输入表示”，哪些代码属于“业务动作”？
翻译层把输入转化为语义，
根据语义和当前处于的页面，改变当前页面，是业务动作
页面显示层根据当前页面，显示，是业务动作

- `key_next` 和字符串 `next` 是否应当被业务层视为同一件事？
业务层不应该看到真实的key_next，next
业务层只理解 menu_slect++

- 两种输入能力不完全一致时，统一语义应该保留哪些差异？
输入频率，旋转编码器可以产生多个动作，可以快速执行多次，比按键快

- 出现多少处重复或哪类修改扩散后，你才认为动作枚举值得引入？
跨平台的输入事件，需要统一语义

- 如果以后把 `key_next` 改名为 `crown_cw`，你预计需要修改哪些位置？
只修改入口层，转化为统一语义后，其他代码不需要修改

### 预测审查

- 方案 B 可以让 `handle_command()` 不认识 `key_*`，但不会自动消除上下文耦合。
- `key_select` 在 Home 和 App Menu 中含义不同，因此第一版翻译函数必须读取当前页面。
- `menu_selection++` 不是输入语义，而是执行“选择下一项”后的状态修改。第一版仍用
  `next` / `prev` / `menu` / `enter` 作为中间字符串，不提前引入动作枚举。
- “跨平台”可以成为动作语义的压力，但不能只凭未来可能跨平台就立即抽象。本 Stage
  先通过改名范围、上下文读取和混合输入矩阵收集证据。

### 涉及位置

Allowed files

- `watch_arch_learning/CMakeLists.txt`
- `watch_arch_learning/README.md`
- `watch_arch_learning/stage_03_multi_input_direct/**`
- `docs/20_guides/watch_arch_learning_lab.md`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage03-q16.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_arch_learning/stage_00_direct/**`
- `watch_arch_learning/stage_01_console_direct/**`
- `watch_arch_learning/stage_02_context_menu/**`

Forbidden changes

- 不修改已冻结 Stage。
- 不修改 Magic Watch 产品代码、PC XML UI、F411 或 `watch_core`。
- 不增加页面、菜单项、导航深度、设置数据或 Power。
- 不接真实串口、GPIO、LVGL、SDL、线程或异步事件。
- 用户完成预测前不创建 Stage 03 代码目录。
- 不创建 Stage 04 或后续卡片。

### 为什么不同时加入异步或事件队列

本 Stage 只研究“同一时刻到来的不同输入表示”。如果同时加入异步到达、并发顺序或缓存，就无法区分压力来自语义重复还是时间解耦。EventQueue 必须等待独立证据。

### 风险

- `key_*` 仍由终端文本模拟，不等价于真实 GPIO 扫描和消抖。
- 方案 B 看起来更整洁，但可能只是把页面上下文耦合移动到翻译函数。
- 方案 A 会有意暴露重复，但不能因为代码丑就自动宣布 Intent 正确。

### 自检

规划轮：

- `git diff --check`
- 检查未创建 `stage_03_multi_input_direct/`
- 检查只增加第二输入表示，不增加新业务状态
- 中文 Markdown 乱码哨兵检查

实现轮：

- Console-only 构建通过
- 终端命令回归通过
- 模拟按键行为矩阵通过
- 混合使用两套输入后状态一致
- 记录新增判断数量和硬件名称出现位置

### 验收标准

规划轮：

- 两种输入及其能力差异明确。
- 用户预测问题完整。
- 未提前引入统一动作抽象。

实现轮：

- 两套输入都能驱动同一份页面状态。
- 没有复制第二套导航状态机。
- 能用代码证据判断输入表示是否泄漏到业务规则。
- 对“保留直接实现”或“引入最小动作语义”形成明确结论。

### Doc Impact

`required`

### 建议提交信息

规划：`docs: plan multi-input conflict stage`

实现：`learn: add direct multi-input stage`

### 执行记录

- 2026-06-14：Stage 02 已冻结；Stage 03 只规划第二种输入表示，不创建代码目录。
- 2026-06-14：用户完成事前预测并选择方案 B；已建立第一版字符串翻译实现，等待人工运行矩阵与复盘，尚未冻结。
- 2026-06-14：Console 全目标构建通过；终端、按键、混合及不可用输入自动矩阵通过。
  扫描确认 `handle_command()` 无 `key_*`，但仍消费命令字符串；翻译函数读取
  `s_current_page`。输入频率差异尚未建模，继续保留为未验证。
- 2026-06-14：用户已人工运行并完成六个复盘问题；结论收口为“隔离了硬件名，但尚未证明必须把命令字符串升级为动作枚举”。Stage 03 冻结。

### Stop policy

- 已冻结；后续只允许把结论作为 Stage 04 的输入，不回写新实现。
