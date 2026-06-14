# Watch Architecture Learning Stage 05

## 卡片 ARCH-LAB-05 最小共享动作语义是否值得引入

- ID：`ARCH-LAB-05`
- 标题：最小共享动作语义是否值得引入
- 批次：架构冲突实验场
- 状态：DONE
- 依赖：`ARCH-LAB-04` DONE

### 问题定位

Stage 04 已经给出当前最强证据：

- 新入口天然想表达步骤语义。
- 旧导航只能消费命令字符串。
- 新入口为了表达更高层目标，还得自己补一段把目标拆成多次 `"next"` 和
  一次 `"enter"` 的中间逻辑。

因此下一阶段不该再讨论硬件名、回放入口、异步、队列或更多页面，而应只回答：

> 最小共享动作语义到底该长什么样，放在哪一层，怎样验证它真的减少了中间翻译？

### 单一新增压力

把 Stage 04 中已经出现的三类入口放在同一张桌子上比较：

- 终端命令
- `key_*` 实体按键模拟
- `replay_*` 回放入口

只观察一件事：

- 如果它们都要复用同一套页面状态与导航行为，最小共享动作语义是否会比字符串命令更贴近“真正共享的那层”。

### 本阶段要回答的问题

1. 最小共享动作语义的候选集合是什么？
修改当前s_current_page值，当前不同页面所具备的修改s_current_page值的选项应该不同
修改s_menu_selection的值，只有菜单页面可修改的值
返回上级页面即，使s_current_page = s_detail_return_page

2. 它应该替代哪一层：输入翻译层、`handle_command()`，还是别处？
他替代handle_command层
但是其他的输入翻译层也要据此完成适配

3. 哪些东西仍不属于共享动作语义，而只是平台输入或页面状态修改？
不知道，也许我对于共享动作语义的理解，当前和你不同

4. 如何设计一个实现轮，让我们能证明“减少中间翻译”，而不是只是把字符串换成枚举名？
我并不觉得这能够显著减少翻译，但是可以使不出现多次翻译的过程，因为翻译的终点被确定了，
他只是选取了一种动作标准

5. 这一轮为什么仍然不该碰异步、队列、第二 View 或更深页面？
不知道

### 实施边界

本卡现在进入实现轮，只允许：

- 复制 Stage 04 为独立 Stage 05。
- 在 Stage 05 内引入最小共享动作语义。
- 比较“旧字符串命令入口”与“共享动作语义入口”的中间翻译长度和修改范围。

本轮仍禁止：

- EventQueue
- 多线程或异步输入
- 第二套导航状态机
- 新页面或更深导航
- Power
- 直接引入 Magic Watch 的 Core、Adapter 或现成动作合同

### 用户事前预测

实现前回答：

1. 你认为最小共享动作语义的候选集合是什么？
2. 你觉得它最应该替代字符串的哪一层？
3. 哪些东西仍然不该进共享动作语义，例如页面状态、具体目标页面或菜单索引？
4. 你预计它会减少哪类中间翻译，又会新增什么样板？
5. 出现什么证据时，你会判断“这次动作语义引入并不值得”？

### 涉及位置

Allowed files

- `watch_arch_learning/CMakeLists.txt`
- `watch_arch_learning/README.md`
- `watch_arch_learning/stage_05_shared_action_semantics/**`
- `docs/20_guides/watch_arch_learning_lab.md`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage05-q18.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_arch_learning/stage_04_command_language_pressure/**`

Forbidden changes

- 不修改已冻结 Stage 00~04。
- 只允许创建 `watch_arch_learning/stage_05_shared_action_semantics/**`。
- 不把 Stage 05 和异步、队列、Power、第二 View 或更深导航混在一起。

### 风险

- 把“值得验证”误写成“已经证明必须引入”。
- 让 Stage 05 借题发挥，提前做成完整架构改造。
- 把页面状态修改、目标页面选择和输入动作语义混成一层。

### 自检

- `git diff --check`
- Console-only 构建通过
- 终端命令回归通过
- 按键输入回归通过
- 回放入口回归通过
- 扫描确认 `handle_command()` 已退出，三类入口都先落到共享动作语义
- 中文 Markdown 乱码哨兵检查

### 验收标准

- 三类入口先落到同一套最小共享动作语义，再驱动同一份页面状态
- 共享动作语义不直接等同于页面状态修改或目标页面枚举
- 能用代码证据说明哪些中间翻译被减少了，哪些仍然存在
- 没有把异步、队列、更多页面或 Power 混入同一轮

### Doc Impact

`required`

### 建议提交信息

实现：`learn: add shared action semantics stage`

### 执行记录

- 2026-06-14：Stage 04 已冻结；Stage 05 只规划“最小共享动作语义是否值得引入”，不创建代码目录。
- 2026-06-14：用户确认进入实现轮；本卡改为执行 `stage_05_shared_action_semantics`，只验证最小共享动作语义是否真的减少中间翻译。
- 2026-06-14：Console 构建通过；终端命令、按键输入、回放入口与混合矩阵自动回归通过。自动观察到：
  三类入口都先落到 `shared_action_t`；终端字符串退回成输入形式，不再是共同终点；
  但“去 Steps/About”的更高层目标仍需在回放入口拆成多个动作。
- 2026-06-14：用户已人工运行并完成五个复盘问题；结论收口为“`shared_action_t` 值得保留，但更高层目标拆分仍留在 replay/test 层”。Stage 05 冻结。

### Stop policy

- 已冻结；后续只允许把本轮结论作为下一阶段输入，不回写新实现。
