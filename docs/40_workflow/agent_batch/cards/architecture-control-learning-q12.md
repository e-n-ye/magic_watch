# 架构掌控力恢复学习轮

本学习轮位于 `V0.5` 收口之后、`V0.6` 实现之前。目标不是继续增加功能，而是让项目所有者能够亲自追踪真实代码链路、判断状态所有权，并用公开合同写出可运行的诊断程序。

> 路线执行状态：PAUSED  
> 2026-06-13 起由[架构冲突实验场总纲](/D:/MY_Desk/watch/magic_watch/docs/20_guides/watch_arch_learning_lab.md)接管当前学习方向。下列 `LEARN-A~D` 状态仅保留当时现场，不得由队列或新窗口继续自动执行。用户已有思考和卡片设计不删除；未来只在具体实验暴露对应知识缺口时按需复用局部内容。

## 卡片 LEARN-A F411 真实主链追踪

- ID：`LEARN-A`
- 标题：F411 真实主链追踪
- 批次：架构掌控力恢复学习轮
- 状态：IN_PROGRESS
- 依赖：`V0.5-P4-A`

### 问题定位

当前 F411 主链已经完成多轮收口，但“代码能运行”和“项目所有者能从入口追到决策与显示”不是同一件事。若不能独立解释表冠短按和左边缘右滑两条真实链路，进入 `V0.6` 后仍容易把输入门控、Core 决策和平台执行混在一起。

### 实施方案

1. 学习工作簿按“最小地图 -> 单个认知结 -> 调用链验证 -> 用户推演 -> Agent 纠偏”推进。
2. 表冠主链拆成独立认知结：
   - 10ms 扫描如何完成消抖和长短按。
   - Input FIFO 如何连接生产任务与 UI 消费任务。
   - LVGL 如何主动读取 encoder 状态并点击当前焦点对象。
   - View callback 如何进入 Adapter 和 Core。
3. 完成表冠链后，再用相同方式学习左边缘右滑链。
4. 只有整条链在脑中可以连续运行后，才讨论状态所有权、跨平台边界和未来 Power 门控位置。
5. 用户保留自由阅读中的问题和质疑；Agent 提供最小解释与代码锚点，不替用户完成时间线推演。

### 涉及位置

Allowed files

- `docs/20_guides/architecture_control_learning_workbook.md`
- `docs/document_map.md`
- `docs/40_workflow/agent_batch/cards/architecture-control-learning-q12.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `try/my_watch_f411_v2.1/Core/Src/freertos.c`
- `try/my_watch_f411_v2.1/user/services/input/watch_input_service.*`
- `try/my_watch_f411_v2.1/user/app/input/watch_input_intent.*`
- `try/my_watch_f411_v2.1/user/app/watch_bringup.*`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.*`
- `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lite_view.*`
- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`

Forbidden changes

- 不修改任何代码、工程文件、输入算法、触摸阈值或真机行为。
- 不提前实现追踪程序。
- 不把工作簿写成完整参考答案。
- 不进入 Power executor 或表冠熄屏门控实现。

### 风险

- 若由 Agent 直接填完整链路，用户只能阅读结果，不能训练代码追踪和所有权判断。
- 若同时追 PC、F411 和旧 C++ 模拟器，第一轮认知负担会过高。
- 若把历史 Wake 键写入当前产品链路，会重现已经纠正过的方向误判。

### 自检

- `git status --short -uall`
- `git diff --check`
- 检查工作簿是否覆盖两条链和六类分层问题
- 对本轮实际改动的中文 Markdown 做乱码哨兵检查

### 验收标准

- 用户完成各认知结的时间线推演，且函数顺序与当前代码一致。
- 用户能区分硬件事实、输入意图、LVGL 输入状态、typed `UiEvent`、Core 页面状态和 View 对象状态。
- 用户能指出未来 Power 门控应发生在表冠 Intent 喂入 LVGL 之前，并解释该结论。
- 用户可以不看架构文档，用自己的话完整讲述两条链。

### Doc Impact

`required`

### 建议提交信息

`docs: start architecture control learning round`

### 执行记录

- 2026-06-13：卡片创建并进入 `IN_PROGRESS`；已准备学习工作簿，等待用户完成首次链路追踪。
- 2026-06-13：用户实际阅读证明“按文件清单通读 -> 一次填写多列表格”的方式不符合调用链思维；已暂停旧表格，改为逐个认知结的讲解、验证、推演和纠偏。
- 2026-06-13：用户完成认知结 3 首次作答；已确认其掌握输入状态由应用准备、LVGL 通过注册回调反向读取的主结构。当前补充 `read_cb`、group、focus、event 与 `user_data` 的最小知识，并纠正“页面 feature 等于焦点状态”的误解；等待用户复述后验收本认知结。
- 2026-06-13：用户已能复述 `CONFIRM -> pulse -> read_cb -> group focus -> CLICKED -> user_data -> on_card_clicked()`，认知结 3 通过。开始认知结 4，先研究 View 如何报告 feature、Adapter 如何生成 typed `UiEvent`、Core 在何处首次作出页面决定；暂不混入 PageState 到 View 的反向同步。
- 2026-06-13：用户确认逐函数学习最终架构不能补足“抽象为何产生”的经验；本路线整体暂停，保留为历史学习现场，不再继续认知结 4 或启动 `LEARN-B~D`。

### Stop policy

- 用户完成工作簿中的 `LEARN-A` 首次作答后停止，由 Agent 审查；验收前不得进入 `LEARN-B`。

## 卡片 LEARN-B PC/F411 边界对照

- ID：`LEARN-B`
- 标题：PC/F411 边界对照
- 批次：架构掌控力恢复学习轮
- 状态：TODO
- 依赖：`LEARN-A`

### 问题定位

只理解 F411 单平台链路，还不足以判断“共享代码”和“共享语义”的差别。需要把 PC XML Adapter 与 F411 Adapter 放在同一张表中比较，确认哪些差异是合理平台分叉，哪些行为必须保持一致。

### 实施方案

1. 用户只读对照 PC XML Adapter、F411 Adapter、两端 View 和入口装配代码。
2. 在工作簿中完成“共享合同 / Adapter / View / Platform”所有权矩阵。
3. 列出至少三项“旧设想与当前实现”的差异，并给出代码证据。
4. 回答：
   - 为什么当前没有独立 `ScreenManager`，仍然存在页面决策与执行边界。
   - 为什么 F411 表冠旋转先进入 LVGL focus，而不是直接生成健康卡点击事件。
   - 为什么两个 Adapter 相似但不应粗暴合并。
5. Agent 只做审查和追问，不修改运行代码。

### 涉及位置

Allowed files

- `docs/20_guides/architecture_control_learning_workbook.md`
- `docs/40_workflow/agent_batch/cards/architecture-control-learning-q12.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_core/**`
- `sim/lv_port_pc_vscode/src/xml_sim_main.cpp`
- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.*`
- `ui/lvgl_pro/**`
- `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lite_view.*`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.*`

Forbidden changes

- 不修改 PC/F411 Adapter、View、输入算法或 `watch_core`。
- 不为了代码相似而抽公共 Adapter 基类或公共 View。
- 不扩页面、不补 PC 左边缘手势。
- 不提前进入 `LEARN-C`。

### 风险

- 只按文件名比较会忽略两端 LVGL 版本和对象生命周期的真实差异。
- 把“共享语义”误解为“共享实现”，会重新引入 F411 强吃 PC generated C 的旧问题。

### 自检

- `git status --short -uall`
- `git diff --check`
- 检查所有权矩阵是否覆盖共享合同、Adapter、View、Platform
- 对本轮实际改动的中文 Markdown 做乱码哨兵检查

### 验收标准

- 用户完成所有权矩阵，并能指出每个权威状态属于谁。
- 用户至少写出三项旧设想与当前实现差异。
- 用户能解释两个 Adapter 的共同消费规则和不可合并原因。
- 所有结论都有当前代码路径或函数作为证据。

### Doc Impact

`required`

### 建议提交信息

`docs: compare pc and f411 architecture boundaries`

### 执行记录

- 2026-06-13：卡片创建，待 `LEARN-A` 验收后执行。

### Stop policy

- 本卡验收后停止，不自动进入 `LEARN-C`。

## 卡片 LEARN-C Core 公开合同追踪程序

- ID：`LEARN-C`
- 标题：Core 公开合同追踪程序
- 批次：架构掌控力恢复学习轮
- 状态：TODO
- 依赖：`LEARN-B`

### 问题定位

现有合同测试可以证明行为正确，但不适合按时间顺序观察输入、瞬时动作和持久状态之间的区别。需要一个只使用公开 API 的可执行教材，让用户亲手把 V0.5 的导航与 Power 合同串起来。

### 实施方案

1. 用户亲自新增 `watch_core/examples/watch_core_contract_trace.c`。
2. 用户亲自在 CMake 中登记 `magic_watch_core_contract_trace`。
3. Agent 在用户首次实现后按代码审查方式反馈，不先提供完整实现。
4. 程序按顺序展示：
   - 初始化后的 Page、Power、Snapshot。
   - SpO2 点击的 push、drain、最后 Intent 和最终 PageState。
   - Back 返回首页。
   - 首页 Back no-op 后仍继续处理合法点击。
   - Power request 前后状态不变。
   - 失败 commit 保持原状态。
   - 成功息屏与成功唤醒。
   - Power 往返后页面与 Snapshot 保持不变。
5. 追踪程序正式入库，但不得成为产品运行依赖或替代合同测试。

### 涉及位置

Allowed files

- `watch_core/examples/watch_core_contract_trace.c`
- `sim/lv_port_pc_vscode/CMakeLists.txt`
- `docs/20_guides/architecture_control_learning_workbook.md`
- `docs/40_workflow/agent_batch/cards/architecture-control-learning-q12.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`
- `watch_core/tests/watch_core_contract_test.c`
- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.*`

Forbidden changes

- 不修改 `watch_core` API 或实现。
- 不访问 `current_page`、`power_controller`、`event_queue`、`queue_head`、`queue_tail`、`queue_count`。
- 不依赖 LVGL、SDL、HAL、CubeMX 或动态内存。
- 不实现 Power executor、背光、唤醒或 F411 输入门控。
- 不让 Agent 在用户首次提交前代写完整程序。

### 风险

- 若直接读取 `WatchCore` 内部字段，练习会固化实现细节而不是公开合同。
- 若追踪程序加入断言并复制全部测试，会与现有合同测试职责重复。
- 若顺手接平台执行，会把学习卡升级成 V0.6 实现卡。

### 自检

- `git status --short -uall`
- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_trace.exe`
- `sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
- `git diff --check`
- `rg -n "current_page|power_controller|event_queue|queue_head|queue_tail|queue_count" watch_core/examples/watch_core_contract_trace.c`
- `rg -n "lvgl|SDL|HAL|CubeMX|malloc|free|new|delete|std::" watch_core/examples/watch_core_contract_trace.c`
- 对本轮实际改动的中文 Markdown 做乱码哨兵检查

### 验收标准

- 追踪目标可独立构建运行，输出顺序能够说明状态演化。
- 程序只通过公开 API 观察 Core。
- 原合同测试继续通过。
- 用户能逐段解释每次调用为什么属于输入、动作、状态读取或 commit。
- 新程序正式入库，但不进入产品目标。

### Doc Impact

`small`

### 建议提交信息

`test: add watch core contract trace example`

### 执行记录

- 2026-06-13：卡片创建，待 `LEARN-B` 验收后执行。

### Stop policy

- 用户完成首次代码后停止，由 Agent 做代码审查；修正并验收后再提交，不自动进入 `LEARN-D`。

## 卡片 LEARN-D 架构复盘与 V0.6 决策门

- ID：`LEARN-D`
- 标题：架构复盘与 V0.6 决策门
- 批次：架构掌控力恢复学习轮
- 状态：TODO
- 依赖：`LEARN-C`

### 问题定位

完成阅读和代码练习后，还需要把理解转化为下一阶段决策能力。否则学习结果仍然停留在“能复述当前代码”，不能用于判断 V0.6 第一张实现卡的边界。

### 实施方案

1. 用户在工作簿中独立回答七个复盘问题。
2. Agent 根据代码事实审查答案，指出所有权、依赖方向或扩展判断中的偏差。
3. 收口一份学习结论，明确：
   - 当前真实分层。
   - 平台共享与分叉边界。
   - V0.6 唯一首要缺口。
   - F411 熄屏态表冠旋转和按下规则。
4. 只在学习结论通过后，规划一张独立的 V0.6 第一实现卡；该卡不得在本轮执行。

### 涉及位置

Allowed files

- `docs/20_guides/architecture_control_learning_workbook.md`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/cards/architecture-control-learning-q12.md`
- `docs/40_workflow/agent_batch/cards/v0-6-platform-power-executor-q13.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/document_map.md`

Read-only files

- `watch_core/**`
- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/**`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/**`
- `docs/50_stage_records/v0_5_core_contract_learning_closure.md`

Forbidden changes

- 不修改任何运行代码、测试、工程文件或平台实现。
- 不执行 V0.6 第一实现卡。
- 不把未经用户作答和审查的结论写成已经掌握。
- 不把整个 V0 或三平台闭环写成完成。

### 风险

- 若复盘只由 Agent 输出，无法判断用户是否重新获得项目掌控力。
- 若复盘和 V0.6 实现混在一起，会再次跳过决策门。

### 自检

- `git status --short -uall`
- `git diff --check`
- 检查七个复盘问题均有用户答案和 Agent 审查结论
- 检查 V0.6 卡片字段完整但状态仍为 `TODO`
- 对本轮实际改动的中文 Markdown 做乱码哨兵检查

### 验收标准

- 用户能独立回答七个复盘问题，并能用代码证据支撑。
- 已明确 V0.6 第一实现卡做什么、不做什么。
- 已形成正式 V0.6 卡片，但没有进入实现。
- 学习工作簿成为按需开发指南，不进入默认新会话入口。

### Doc Impact

`required`

### 建议提交信息

`docs: close architecture control learning round`

### 执行记录

- 2026-06-13：卡片创建，待 `LEARN-C` 验收后执行。

### Stop policy

- 本卡完成后停止，等待用户确认是否进入 V0.6。
