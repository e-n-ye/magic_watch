# Watch Architecture Learning Stage 06

## 卡片 ARCH-LAB-06 时间维度、系统事件与 dirty render 压力规划

- ID：`ARCH-LAB-06`
- 标题：时间维度、系统事件与 dirty render 压力规划
- 批次：架构冲突实验场
- 状态：TODO
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

### 结论边界

本卡是纯规划卡，只允许形成 Stage 06 的实现边界，不进入代码。

本卡必须明确：

- Stage 06 不引入 `GO_TO_ABOUT`、`NAVIGATE_TO_*` 一类目标导航层。
- `shared_action_t` 继续只代表用户动作，不承载 tick、电量变化、idle timeout。
- 如果需要更外层事件，只能把它写成“候选实现边界”，不能写成已证明唯一正确答案。
- replay/test helper 允许继续存在，但不能反过来驱动产品结构。

### 用户事前预测

进入实现轮前，用户需要先回答：

1. 你认为哪些输入或变化已经不属于 `shared_action_t`，而应该单独视为系统事件？
2. 如果系统处于 sleep，第一拍用户输入应当只负责唤醒，还是唤醒并继续执行原动作？
3. 你觉得 render 延后之后，最容易暴露的 bug 会是什么：漏刷新、重复刷新，还是状态和显示不同步？
4. 你预期 Stage 06 的最小实现更像“事件外壳”，还是“脏标记刷新”？
5. 出现什么证据时，你会判断“Stage 06 这次抽象还不值得引入”？

### 下一实现卡的预设边界

如果用户确认进入实现轮，下一张卡应只做一个最小闭环：

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

### 涉及位置

Allowed files

- `watch_arch_learning/README.md`
- `docs/20_guides/watch_arch_learning_lab.md`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage06-q19.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_arch_learning/stage_05_shared_action_semantics/**`
- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage05-q18.md`

Forbidden changes

- 不创建 `stage_06_system_event_pressure/` 代码目录。
- 不修改 Stage 00~05 任何实现代码。
- 不把 Stage 06 规划写成已实现事实。
- 不引入 `GO_TO_*` 目标导航层。
- 不把系统事件塞进 `shared_action_t`。

### 风险

- 把“时间维度与系统事件”偷换成“目标导航层”或“更大导航状态机”。
- 因为担心未来复杂度，提前引入 EventQueue、线程或完整 PowerController。
- 把 replay/test helper 的便利性误判成产品真实需求。

### 自检

- `git diff --check`
- 检查 Stage 06 卡片仍是纯规划，不包含实现完成表述
- 检查规划明确排除了 `GO_TO_*` 目标导航层
- 检查 `shared_action_t` 与系统事件的边界已写清
- 中文 Markdown 乱码哨兵检查

### 验收标准

- 能明确说明为什么 Stage 06 不该做目标导航层
- 能明确说明下一压力是“时间维度 + 系统事件 + dirty render”
- 能给出下一实现卡的最小边界和禁止项
- 没有把实现轮和规划轮混在一起

### Doc Impact

`required`

### 建议提交信息

规划：`docs: plan stage 06 system event pressure`

### 执行记录

- 2026-06-14：Stage 05 已冻结；用户明确否决 `GO_TO_ABOUT` 一类目标导航层，认为它更像测试便利而非真实产品输入语义。
- 2026-06-14：本卡只落成 Stage 06 纯规划卡，把下一压力收敛为“时间维度、系统事件与 dirty render”，不创建代码目录，不进入实现。

### Stop policy

- 规划提交后停止，等待用户先完成事前预测，再决定是否进入 Stage 06 最小实现卡。
