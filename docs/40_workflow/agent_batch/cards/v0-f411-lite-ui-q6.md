# V0.4R F411 Lite UI 垂直闭环

## 卡片 V0.4R-B F411 编译接入 `watch_core` public contract

- ID：`V0.4R-B`
- 标题：F411 编译接入 `watch_core` public contract
- 状态：DONE
- 依赖：`V0.3-D`

### Problem

`V0.4R` 需要先证明 F411 工程可以只接入 `UiEvent`、Coordinator、`PageIntent`、`UiModelSnapshot` 等公共合同，而不把 PC 目标里的 SDL、C++、文件路径或运行时 XML 假设带进真机构建。

### Implementation plan

1. 审计 `watch_core` 当前 public contract 是否存在 LVGL、SDL、C++、`malloc`、文件路径或 PC-only 假设。
2. 在不创建 F411 UI 文件的前提下，把最小公共头和最小实现接入 F411 工程编译链。
3. 只解决编译接线和边界问题，不做 UI、不做输入、不做行为联调。
4. 记录编译结果、边界检查结果和未解决风险。

### Allowed files

- `watch_core/**`
- `try/my_watch_f411_v2.1/**`
- `docs/40_workflow/agent_batch/cards/v0-f411-lite-ui-q6.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/30_testing/**`，仅在需要记录编译/边界检查结果时允许新增或更新

### Read-only files

- `docs/00_current/project_brief.md`
- `docs/00_current/current_decisions.md`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `ui/**`
- `sim/**`

### Forbidden changes

- 不创建 F411 UI View 文件。
- 不启用 `LV_USE_IMG`。
- 不修改 DMA、flush、draw buffer、PNG、FS、W25Q128 路线。
- 不把 `watch_core` 改成依赖 LVGL、HAL、SDL 或 C++。
- 不提前实现 `V0.4R-C` 及后续卡片内容。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `rg -n "lvgl|SDL|malloc|free|new|delete|std::|\\.png|asset_path" watch_core`
- Keil / MDK 编译结果由用户执行并回填

### Acceptance checklist

- 只接入 `UiEvent`、Coordinator、`PageIntent`、`UiModelSnapshot` 等公共合同。
- 未创建 F411 UI，也未启用输入或页面逻辑。
- 编译问题若存在，已明确是合同边界问题还是工程接线问题。
- 未执行的真机或编译项保留为未验证。

### Risks

- `watch_core` 现状可能仍混入 PC 假设，导致本卡需要先做最小边界收缩。
- F411 工程编译系统可能暴露 include 路径或 C/C++ 混编问题。

### Doc Impact

`small`

### Suggested commit message

`f411: wire watch core public contract`

### Execution record

- 2026-06-11：已完成第一轮静态边界审计，`watch_core` 未扫出明显的 LVGL、SDL、heap、PNG 路径或 C++ 依赖。
- 2026-06-11：已在 F411 工程内新增 `watch_core_bridge`，仅做 `watch_core_init()`、`watch_core_get_ui_snapshot()`、`watch_core_make_back_event()`、`watch_core_push_event()`、`watch_core_process_next_event()` 的最小合同级接线，不创建 UI、不接输入。
- 2026-06-11：已更新 MDK 工程 include path，并把 `watch_core/src/watch_core.c` 纳入 F411 工程文件。
- 2026-06-11：用户已确认 Keil / MDK 编译通过。本卡验收结论为：F411 已能接入 `watch_core` public contract，当前未暴露 PC-only 合同污染或基础工程接线错误。

### Stop policy

- 若必须创建 F411 UI、修改 LVGL 配置或引入 PC-only 依赖才能通过，立即停止并标记 BLOCKED。

---

## 卡片 V0.4R-C F411 LVGL 8.2 四卡静态 Lite View

- ID：`V0.4R-C`
- 标题：F411 LVGL 8.2 四卡静态 Lite View
- 状态：DONE
- 依赖：`V0.4R-B`

### Problem

在不接 `watch_core`、不接输入的前提下，需要先证明 F411 当前 LVGL 8.2、DMA flush 和 20 行 draw buffer 基线下，四卡静态 Lite View 可以独立显示，并且不重新引入 `LV_USE_IMG` 或 XML 产物依赖。

### Implementation plan

1. 用 `lv_obj`、`lv_label`、色块和边框手写四卡静态页面。
2. 保持当前 DMA、flush、draw buffer 和 debug 基线不变。
3. 不接 `watch_core`、不接输入，只验证页面装配和显示稳定性。
4. 记录静态页面显示结果和观测边界。

### Allowed files

- `try/my_watch_f411_v2.1/**`
- `docs/40_workflow/agent_batch/cards/v0-f411-lite-ui-q6.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/30_testing/**`，仅在需要记录真机静态页面观测时允许新增或更新

### Read-only files

- `watch_core/**`
- `ui/**`
- `sim/**`
- `docs/00_current/project_brief.md`
- `docs/00_current/current_decisions.md`

### Forbidden changes

- 不接 `watch_core`。
- 不接输入。
- 不启用 `LV_USE_IMG`。
- 不修改 DMA、flush、draw buffer、PNG、FS。
- 不提前实现 `V0.4R-D` 的语义闭环。

### Self-check

- `git status --short -uall`
- `git diff --check`
- Keil / MDK 编译结果由用户执行并回填
- 真机静态显示观察由用户执行并回填

### Acceptance checklist

- 四卡静态 Lite View 可显示。
- 只使用 `lv_obj`、`lv_label`、色块和边框。
- 当前 DMA 和 draw buffer 基线未被改动。
- 未执行的真机项保留为未验证。

### Risks

- 即使不接 core，页面装配仍可能暴露 F411 现有 UI 组织方式的耦合。
- debug screen 或现有 demo 入口可能需要最小接线调整。

### Doc Impact

`small`

### Suggested commit message

`f411: add lite static health cards view`

### Execution record

- 2026-06-11：已复用现有 `watch_lvgl_debug_screen` 入口，将其临时切换为静态四卡 Lite View，避免新增复杂 bring-up 路由。
- 2026-06-11：当前页面只使用 `lv_obj`、`lv_label`、色块和边框，未接入 `watch_core`、未接入输入语义、未启用 `LV_USE_IMG`。
- 2026-06-11：`watch_lvgl_debug_screen_on_input_intent()` 和 `watch_lvgl_debug_screen_task()` 当前为 no-op，确保本卡只验证静态页面装配，不提前进入 `V0.4R-D`。
- 2026-06-11：用户已确认可正常编译、烧录和显示四卡静态 Lite View。本卡验收结论为：F411 LVGL 8.2 静态四卡页面装配成立，当前 DMA 与 draw buffer 基线未被改动。

### Stop policy

- 若必须启用 `LV_USE_IMG`、修改 DMA 基线或引入 XML 生成物才能显示，立即停止并标记 BLOCKED。

---

## 卡片 V0.4R-D `F411UiAdapter` 语义闭环

- ID：`V0.4R-D`
- 标题：`F411UiAdapter` 语义闭环
- 状态：TODO
- 依赖：`V0.4R-C`

### Problem

静态 Lite View 成立后，需要把它接到共享语义合同上，证明 F411 侧可以应用 `UiModelSnapshot`、执行 `PageIntent`，并把 LVGL 点击只转换为 typed `UiEvent`，而不在平台侧复制业务状态机或导航逻辑。

### Implementation plan

1. 引入 `F411UiAdapter`，接收 `UiModelSnapshot` 并刷新 Lite View。
2. 让页面切换只响应 Coordinator 给出的 `PageIntent`。
3. 让 LVGL 点击只转换为 typed `UiEvent`，不在 UI 层直接决定业务导航。
4. 实现四卡、占位详情和 Back 的最小语义闭环。

### Allowed files

- `watch_core/**`
- `try/my_watch_f411_v2.1/**`
- `docs/40_workflow/agent_batch/cards/v0-f411-lite-ui-q6.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/30_testing/**`，仅在需要记录语义闭环观察时允许新增或更新

### Read-only files

- `ui/**`
- `sim/**`
- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `docs/00_current/current_decisions.md`

### Forbidden changes

- 不新增业务状态机副本。
- 不让 LVGL 点击直接决定页面跳转。
- 不启用 `LV_USE_IMG` 作为本卡前置条件。
- 不修改 DMA、flush、draw buffer、PNG、FS 主链。
- 不提前执行 `V0.4R-E` 验收收口。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `rg -n "lvgl|HAL|SDL" watch_core`
- Keil / MDK 编译结果由用户执行并回填
- 真机语义行为观察由用户执行并回填

### Acceptance checklist

- `UiModelSnapshot` 被用于刷新 F411 Lite View。
- `PageIntent` 决定页面切换。
- LVGL 点击只转换成 typed `UiEvent`。
- 四卡、占位详情和 Back 构成最小闭环。
- 未执行的真机项保留为未验证。

### Risks

- 容易在“先跑起来”压力下把导航逻辑偷放到 UI 层。
- `watch_core` 合同与 F411 页面组织之间可能出现命名或生命周期不匹配。

### Doc Impact

`required`

### Suggested commit message

`f411: bridge lite ui with watch core semantics`

### Execution record

- 未开始。

### Stop policy

- 若要靠平台直接解释业务、复制状态机或绕过 Coordinator 才能闭环，立即停止并标记 BLOCKED。

---

## 卡片 V0.4R-E 真机验收与指标收口

- ID：`V0.4R-E`
- 标题：真机验收与指标收口
- 状态：TODO
- 依赖：`V0.4R-D`

### Problem

语义闭环成立后，需要单独收口真机编译、Flash/RAM、刷新区域与 flush、静态稳定性、输入压力和人工验收结果，避免把未执行项写成通过。

### Implementation plan

1. 整理本阶段需要用户回填的真机观测口径。
2. 记录编译结果、Flash/RAM、刷新区域与 flush 指标。
3. 记录静态稳定性、输入压力和四卡/详情/Back 人工验收。
4. 将已验证、未验证和风险项分开收口。

### Allowed files

- `docs/30_testing/**`
- `docs/40_workflow/agent_batch/cards/v0-f411-lite-ui-q6.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `try/my_watch_f411_v2.1/README.md`，仅在需要同步真机验收口径时允许更新

### Read-only files

- `watch_core/**`
- `ui/**`
- `sim/**`
- `try/my_watch_f411_v2.1/**`，除 `README.md` 外其余代码文件只读

### Forbidden changes

- 不再扩功能。
- 不借验收名义修改 DMA、LVGL 配置、FS、PNG 或图标路径。
- 不把未执行的编译、真机或人工项写成通过。

### Self-check

- `git status --short -uall`
- `git diff --check`
- 真机编译和人工验收由用户执行并回填

### Acceptance checklist

- 编译结果、Flash/RAM、刷新区域与 flush 已分别记录。
- 静态稳定性和输入压力已分别记录。
- 四卡、详情和 Back 人工验收已分别记录。
- 未执行项明确保留。

### Risks

- 真机验收口径容易和上一阶段刷新探针口径混淆。
- 如果本卡夹带修代码，会破坏“验收收口”定位。

### Doc Impact

`required`

### Suggested commit message

`docs: record f411 lite ui validation baseline`

### Execution record

- 未开始。

### Stop policy

- 若需要修改代码才能继续验收，停止本卡并回到新的实现卡，不在本卡内扩 scope。

---

## 卡片 V0.4R-C2 可选 32x32 C 数组图标探针

- ID：`V0.4R-C2`
- 标题：可选 32x32 C 数组图标探针
- 状态：TODO
- 依赖：`V0.4R-C`

### Problem

主链默认不启用 `LV_USE_IMG`。若后续需要判断小尺寸 C 数组图标是否可接受，应单独做可选探针，避免把图标资源问题重新绑回主闭环。

### Implementation plan

1. 独立开启 `LV_USE_IMG` 并只接一组 32x32 C 数组图标。
2. 记录 Flash/RAM、刷新面积和 flush 增量。
3. 只评估增量成本，不改变 `V0.4R` 主闭环定义。

### Allowed files

- `try/my_watch_f411_v2.1/**`
- `docs/30_testing/**`
- `docs/40_workflow/agent_batch/cards/v0-f411-lite-ui-q6.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

### Read-only files

- `watch_core/**`
- `ui/**`
- `sim/**`
- `docs/40_workflow/agent_batch/agent-queue.md`

### Forbidden changes

- 不把本卡并入默认主队列。
- 不因为图标探针失败阻塞 `V0.4R-D/E`。
- 不扩大到 PNG、FS、W25Q128 主链验证。

### Self-check

- `git status --short -uall`
- `git diff --check`
- Keil / MDK 编译结果由用户执行并回填
- 真机资源和刷新观测由用户执行并回填

### Acceptance checklist

- 只接一组 32x32 C 数组图标。
- 已记录 Flash/RAM 和刷新增量。
- 失败不会阻塞 `V0.4R` 主闭环。
- 未执行项保留为未验证。

### Risks

- 一旦 scope 漂移，容易重新打开 PNG/FS 旧路线。
- `LV_USE_IMG` 开启后可能带来额外内存与配置影响。

### Doc Impact

`small`

### Suggested commit message

`f411: probe lite c-array icons`

### Execution record

- 未开始。

### Stop policy

- 若探针开始影响主闭环定义或需要接入 PNG/FS，立即停止并单独重评。
