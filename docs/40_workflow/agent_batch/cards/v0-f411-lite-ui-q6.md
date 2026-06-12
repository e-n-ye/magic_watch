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
- 状态：DONE
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

- 2026-06-11：已把旧 `screen_manager + app_event_queue` 从当前 Lite UI 路线旁路，避免与共享 `watch_core` 页面意图并行决策。
- 2026-06-11：已在现有四卡 Lite View 上接入 `watch_core_bridge`，开始应用 `UiModelSnapshot`、分发 typed `UiEvent`、响应 `PageIntent`。
- 2026-06-12：根据用户确认，本卡当前先收敛到“表冠旋转选卡 + 短按进入 / 短按 Back 按钮返回”的最小闭环；物理 `Back` 键和左边缘右滑返回留到后续独立触摸输入卡，不在本卡内扩 scope。
- 2026-06-12：用户已确认 Keil / MDK 编译通过；表冠旋转选卡、短按进入详情、详情页短按屏上 `Back` 返回均正常；未观察到花屏、卡死或停更。本卡验收结论为：F411 当前已形成基于共享 `watch_core` 合同的表冠最小语义闭环。

### Stop policy

- 若要靠平台直接解释业务、复制状态机或绕过 Coordinator 才能闭环，立即停止并标记 BLOCKED。

---

## 卡片 V0.4R-D2 F411 touch pointer 接入

- ID：`V0.4R-D2`
- 标题：F411 touch pointer 接入
- 状态：DONE
- 依赖：`V0.4R-D`

### Problem

当前 `V0.4R` 主链只有 encoder 输入，没有把 F411 现有触摸屏接到 Lite UI 语义闭环上。需要先证明当前工程能以最小改动接入 `CST816` 触摸 BSP 和 `LV_INDEV_TYPE_POINTER`，并让触摸点击卡片进入详情，而不把左边缘右滑 Back、手势歧义处理或更多 UI 行为揉进同一轮。

### Implementation plan

1. 审计当前 F411 工程中已有的 `TP_RST/TP_SDA/TP_SCL`、`MX_I2C1`、GPIO 配置和可复用旧代码来源。
2. 从 `old_watch` 借鉴最小 `bsp_touch` / `lv_port_indev` 方案，只下放本轮必需的触摸读取与 pointer 注册能力。
3. 将 touch pointer 接到当前 Lite UI 主链，使触摸点击四卡可进入详情。
4. 保持表冠最小闭环不被破坏，不在本卡实现左边缘右滑 Back。

### Allowed files

- `try/my_watch_f411_v2.1/**`
- `docs/40_workflow/agent_batch/cards/v0-f411-lite-ui-q6.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/30_testing/**`，仅在需要记录 touch pointer 观察口径时允许新增或更新

### Read-only files

- `watch_core/**`
- `ui/**`
- `sim/**`
- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `D:/MY_Desk/watch/old_watch/my_watch_f411_v2.1/User/**`

### Forbidden changes

- 不实现左边缘右滑 Back。
- 不修改 DMA、flush、draw buffer、PNG、FS 主链。
- 不新增第二套业务状态机或页面决策逻辑。
- 不整搬 `old_watch` 全量 BSP/APP/Power 体系。

### Self-check

- `git status --short -uall`
- `git diff --check`
- `rg -n "TP_RST|TP_SDA|TP_SCL|MX_I2C1|bsp_touch|LV_INDEV_TYPE_POINTER" try/my_watch_f411_v2.1`
- Keil / MDK 编译结果由用户执行并回填
- 真机 touch 点击观察由用户执行并回填

### Acceptance checklist

- 当前工程已接入最小 `CST816` 触摸读取链路。
- 当前工程已注册 `LV_INDEV_TYPE_POINTER`。
- 触摸点击四卡可进入详情。
- 表冠旋转/短按最小闭环未被破坏。
- 未执行的真机项保留为未验证。

### Risks

- `bsp_touch` 依赖旧 `hal_i2c_bus` / `hal_delay`，若直接搬运可能把无关层级一并引入。
- 当前板级触摸引脚虽然已在 `main.h` / `gpio.c` 中出现，但真实连线、极性和轮询时序仍可能与旧工程不同。

### Doc Impact

`required`

### Suggested commit message

`f411: add touch pointer ingress for lite ui`

### Execution record

- 2026-06-12：已确认当前工程存在 `TP_RST/TP_SDA/TP_SCL` 引脚定义与 GPIO 初始化，且 `old_watch` 中有 `CST816` 的 `bsp_touch` 与 `LV_INDEV_TYPE_POINTER` 旧实现可供最小借鉴。
- 2026-06-12：已明确本卡只接 touch pointer 和点击进入详情，不把左边缘右滑 Back 混入同一轮。
- 2026-06-12：当前实现路径收敛为“现工程内新增最小 `watch_touch_hw` 读链 + 在既有 `watch_lvgl_port` 上增量注册 `LV_INDEV_TYPE_POINTER`”，避免引入旧工程整套 input / power / HAL 包装层。
- 2026-06-12：用户已确认编译通过；触摸四卡可进入详情、触摸 `Back` 可返回四卡、表冠原有闭环保持正常，且未观察到花屏、卡死、停更。
- 2026-06-12：验收中暴露新的全局语义缺口：按住后滑动再松手仍可能触发卡片进入或 `Back` 返回。该问题已拆到独立卡 `V0.4R-D2B`，不回滚本卡“pointer 已接通”的结论。

### Stop policy

- 若必须整搬旧工程的 APP / Power / 全量 HAL 包装层才能读到触摸，立即停止并把触摸 BSP 依赖拆成新的更小卡。

---

## 卡片 V0.4R-D2B F411 tap-only 防误触语义

- ID：`V0.4R-D2B`
- 标题：F411 tap-only 防误触语义
- 状态：DONE
- 依赖：`V0.4R-D2`

### Problem

`V0.4R-D2` 已证明 F411 touch pointer 可以接通，但用户实测发现“按下后滑动再松手”仍会误触发卡片进入详情和 `Back` 返回。这说明当前系统还没有形成稳定的 tap-only 语义，不能直接进入左边缘右滑 Back。

### Implementation plan

1. 在当前 F411 touch pointer 主链上记录一次触摸是否超过 tap 位移阈值。
2. 让四卡进入详情和详情页 `Back` 只接受“合法 tap”触发的 pointer 激活事件。
3. 保持表冠点击语义不受影响。
4. 不在本卡内实现左边缘右滑 Back，只把 swipe 与 tap 的边界先收清楚。

### Allowed files

- `try/my_watch_f411_v2.1/**`
- `docs/40_workflow/agent_batch/cards/v0-f411-lite-ui-q6.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

### Read-only files

- `watch_core/**`
- `sim/**`
- `ui/**`

### Forbidden changes

- 不提前实现 `V0.4R-D3` 左边缘右滑 Back。
- 不修改 DMA、flush、draw buffer。
- 不把防误触逻辑下沉为新的业务状态机。
- 不引入 PC 专用输入假设。

### Self-check

- `git status --short -uall`
- `git diff --check`
- Keil / MDK 编译结果由用户执行并回填
- 真机 tap / slide 行为由用户执行并回填

### Acceptance checklist

- 轻触卡片可进入详情。
- 按住卡片后滑动再松手，不进入详情。
- 轻触 `Back` 可返回四卡。
- 按住 `Back` 后滑动再松手，不返回四卡。
- 表冠原有闭环不回退。
- 未执行的真机项保留为未验证。

### Risks

- 若阈值过小，正常轻触会被误判成 slide。
- 若阈值过大，防误触收益会不足。

### Doc Impact

`small`

### Suggested commit message

`f411: guard lite ui activations behind tap-only touch`

### Execution record

- 2026-06-12：根据用户真机反馈新增本卡；问题不是“touch 不通”，而是“touch 语义未收口”，因此应独立于 `V0.4R-D2` 和 `V0.4R-D3` 验收。
- 2026-06-12：已开始在 F411 当前 pointer 输入层加入 tap 位移阈值与 activation 过滤，目标是让卡片和 `Back` 只接受合法 tap，不影响 encoder 激活链。
- 2026-06-12：用户已确认 `D2B` 该测内容全部通过：轻触卡片可进入详情，按住后滑动再松手不会误入详情；轻触 `Back` 可返回四卡，按住后滑动再松手不会误返回；表冠原闭环正常，未观察到花屏、卡死或停更。

### Stop policy

- 若必须提前把 swipe back 一起实现才能验证 tap-only，立即停止并重新拆卡。

---

## 卡片 V0.4R-D3 F411 左边缘右滑 Back

- ID：`V0.4R-D3`
- 标题：F411 左边缘右滑 Back
- 状态：DONE
- 依赖：`V0.4R-D2B`

### Problem

在 touch pointer 稳定后，需要把你定义的“左边缘右滑返回”接到当前共享 `watch_core` 语义上。但这一步既涉及 `CST816` 手势寄存器，又涉及“左边缘约束”的产品语义，必须和 touch pointer 上板分开验收。

### Implementation plan

1. 审计 `CST816` 当前可提供的是离散 gesture、坐标，还是同时可可靠表达“左边缘起手”。
2. 在不破坏点击和表冠闭环的前提下，实现左边缘右滑触发 `Back`。
3. 若硬件 gesture 语义不足，再做最小软件补充判定；但不得把复杂触摸状态机扩进平台层。

### Allowed files

- `try/my_watch_f411_v2.1/**`
- `docs/40_workflow/agent_batch/cards/v0-f411-lite-ui-q6.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`
- `docs/30_testing/**`，仅在需要记录手势观察口径时允许新增或更新

### Read-only files

- `watch_core/**`
- `ui/**`
- `sim/**`
- `D:/MY_Desk/watch/old_watch/my_watch_f411_v2.1/User/**`

### Forbidden changes

- 不为了手势回退到物理 `Back` 键主链。
- 不修改 DMA、flush、draw buffer、PNG、FS 主链。
- 不把手势判定升级成通用复杂触摸状态机框架。

### Self-check

- `git status --short -uall`
- `git diff --check`
- Keil / MDK 编译结果由用户执行并回填
- 真机左边缘右滑观察由用户执行并回填

### Acceptance checklist

- 左边缘右滑可触发 `Back`。
- 触摸点击进入详情仍可用。
- 表冠最小闭环仍可用。
- 未执行的真机项保留为未验证。

### Risks

- `CST816` 给出的 gesture 可能只有“向右滑”而没有足够稳定的“左边缘起手”语义。
- 若软件补判定过重，容易把平台层变成手势业务状态机。

### Doc Impact

`required`

### Suggested commit message

`f411: add lite ui swipe back semantics`

### Execution record

- 2026-06-12：已开始实现最小 `D3` 闭环；当前策略不依赖 LVGL 自身 `GESTURE` 事件，而是在 F411 当前 touch 输入层内记录“左边缘起手 + 向右滑”并由 detail 页任务消费为 `Back` 语义。
- 2026-06-12：根据用户真机反馈，当前 `D3` 已追加参考模拟器做法：左边缘右滑提交条件向“允许拇指弧线”的方向放宽，并在 detail 页补入最小进度提示与轻微跟手位移，避免用户在滑动时完全无反馈。
- 2026-06-12：基于用户二轮真机反馈，已继续做 `D3` 手感微调：在保持左边缘起手约束不变的前提下，略微缩短右滑提交距离，目标是让“滑过屏幕中线偏右”即可更自然地完成返回。
- 2026-06-12：用户已确认当前版本编译通过；进入详情页后，从左边缘右滑可看到左侧临时提示和轻微跟手位移；大拇指弧线右滑比上一版更容易触发返回；中间起手右滑不会误返回；四卡首页左边缘右滑不会出现异常返回或异常跳转；轻触进入详情、轻触 `Back`、tap-only 防误触和表冠原闭环均保持正常；未观察到花屏、卡死或停更。最终手调 `WATCH_TOUCH_SWIPE_BACK_COMMIT_DISTANCE = 36U`，本卡验收通过。

### Stop policy

- 若无法在当前硬件/驱动能力下可靠判定“左边缘右滑”，立即停止并把结果写成能力缺口，不伪造通过。

---

## 卡片 V0.4R-E 真机验收与指标收口

- ID：`V0.4R-E`
- 标题：真机验收与指标收口
- 状态：DONE
- 依赖：`V0.4R-D3`

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

- 2026-06-12：本卡已启动，当前只进入“验收口径整理与用户回填”阶段，不修改代码、不提前写通过结论。
- 2026-06-12：用户已确认 Keil / MDK 编译通过，编译产物为 `Code=114036`、`RO-data=46624`、`RW-data=820`、`ZI-data=100364`，`FromELF` 生成 hex 正常，构建结果为 `0 Error(s), 80 Warning(s)`。
- 2026-06-12：用户已确认当前四卡、详情、屏上 `Back`、左边缘右滑 `Back`、表冠原闭环等人工功能验收正常；稳定性观察正常，未反馈花屏、卡死或停更。
- 2026-06-12：本轮未能记录“静态 60 秒”和“输入压力 120 秒”的刷新区域与 flush 指标。原因不是未执行回填，而是当前屏幕已无任何调试项显示，无法观测 `calls/s`、`full/s`、`pixels/s`、`area px`、`area %`、`fps/ms` 等字段。本卡将这些项目如实保留为“未验证”，不写成通过。
- 2026-06-12：本卡验收结论为：`V0.4R` 主链在真机上已完成功能与稳定性收口，并记录了当前一次真实构建占用；刷新区域 / flush 数值型观测因当前 UI 口径不可见而暂缺，需后续若有专门观测入口再独立补测。

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
