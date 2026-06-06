# LVGL XML + watch_core Q1 卡片包

本卡片包用于把当前主线从文档固化推进到第一条可交互垂直闭环：主页健康四卡 XML、PC SDL 验证目标、`watch_core` 最小模型快照与 `UiEvent`。

本包不接真实传感器、BLE、OTA、TinyML，不迁 Zephyr，不把旧 C++ sim 页面系统作为新主线继续扩建。

---

## LVGL-XML-DOC-1 固化 LVGL XML + watch_core 主线文档

- 批次：LVGL-XML-Q0
- 状态：DONE
- 依赖：无
- 自检：
  - `git status --short -uall`
  - `git diff --check`
  - 本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`docs: define lvgl xml watch core roadmap`
- Doc Impact：required

### 问题定位

当前项目入口仍偏向 F411 / C-first 蓝图，未把用户已确认的 `LVGL XML + watch_core + PC/F411 双后端` 路线固化为新会话默认主线。

### 实施方案

1. 新增架构文档和 UML。
2. 更新当前项目简报、当前决策和文档路由。
3. 新增本卡片包并重排执行队列。
4. 不创建工程、不写运行时代码、不改 F411 工程。

### 涉及位置

Allowed files:

- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `docs/10_architecture/lvgl_xml_watch_core_architecture_uml.html`
- `docs/00_current/project_brief.md`
- `docs/00_current/current_decisions.md`
- `docs/document_map.md`
- `docs/40_workflow/agent_batch/cards/lvgl-xml-watch-core-q1.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `docs/10_architecture/f411_future_watch_architecture.md`
- `docs/40_workflow/agent_batch/cards/f411-lvgl-perf-q3.md`
- `sim/lv_port_pc_vscode/**`
- `try/my_watch_f411_v2.1/**`

Forbidden changes:

- 禁止创建 `ui/`、`watch_core/` 或新的构建目标。
- 禁止修改 F411 代码。
- 禁止修改旧 sim 代码。
- 禁止迁 Zephyr 或绑定最终芯片。

### 风险

- 文档路线如果没有进入默认入口和队列，后续 AI 容易回到旧 C++ sim 或直接跳 F411。

### 验收标准

- 新会话能从 `document_map.md` 找到当前 XML/watch_core 主线。
- 队列顺序固定为 `LVGL-XML-Q1` -> `F411-Q3` -> `F411-XML-Q2`。
- `F411-Q3` 不再显示为当前进行中，而是排在 PC 闭环之后。

### 执行记录

- 已完成：本轮已创建架构文档、UML、卡片包，并更新当前入口、决策、路由和队列。
- 自检：`git status --short -uall` 已确认只改文档和卡片文件；`git diff --check` 通过，仅有 LF/CRLF 提示；本轮实际改动中文文档乱码哨兵检查无命中。

---

## LVGL-XML-Q1-1 创建 `ui/lvgl_pro` 工程骨架和 240x280 target

- 批次：LVGL-XML-Q1
- 状态：DONE
- 依赖：`LVGL-XML-DOC-1`
- 自检：
  - `git status --short -uall`
  - Editor preview 能打开项目
  - `git diff --check`
  - 本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: add lvgl pro ui project skeleton`
- Doc Impact：small

### 问题定位

当前仓库尚无 `ui/lvgl_pro`，后续 XML 页面和生成 C 没有稳定落点。

### 实施方案

1. 使用 LVGL Pro App 创建初始工程到 `ui/lvgl_pro`。
2. 建立 240x280 display target。
3. 建立全局 layout token，例如安全区、间距、圆角、字体和触摸尺寸。
4. 保留生成 C 入库策略，不要求构建机现场运行 Editor。

### 涉及位置

Allowed files:

- `ui/lvgl_pro/**`
- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `docs/40_workflow/agent_batch/cards/lvgl-xml-watch-core-q1.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `sim/lv_port_pc_vscode/**`
- `try/my_watch_f411_v2.1/**`
- `watch_core/**`

Forbidden changes:

- 禁止创建 PC 可执行目标。
- 禁止创建 `watch_core`。
- 禁止修改 F411 工程。
- 禁止迁移旧 sim 页面。

### 风险

- LVGL Pro App 生成的初始结构可能包含工具版本差异，必须保留生成文件并检查 diff。

### 验收标准

- `ui/lvgl_pro` 存在。
- 240x280 target 可在 Editor 中预览。
- 项目结构能容纳后续 screens、components、styles 和生成 C。

### 执行记录

- 已完成：确认 `ui/lvgl_pro` 空工程结构，补齐 `watch_240x280` preview target、全局 layout token，并新增 `file_list_gen.cmake` 与 `component_lib_list_gen.cmake` 占位文件，避免后续把工程作为子项目接入时因缺少生成清单直接失败。
- 自检：`git status --short -uall` 已确认工作区同时包含上一张依赖卡 `LVGL-XML-DOC-1` 的未提交文档文件与本轮 `ui/lvgl_pro/**` 骨架文件；Editor 侧目标定义已切换到 `240x280`；`git diff --check` 通过（仅有 LF/CRLF 提示，无 diff 格式错误）；本轮实际改动中文文档乱码哨兵检查通过。

---

## LVGL-XML-Q1-2 实现主页健康四卡 XML 预览

- 批次：LVGL-XML-Q1
- 状态：DONE
- 依赖：`LVGL-XML-Q1-1`
- 自检：
  - `git status --short -uall`
  - Editor preview 正常，无 invalid
  - `git diff --check`
  - 本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: add health shortcut xml screen`
- Doc Impact：small

### 问题定位

首个可视页面需要证明 XML 能组织主页环中的健康快捷页，而不是继续手写旧 LVGL 页面。

### 实施方案

1. 新建健康快捷 screen。
2. 创建心率、血氧、呼吸、心情四张 1/4 卡。
3. 优先使用 Flex、百分比和 layout token，减少绝对坐标。
4. 卡片文字和样式先用静态占位数据。
5. 不接 `watch_core`，不接点击事件链路。

### 涉及位置

Allowed files:

- `ui/lvgl_pro/**`
- `docs/40_workflow/agent_batch/cards/lvgl-xml-watch-core-q1.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `sim/lv_port_pc_vscode/**`
- `watch_core/**`
- `try/my_watch_f411_v2.1/**`

Forbidden changes:

- 禁止实现 PC 目标。
- 禁止实现 `watch_core`。
- 禁止接真实硬件数据。
- 禁止把 XML 直接接硬件或业务服务。

### 风险

- 卡片排布可能在 Editor 看起来正确，但还未证明生成 C 能在 PC 工程编译。

### 验收标准

- 240x280 preview 显示四张健康卡。
- Editor 右侧 preview 正常，不报 invalid。
- XML 结构能清楚区分 styles、view、卡片组件和屏幕组织。

### 执行记录

- 已完成：已新增 `components/health_shortcut_card.xml` 与 `screens/screen_health_shortcuts.xml`，并在 `globals.xml` 补充 240x280 健康四卡所需的尺寸、颜色和标题样式 token；当前文件结构已经清楚区分全局 styles、卡片 component 和 screen 组织。
- 已完成：根据参考图纠偏后，已把组件收敛为“顶部单图标 + 底部一行关键信息”的极简卡片，并把 screen 改成固定 2x2 卡片舞台，去掉导致滚动的顶部说明与列表式布局。
- 已完成：已把 `assets/generated_icons/` 中的四个健康图标复制到 `ui/lvgl_pro/images/` 并在 `globals.xml` 注册为图片资源；卡片组件改为使用 `icon_src` 图片属性，不再用文字假图标。
- 已完成：已保留 `card_id` 作为后续交互接线标识，组件根节点继续使用 `lv_button` 以保留可点击边界；但为避免 Editor preview 持续报 callback not found，本卡阶段暂不在 XML 里直绑 `magic_watch_health_card_clicked`，留待 `Q1-4` 接 `UiEvent` 时再恢复真实回调。
- 已完成：用户侧验证显示早期 XML 工作流不可靠：screen 与 component 职责没有收敛，`card_bg` 不能稳定手调，图标也未正常显示；因此新增 `.agents/skills/lvgl-xml-workflow`，把 LVGL XML 语法、组件边界、样式、图片和事件规则沉淀后再返工健康四卡。
- 已完成：已按新 skill 返工 `health_shortcut_card`：组件保留卡片尺寸、圆角、padding、内部图标和数值位置；screen 只负责实例化四张卡、传入 `card_id` / `icon_src` / `metric_text`，并通过官方例程已验证的 `style_bg_color` 直接覆盖卡片颜色。
- 已完成：已放弃未在本地 examples/tutorials 中找到可靠样例的 `scale_x` / `scale_y` 主路径，改为生成并注册 `health_*_64.png`，让四卡先走 `globals.xml` + `<lv_image src="..."/>` 的已验证图片路径。
- 已完成：用户已在 LVGL Pro Editor 侧确认 240x280 preview 四卡可见，并已生成代码；卡片背景、页面背景和呼吸卡颜色已收敛为全局 color token，screen 不再散写这些裸色值。
- 自检：`git status --short -uall` 仅出现本轮允许文件；`git diff --check` 通过（仅有 LF/CRLF 提示，无 diff 格式错误）；本轮实际改动中文文档乱码哨兵检查通过；本机未找到可调用的 LVGL Pro CLI，也未直接驱动 Editor GUI，Editor 预览验收以用户侧确认为准。

---

## LVGL-XML-Q1-3 新增 `magic_watch_xml_sim` PC 验证目标

- 批次：LVGL-XML-Q1
- 状态：DONE
- 依赖：`LVGL-XML-Q1-2`
- 自检：
  - `git status --short -uall`
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - 新目标能启动并显示 XML 生成 UI，或如本机无法运行则记录原因
  - `git diff --check`
  - 本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: add xml ui pc simulator target`
- Doc Impact：small

### 问题定位

Editor preview 只能证明 UI 预览，不足以证明生成 C 能进入真实工程、接收输入并为 `watch_core` 垂直闭环服务。

### 实施方案

1. 在现有 `sim/lv_port_pc_vscode` 中新增独立目标 `magic_watch_xml_sim`。
2. 复用现有 LVGL 和 SDL display/input/tick/fs 端口。
3. 链接 `ui/lvgl_pro` 生成 C。
4. 不编译旧 C++ 页面系统、旧 `AppStateMachine` 或旧 `PageManager`。

### 涉及位置

Allowed files:

- `sim/lv_port_pc_vscode/CMakeLists.txt`
- `sim/lv_port_pc_vscode/src/**`
- `ui/lvgl_pro/**`
- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `docs/40_workflow/agent_batch/cards/lvgl-xml-watch-core-q1.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/**`
- `watch_core/**`

Forbidden changes:

- 禁止修改旧 `magic_watch_sim` 行为。
- 禁止把旧 C++ 页面系统作为新目标依赖。
- 禁止接 F411 工程。

### 风险

- 现有 CMake 可能已经为旧 sim 聚合了大量源文件；新目标必须保持独立，不能为了省事复用旧 main source list。

### 验收标准

- 构建系统中存在独立 `magic_watch_xml_sim` 目标。
- 目标可显示 XML 生成 UI。
- 旧 `magic_watch_sim` 仍保留为参考，不被新目标替换。

### 执行记录

- 已完成：新增独立 `magic_watch_xml_sim` target，入口为 `src/xml_sim_main.cpp`；该目标复用现有 SDL simulator device、链接 `ui/lvgl_pro` 生成库，并直接加载 `screen_health_shortcuts_create()`。
- 已完成：`magic_watch_xml_sim` 使用 `MAGIC_WATCH_XML_UI_ASSET_PATH` 指向 `ui/lvgl_pro/` 图片资源目录，避免运行时依赖 LVGL Pro Editor。
- 自检：`cmake --build sim/lv_port_pc_vscode/build --config Debug` 通过，构建产出 `build/out/magic_watch_xml_sim.exe`；隐藏启动烟测 3 秒返回 `started-ok`，说明新目标可启动并保持运行；`git diff --check` 提交前执行。
- 风险回应：本轮未接旧 C++ 页面系统、旧 `AppStateMachine` 或旧 `PageManager`；旧 `magic_watch_sim` target 保留。隐藏启动烟测不等同于人工可见窗口验收，后续 Q1-5 仍需手动验证四卡显示和交互。

---

## LVGL-XML-Q1-4 实现 `watch_core` 最小快照、事件和 UI Adapter

- 批次：LVGL-XML-Q1
- 状态：TODO
- 依赖：`LVGL-XML-Q1-3`
- 自检：
  - `git status --short -uall`
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - `git diff --check`
  - 本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: add watch core ui event bridge`
- Doc Impact：required

### 问题定位

仅有 XML 页面和 PC 目标还不能证明 UI 与产品核心解耦。需要最小 `watch_core` 承担状态快照、点击事件和页面意图。

### 实施方案

1. 新增 `watch_core` 纯 C 模块。
2. 定义健康快捷页最小 `UiModelSnapshot`。
3. 定义固定大小 `UiEvent`，覆盖四张健康卡点击和返回。
4. 定义 Coordinator，根据卡片事件产生占位详情页页面意图。
5. 在 UI Adapter 中连接 LVGL 点击回调、Subject 更新和 screen create/load API。

### 涉及位置

Allowed files:

- `watch_core/**`
- `sim/lv_port_pc_vscode/CMakeLists.txt`
- `sim/lv_port_pc_vscode/src/**`
- `ui/lvgl_pro/**`
- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `docs/10_architecture/lvgl_xml_watch_core_architecture_uml.html`
- `docs/40_workflow/agent_batch/cards/lvgl-xml-watch-core-q1.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/**`

Forbidden changes:

- 禁止使用 C++ 继承、多态或动态容器实现核心事件边界。
- 禁止让 `watch_core` include LVGL、SDL、HAL 或 CubeMX 头文件。
- 禁止接真实传感器。

### 风险

- UI Adapter 容易偷懒变成业务状态中转站；必须让业务状态权威仍在 `watch_core`。

### 验收标准

- `watch_core` 可被 PC 目标编译。
- 模型快照可驱动健康四卡显示。
- 四张卡点击能进入 `UiEvent`。
- 页面切换由 Coordinator 页面意图触发，而不是 XML 直接决定。

### 执行记录

- 待执行。

---

## LVGL-XML-Q1-5 PC 可交互垂直闭环验收

- 批次：LVGL-XML-Q1
- 状态：TODO
- 依赖：`LVGL-XML-Q1-4`
- 自检：
  - `git status --short -uall`
  - `cmake --build sim/lv_port_pc_vscode/build --config Debug`
  - 手动运行 `magic_watch_xml_sim`
  - 四卡点击、占位详情页、返回路径人工验证
  - `git diff --check`
  - 本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`test: verify xml health card pc loop`
- Doc Impact：small

### 问题定位

第一阶段必须用一个可操作闭环证明路线成立，而不是停留在可编译或可预览。

### 实施方案

1. 在 PC 目标中提供假健康数据。
2. 记录或显示四卡点击产生的 `UiEvent`。
3. 进入同一个占位详情页，详情页展示来源功能名。
4. 支持返回健康快捷页。
5. 记录手动验收结果和未验证项目。

### 涉及位置

Allowed files:

- `watch_core/**`
- `sim/lv_port_pc_vscode/**`
- `ui/lvgl_pro/**`
- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `docs/30_testing/**`
- `docs/40_workflow/agent_batch/cards/lvgl-xml-watch-core-q1.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `try/my_watch_f411_v2.1/**`

Forbidden changes:

- 禁止宣称 F411 真机已通过。
- 禁止接真实传感器。
- 禁止迁 Zephyr。
- 禁止把 PC SDL 性能当作 LCD flush 性能结论。

### 风险

- 手动 UI 验收如果未执行，不能写成通过。

### 验收标准

- PC 窗口显示主页健康四卡。
- 点击心率、血氧、呼吸、心情四卡均可进入占位详情页。
- 占位详情页能区分来源功能。
- 返回后能回到健康快捷页。
- 日志或调试输出能追踪 `UiEvent -> Coordinator -> PageIntent -> UI Adapter`。

### 执行记录

- 待执行。

---

## F411-XML-Q2-1 接入同一套 XML 生成 UI 到 F411

- 批次：F411-XML-Q2
- 状态：TODO
- 依赖：`LVGL-XML-Q1-5`、`F411-LVGL-DMA-3`
- 自检：
  - `git status --short -uall`
  - Keil / MDK 工程编译通过，或如本机无法编译则明确记录未执行原因
  - `git diff --check`
  - 真机观察：同一套健康四卡 UI 可显示，颜色、字体和输入路径符合 F411-Q3 基线
  - 本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`feat: run xml generated ui on f411`
- Doc Impact：required

### 问题定位

PC 闭环只能证明生成 UI、事件链和模型快照可运行；最终仍需在 F411 真实 LCD、输入和刷新约束下验证同一套生成 C。

### 实施方案

1. 在 F411 工程中登记 `ui/lvgl_pro` 生成 C。
2. 接入 `watch_core` 的最小快照和事件边界。
3. 复用 F411-Q3 已验证的颜色、字体、flush 和 DMA 基线。
4. 真机显示健康四卡，并至少验证卡片点击或按键等价输入路径。

### 涉及位置

Allowed files:

- `try/my_watch_f411_v2.1/**`
- `ui/lvgl_pro/**`
- `watch_core/**`
- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `docs/40_workflow/agent_batch/cards/lvgl-xml-watch-core-q1.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `sim/lv_port_pc_vscode/**`

Forbidden changes:

- 禁止跳过 F411-Q3。
- 禁止在未验证颜色/字体/flush 基线时宣称真机迁移完成。
- 禁止把平台 driver 变成业务逻辑入口。

### 风险

- 生成 C 的资源、字体或图片登记可能影响 F411 Flash/RAM，需要以真机构建和显示结果为准。

### 验收标准

- F411 编译通过。
- 真机可显示同一套 XML 生成健康四卡 UI。
- 真机显示结果不破坏 F411-Q3 的颜色、字体和刷新基线。
- 文档记录 PC 与 F411 的差异和未验证项。

### 执行记录

- 待执行：Q2 需要复用 Q1 已验证的同一套 XML 生成产物；当前只完成 skill 沉淀和 Q1 健康四卡 preview 返工入口，尚未满足 `LVGL-XML-Q1-5` 与 `F411-LVGL-DMA-3` 依赖，不能越过 F411-Q3 直接宣称 Q2 返工完成。
