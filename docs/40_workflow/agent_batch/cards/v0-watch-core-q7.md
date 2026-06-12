# V0.5 共享合同护栏与 F411 所有权收口

## 卡片 V0.5-P0-A 共享合同测试与 F411 所有权审计

- ID：`V0.5-P0-A`
- 标题：共享合同测试与 F411 所有权审计
- 状态：DONE
- 依赖：`V0.4R-E`

### Problem

进入 V0.5 前，路线图仍把 `EventQueue`、Coordinator、Snapshot 等模块写成“尚未实现”，但当前仓库里 `watch_core` 已经有最小事件队列、页面状态和 Coordinator 行为，F411 也已形成共享合同下的真机闭环。如果直接按旧路线继续，很容易重复造轮子或错判下一张卡重点。

### Implementation plan

1. 为现有 `watch_core` 建立纯 PC、无 LVGL 的语义合同测试。
2. 只读审计 F411 当前新旧主链的状态所有权。
3. 区分“被编译”“被初始化”“被调用”“当前运行主链实际使用”。
4. 基于证据修正 V0.5 后续拆分建议，并只给出一张第二卡建议。

### Allowed files

- `watch_core/tests/**`
- `sim/lv_port_pc_vscode/CMakeLists.txt`
- `docs/30_testing/**`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/40_workflow/agent_batch/cards/v0-watch-core-q7.md`

### Read-only files

- `watch_core/include/watch_core/watch_core.h`
- `watch_core/src/watch_core.c`
- `sim/lv_port_pc_vscode/src/XmlUi/**`
- `try/my_watch_f411_v2.1/**`
- 其他当前架构和决策文档

### Forbidden changes

- 不修改 `watch_core` 公共 API、数据结构或运行实现。
- 不修改 PC UI Adapter。
- 不修改任何 F411 代码或 MDK 工程。
- 不抽取 `F411UiAdapter`。
- 不移除旧 ScreenManager 或 EventQueue。
- 不新增 `SystemEvent`、`PowerState`、`PowerController`。
- 不恢复性能 overlay。
- 不修改 DMA、flush、draw buffer、触摸阈值和 UI。
- 不开始第二张卡。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- `D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
- `rg -n "lvgl|SDL|HAL|CubeMX|malloc|free|new|delete|std::" watch_core`
- 对本轮实际修改的中文 Markdown 做乱码哨兵检查

### Acceptance checklist

- 独立 PC 合同测试目标可以构建并运行。
- 十类行为均有明确结果。
- 测试不依赖 LVGL、SDL、HAL 或 F411。
- F411 所有权审计明确区分新旧路径的编译状态和运行状态。
- 已明确推荐第二张卡方向。
- 路线图不再把已有 EventQueue/Coordinator 当作尚未实现的空白模块。

### Risks

- 旧工程残留可能让“被编译”与“仍掌权”混淆。
- 若合同测试直接写死内部字段，会把实现细节误升格为长期合同。

### Doc Impact

`required`

### Suggested commit message

`test: add watch core contract guardrails`

### Execution record

- 2026-06-12：已新增 `watch_core/tests/watch_core_contract_test.c`，以纯 C 方式覆盖“通过公开行为证明默认页面语义”、页面跳转、Back 语义、空队列、非法 feature、FIFO、满队列拒绝、Snapshot 更新和字符串截断。
- 2026-06-12：已在 `sim/lv_port_pc_vscode/CMakeLists.txt` 中新增 `magic_watch_core_contract_test` 目标，但未让测试依赖 LVGL、SDL、HAL 或 F411。
- 2026-06-12：`cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过；`magic_watch_core_contract_test.exe` 运行结果为 `watch_core contract tests passed.`。
- 2026-06-12：已输出 `docs/30_testing/v0_5_watch_core_contract_and_f411_ownership_audit.md`，明确当前 F411 运行主链实际由 `watch_core_bridge + watch_lvgl_debug_screen + watch_lvgl_port` 共同承担事实上的 Adapter 职责；旧 `watch_event_queue` / `watch_screen_manager` 虽仍被编译，但未初始化、未调用、未进入当前主链。
- 2026-06-12：基于审计，第二张卡建议改为“抽取真正的 `F411UiAdapter`”，而不是优先隔离旧 `ScreenManager`。

### Stop policy

- 若测试暴露真实 `watch_core` 缺陷，只记录失败用例和建议，停止本卡，不顺手修实现。

---

## 卡片 V0.5-P0-B 抽取真正的 `F411UiAdapter`

- ID：`V0.5-P0-B`
- 标题：抽取真正的 `F411UiAdapter`
- 状态：TODO
- 依赖：`V0.5-P0-A`

### Problem

当前 F411 侧已经存在事实上的 Adapter 职责组合，但职责散落在 `watch_core_bridge.c`、`watch_lvgl_debug_screen.c`、`watch_lvgl_port.c` 三处。继续扩功能前，如果没有把 `typed UiEvent` 派发、Snapshot 应用、`PageIntent` 执行和 view lifecycle 收口到单点边界，后续页面和输入语义很容易重复或泄漏。

### Implementation plan

1. 只根据 `V0.5-P0-A` 的审计结果，定义 F411 当前真正需要的 Adapter 边界。
2. 让 Adapter 负责接收已经形成的输入结果，将 UI 激活或 Back 语义转换成 typed `UiEvent`，驱动 `watch_core`，再获取并应用 `UiModelSnapshot`。
3. 让 Adapter 消费 `PageIntent` 并协调 Lite View 页面表现，形成 F411 当前 UI/Core 的单一装配边界。
4. 保持 `watch_core` 公共合同不变，不顺手扩页面、不改 DMA、不碰电源语义。

### Allowed files

- `try/my_watch_f411_v2.1/**`
- `docs/30_testing/**`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/cards/v0-watch-core-q7.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

### Read-only files

- `watch_core/**`
- `sim/**`
- `ui/**`

### Forbidden changes

- 不修改 `watch_core` API 或运行实现。
- 不扩新页面或新业务功能。
- 不顺手隔离旧 `watch_screen_manager` / `watch_event_queue`。
- 不把 `watch_lvgl_port` 中的硬件读取、indev 注册、原始坐标、tap-only、防误触、左边缘右滑和手感阈值迁入 Adapter。
- 不把 Lite View 的 LVGL 对象创建或具体视觉更新硬塞进 Adapter。
- 不把 bridge、View 和 input port 粗暴合并成单文件。
- 不修改 DMA、flush、draw buffer、触摸阈值或性能 probe。
- 不进入电源语义、`SystemEvent` 或 `PowerController`。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- Keil / MDK 编译结果由用户执行并回填

### Acceptance checklist

- F411 侧形成单点 Adapter 装配边界。
- Snapshot 应用、`PageIntent` 执行和 typed `UiEvent` 派发不再散落。
- Input Port、Adapter、Lite View 的职责边界明确且未互相吞并。
- `watch_core` 公共合同未被改写。
- 未执行的真机项保留为未验证。

### Risks

- 若 scope 漂移，很容易在“收口 Adapter”名义下顺手改 UI、输入驱动或功能页面。
- 若没有先约束边界，可能只是把散落代码挪位置而没有真正降低所有权混乱。

### Boundary

`F411UiAdapter` 应负责：

- 接收已经形成的输入结果。
- 将 UI 激活或 Back 语义转换、派发为 typed `UiEvent`。
- 驱动 `watch_core` 处理事件。
- 获取并应用 `UiModelSnapshot`。
- 消费 `PageIntent` 并协调 Lite View 页面表现。
- 提供 F411 当前 UI/Core 的单一装配边界。

继续属于 `watch_lvgl_port`，本卡不得迁移：

- `CST816` 或其他硬件读取。
- LVGL encoder/pointer indev 注册和喂入。
- 原始坐标处理。
- tap-only 位移判定与防误触。
- 左边缘起手、右滑距离和 swipe commit 判定。
- `WATCH_TOUCH_SWIPE_BACK_COMMIT_DISTANCE = 36U` 等手感阈值。

Lite View 继续负责：

- LVGL 对象创建。
- 标签、卡片、详情面板和手势提示的具体视觉更新。
- 不持有业务状态，不直接决定产品导航。

### Doc Impact

`required`

### Suggested commit message

`f411: extract concrete lite ui adapter boundary`

### Execution record

- 未开始。

### Stop policy

- 若必须同时修改 `watch_core` 合同、电源语义或旧工程残留清理才能前进，立即停止并重新拆卡。
