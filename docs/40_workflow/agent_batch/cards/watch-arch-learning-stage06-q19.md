# Watch Architecture Learning Stage 06

## 卡片 ARCH-LAB-06 时间维度、系统事件与 dirty render 压力

- ID：`ARCH-LAB-06`
- 标题：时间维度、系统事件与 dirty render 压力
- 批次：架构冲突实验场
- 状态：DONE
- 依赖：`ARCH-LAB-05` DONE

### 问题定位

Stage 05 已经证明：

- `shared_action_t` 值得保留为多输入入口的最小共同终点。
- 它属于用户动作语义，不等同于页面状态，也不等同于“目标页面直达”。
- 更高层的 `go_to_about` 之类目标拆分，当前更像 replay/test helper，而不是产品真实共享语义。

因此 Stage 06 不该继续发明“目标导航层”，而该转向更真实的新压力：

> 当系统开始面对时间推进、非用户事件和显示刷新节奏时，现有“动作即修改、修改即显示”的直接链路还是否够用？

### 单一新增压力

本阶段只增加一类压力：

- 时间维度
- 非用户系统事件
- 状态修改与显示输出分离

具体只围绕这些代表性场景做规划：

- 用户 10 秒无操作，系统进入 sleep。
- 用户未打开电池页，但电量变化了，状态栏或全局信息需要更新。
- 用户在菜单页时，1 秒 tick 仍会推动别处的时间数据变化。
- 用户处于 sleep 时输入 `next` / `enter` / `back`，要先唤醒还是继续执行原动作。
- 状态发生变化后，不一定立刻 render；render 可能延后、合并或按 dirty 标记触发。

### 本阶段要回答的问题

1. `shared_action_t` 之外，是否需要一个更外层的 `app_event_t` 来承接：
   - 用户动作
   - tick
   - idle timeout
   - battery changed

2. 哪些事件仍属于用户动作语义，哪些已经是系统事件，绝不能混进 `shared_action_t`？

3. 是否需要引入最小 `power_state_t`（例如 `ACTIVE` / `SLEEP`）来表达：
   - 系统当前是否接受用户动作
   - 第一拍输入是“唤醒”还是“执行动作”

4. 状态修改与 render 是否应该分离成：
   - 先修改状态并设置 dirty
   - 后续再统一 render

5. 这一轮最小实现若成立，真正该证明的是什么？
   - 不是“抽象更优雅”
   - 而是“时间与系统事件进入后，旧直连写法是否开始出现真实压力”

### 本轮实现边界

本轮现在进入最小实现，只允许验证 Stage 06 的第一版压力。

本轮必须继续明确：

- Stage 06 不引入 `GO_TO_ABOUT`、`NAVIGATE_TO_*` 一类目标导航层。
- `shared_action_t` 继续只代表用户动作，不承载 tick、电量变化、idle timeout。
- 如果需要更外层事件，只能把它写成“当前这轮最小实现下成立的边界”，不能写成已证明唯一正确答案。
- replay/test helper 允许继续存在，但不能反过来驱动产品结构。

### 用户事前预测

进入实现轮前，用户需要先回答：

1. 你认为哪些输入或变化已经不属于 `shared_action_t`，而应该单独视为系统事件？
非用户输入，系统级行为，被动，可能有稳定时钟周期

2. 如果系统处于 sleep，第一拍用户输入应当只负责唤醒，还是唤醒并继续执行原动作？
第一拍用户输入应当只负责唤醒

3. 你觉得 render 延后之后，最容易暴露的 bug 会是什么：漏刷新、重复刷新，还是状态和显示不同步？
render延后过后，可能会带来同步问题，在刷新之前数据变动了两次？
不过我觉得保存相对固定的刷新周期问题不大

4. 你预期 Stage 06 的最小实现更像“事件外壳”，还是“脏标记刷新”？
他应该更像是事件外壳，系统级事件，用户事件，脏区刷新本质上也是一个事件

5. 出现什么证据时，你会判断“Stage 06 这次抽象还不值得引入”？
不知道，我几乎肯定，引入系统级事件，这种抽象几乎是必然的

### 下一实现卡的预设边界

本轮实现只做一个最小闭环：

- 新建独立 `stage_06_system_event_pressure/`
- 保留 Stage 05 的页面数量与导航规模
- 加入有限的 tick / idle / battery / sleep 事实
- 观察 `shared_action_t` 外面是否自然长出一层系统事件承接点
- 观察 dirty render 是否真的减少“状态改动立即输出”的耦合

下一实现卡仍禁止：

- EventQueue
- 多线程
- RTOS
- 目标导航层
- 更多页面
- 第二 View
- 直接复刻 Magic Watch 的 Core、Adapter、PowerController

### 实施方案

- 复制 Stage 05 为独立 `stage_06_system_event_pressure/`
- 保留 `shared_action_t` 作为用户动作语义
- 新增最小 `app_event_t`，只承接：
  - 用户动作
  - `tick`
  - `idle timeout`
  - `battery changed`
- 新增最小 `power_state_t`，只区分 `ACTIVE` / `SLEEP`
- 引入 dirty render：
  - 事件只负责修改状态并置 dirty
  - 顶层循环统一决定何时 render
- 保留当前页面规模，不新增页面
- 用最小命令入口模拟系统事件，不引入线程、异步循环或真实定时器

### 涉及位置

Allowed files

- `watch_arch_learning/CMakeLists.txt`
- `watch_arch_learning/README.md`
- `watch_arch_learning/stage_06_system_event_pressure/**`
- `docs/20_guides/watch_arch_learning_lab.md`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage06-q19.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_arch_learning/stage_05_shared_action_semantics/**`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage05-q18.md`

Forbidden changes

- 不修改 Stage 00~05 任何实现代码。
- 不引入 `GO_TO_*` 目标导航层。
- 不把系统事件塞进 `shared_action_t`。
- 不引入 EventQueue、多线程、RTOS 或真实后台定时器。

### 风险

- 把“时间维度与系统事件”偷换成“目标导航层”或“更大导航状态机”。
- 因为担心未来复杂度，提前引入 EventQueue、线程或完整 PowerController。
- 把 replay/test helper 的便利性误判成产品真实需求。

### 自检

- `git diff --check`
- `cmake --build --preset console-debug --target watch_arch_stage_06_system_event_pressure`
- 人工命令矩阵覆盖：sleep、wake、battery、tick、用户动作
- 检查 Stage 06 仍未引入 `GO_TO_*` 目标导航层
- 检查 `shared_action_t` 与系统事件的边界仍然清楚
- 中文 Markdown 乱码哨兵检查

### 验收标准

- `stage_06_system_event_pressure` 可独立构建运行
- `shared_action_t` 仍只承载用户动作，系统事件通过 `app_event_t` 进入
- sleep 下第一拍用户输入只负责唤醒，不执行原动作
- 状态修改与 render 已分离，至少能观察到“一次顶层命令触发多个状态变化，但只 render 一次”
- 没有把 EventQueue、线程、更多页面或目标导航层混入同一轮

### Doc Impact

`required`

### 建议提交信息

实现：`learn: add system event pressure stage`

### 执行记录

- 2026-06-14：Stage 05 已冻结；用户明确否决 `GO_TO_ABOUT` 一类目标导航层，认为它更像测试便利而非真实产品输入语义。
- 2026-06-14：本卡只落成 Stage 06 纯规划卡，把下一压力收敛为“时间维度、系统事件与 dirty render”，不创建代码目录，不进入实现。
- 2026-06-14：用户已完成 Stage 06 事前预测，卡片转入最小实现轮；本轮只允许创建 `stage_06_system_event_pressure/`，验证 `app_event_t`、最小 `power_state_t` 与 dirty render。
- 2026-06-14：第一版实现已接通；新增 `stage_06_system_event_pressure/`、`app_event_t`、最小 `power_state_t` 和 dirty render。`cmake --preset console-debug` 与 `cmake --build --preset console-debug --target watch_arch_stage_06_system_event_pressure` 通过。
- 2026-06-14：脚本验证已覆盖 `tick 10 -> idle timeout -> sleep` 与 `sleep -> key_select -> wake only -> key_select -> open menu`；说明“多次状态变化只 render 一次”和“第一拍输入只唤醒”都已成立。
- 2026-06-15：用户复核后指出当前 `fgets()` 仍阻塞主循环，事件仍以直接调用传递，dirty render 仍由输入处理路径主动触发。结论是 Stage 06 已成功暴露“事件对象化不等于时间片系统”的真实压力，但不继续在实验场实现队列与调度。
- 2026-06-15：本卡按阶段性实验完成收口。保留代码作为过渡样本，不宣称它已经实现自动 tick、事件队列或周期 render task。

### Stop policy

- 已冻结；架构冲突实验场暂停，后续转入真实 PC 模拟器架构审查。
