# Watch Architecture Learning Stage 04

## 卡片 ARCH-LAB-04 命令字符串是否已变成隐式内部合同

- ID：`ARCH-LAB-04`
- 标题：命令字符串是否已变成隐式内部合同
- 批次：架构冲突实验场
- 状态：TODO
- 依赖：`ARCH-LAB-03` DONE

### 问题定位

Stage 03 已经证明两件事：

- `key_*` 硬件名称可以被隔离在翻译入口之外。
- 业务导航仍直接消费 `"menu"`、`"next"`、`"enter"`、`"back"` 这些字符串。

这意味着当前系统虽然不再泄漏硬件名，但已经把“终端命令语言”当成了
隐式共享合同。Stage 04 不讨论异步、队列、Power 或第二套 View，只回答：

> 当新调用入口不愿意伪装成终端文本时，字符串命令是否开始变贵？

### 单一新增压力

增加一个“非终端输入、非实体按键”的调用入口，例如脚本回放器或内建 demo driver。

它的特点是：

- 想复用同一套页面状态和导航行为。
- 不自然属于 `"menu"`、`"next"` 这种终端文本。
- 又不应该直接知道页面渲染细节。

### 本阶段要观察的冲突

1. 新入口是否只能继续产出命令字符串，才能复用现有逻辑？
2. 如果不愿继续产出字符串，最小内部动作语义是否开始变得合理？
3. 当前字符串是否只是“方便的输入形式”，还是已经变成“系统内部语言”？
4. 引入最小动作枚举时，是否真的减少修改扩散，还是只是把字符串换成名字更正式的枚举？

### 实施边界

本卡先做规划，不实现代码。

下一实现轮只允许：

- 复制 Stage 03 为独立 Stage 04。
- 增加一个新的调用入口。
- 比较“继续复用命令字符串”与“引入最小动作枚举”两种接法。

下一实现轮仍禁止：

- EventQueue
- 多线程或异步输入
- 第二套导航状态机
- Power
- 新页面或更深导航
- 直接引入 Magic Watch 的 Core、Adapter 或现成架构

### 用户事前预测

实现前回答：

1. 新入口最自然产出的是什么，而不是它“能否伪装成字符串”？
2. 如果继续共用字符串，哪些地方会开始别扭？
3. 你认为动作枚举的最小集合应该是什么？
4. 你预计动作枚举会减少哪些修改，又会增加哪些样板？
5. 你认为什么时候仍不值得引入动作枚举？

### 涉及位置

Allowed files

- `watch_arch_learning/README.md`
- `docs/20_guides/watch_arch_learning_lab.md`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage04-q17.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_arch_learning/stage_03_multi_input_direct/**`

Forbidden changes

- 不修改已冻结 Stage 00~03。
- 不创建 Stage 04 代码目录。
- 不实现动作枚举或新入口代码。
- 不引入异步、队列、Power、第二 View 或更深导航。

### 风险

- 过早把“字符串不优雅”误判成“必须抽象”。
- 把“新增调用入口”与“时间解耦”混在一起，导致错误地提前引入队列。
- 把 Stage 04 写成 Magic Watch 最终架构的复刻。

### 自检

- `git diff --check`
- 检查 Stage 04 仅是规划，不创建代码目录
- 检查规划只攻击“命令字符串是否已成内部语言”
- 中文 Markdown 乱码哨兵检查

### 验收标准

- 明确 Stage 04 的唯一压力是“命令字符串语言”
- 没有把异步、队列、Power 或第二 View 混进同一轮
- 用户在实现前就能分清“输入形式”“内部动作”“状态修改”

### Doc Impact

`required`

### 建议提交信息

规划：`docs: plan command-language pressure stage`

### 执行记录

- 2026-06-14：Stage 03 已冻结；Stage 04 只规划“命令字符串是否已变成隐式内部合同”，不创建代码目录。

### Stop policy

- 规划提交后停止，等待用户填写事前预测；不得直接实现动作枚举。
