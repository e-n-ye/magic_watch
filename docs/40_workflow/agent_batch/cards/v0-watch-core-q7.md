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
- 状态：DONE
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

- 2026-06-12：先依据 `P0-A` 审计把迁移表固定为“`watch_core_bridge/watch_lvgl_debug_screen` -> `f411_ui_adapter/watch_lite_view`，`watch_lvgl_port` 保持不变”，避免把输入端口和 Lite View 一起粗暴吞进 Adapter。
- 2026-06-12：新增 `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.c/.h`，由 Adapter 统一持有 `WatchCore`、创建并派发 typed `UiEvent`、拉取 snapshot、消费 `PageIntent`，并以单点方式驱动 Lite View。
- 2026-06-12：新增 `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lite_view.c/.h`，由 Lite View 统一持有 LVGL 对象、应用 snapshot、执行页面视觉切换，并把 LVGL 点击转换成无业务决策的 Adapter 调用。
- 2026-06-12：`watch_bringup.c` 当前只装配 `watch_lvgl_port + f411_ui_adapter`；旧 `watch_core_bridge.*` 和 `watch_lvgl_debug_screen.*` 已从当前活跃路径与 MDK 工程登记移出，不再保留第二套活跃 UI/Core 装配链。
- 2026-06-12：`watch_lvgl_port.c` 未迁入任何 `CST816`、原始坐标、tap-only、swipe 判定或阈值逻辑到 Adapter；`WATCH_TOUCH_SWIPE_BACK_COMMIT_DISTANCE` 仍保持 `36U`。
- 2026-06-12：用户已完成人工验收并确认以下 12 项全部通过：Keil / MDK 编译通过；四卡首页正常显示；表冠旋转切卡正常；表冠短按进入详情正常；触摸点击卡片进入详情正常；触摸点击屏上 Back 返回正常；tap-only 防误触正常；左边缘右滑 Back 正常；中间起手右滑不会误返回；首页左边缘右滑不会异常跳转；无花屏、卡死、停更；`WATCH_TOUCH_SWIPE_BACK_COMMIT_DISTANCE` 仍为 `36U`。

### Stop policy

- 若必须同时修改 `watch_core` 合同、电源语义或旧工程残留清理才能前进，立即停止并重新拆卡。

---

## 卡片 V0.5-P0-C 旧主链构建隔离

- ID：`V0.5-P0-C`
- 标题：旧主链构建隔离
- 状态：IN_PROGRESS
- 依赖：`V0.5-P0-B`

### Problem

`P0-B` 已经把 F411 活跃链路收口到 `Input Port -> F411UiAdapter -> watch_core -> F411UiAdapter -> Lite View`，但旧 `watch_event_queue.*` 和 `watch_screen_manager.*` 仍保留在当前 MDK target 的文件登记里。这会制造“双 EventQueue / 双 ScreenManager 仍被当前目标编译”的架构歧义。

### Implementation plan

1. 审计旧 `watch_event_queue.*` 和 `watch_screen_manager.*` 是否仍被 MDK target 登记。
2. 审计它们是否被任何当前活跃代码 include、初始化或调用。
3. 在证据确认无活跃依赖后，只从 `my_watch_f411.uvprojx` 中移除旧文件登记。
4. 保留旧源码文件不删，并更新审计文档、路线图、队列和进度记录。

### Allowed files

- `try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`
- `try/my_watch_f411_v2.1/README.md`
- `docs/30_testing/v0_5_watch_core_contract_and_f411_ownership_audit.md`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/cards/v0-watch-core-q7.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

### Read-only files

- `try/my_watch_f411_v2.1/user/core/event/watch_event_queue.*`
- `try/my_watch_f411_v2.1/user/app/screen/watch_screen_manager.*`
- `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.*`
- `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lite_view.*`
- `try/my_watch_f411_v2.1/user/ui/lvgl_port/**`
- `watch_core/**`
- `sim/**`

### Forbidden changes

- 不删除旧源码文件。
- 不修改旧模块实现。
- 不修改 `watch_core`。
- 不修改 Adapter、Lite View 或 Input Port。
- 不修改 DMA、flush、draw buffer、触摸算法和阈值。
- 不新增页面、功能或电源状态。
- 不做目录清理或批量删除。
- 不进入 `V0.5-P1-*`。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- `D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/build/out/magic_watch_core_contract_test.exe`
- `rg -n "watch_event_queue.c|watch_event_queue.h|watch_screen_manager.c|watch_screen_manager.h" try/my_watch_f411_v2.1/MDK-ARM/my_watch_f411.uvprojx`

### Acceptance checklist

- 旧 `watch_event_queue` 和 `watch_screen_manager` 不再被当前 MDK target 编译。
- 旧源码仍保留。
- 当前活跃链路仍只有 `Input Port -> F411UiAdapter -> watch_core -> F411UiAdapter -> Lite View`。
- PC 构建和合同测试保持通过。
- 未执行的 Keil / MDK 与真机项保留为未验证。

### Risks

- 若旧模块其实仍有隐藏依赖，机械移出 target 会把问题推迟到 Keil 编译阶段才暴露。
- 若把“停止编译”误写成“删除源码”或“历史功能已不存在”，会丢失预研资产的可追溯性。

### Doc Impact

`required`

### Suggested commit message

`f411: isolate legacy queue and screen manager build entries`

### Execution record

- 2026-06-12：已用 `rg -n "watch_event_queue|watch_screen_manager|watch_screen_id|watch_app_event" try/my_watch_f411_v2.1` 复核，命中只剩旧模块自身声明/实现、MDK 工程登记和审计文档历史文字，未发现活跃链路 include、初始化或函数调用。
- 2026-06-12：已确认 `my_watch_f411.uvprojx` 在 `User/core/event` 与 `User/app/screen` 组下仍登记旧 `watch_event_queue.*` / `watch_screen_manager.*`，这是当前唯一需要隔离的构建残留。
- 2026-06-12：已仅从当前 MDK target 移除上述旧文件登记，源码文件继续保留，不删除、不改实现。
- 2026-06-12：Keil / MDK 编译与真机回归仍待用户回填；在回填前，本卡状态保持 `IN_PROGRESS`。

### Stop policy

- 若发现活跃代码仍 include 或调用旧模块，立即停止，不机械移除。
- 若移除旧模块需要修改 Adapter、Lite View、Input Port 或 `watch_core`，立即停止并重新拆卡。
