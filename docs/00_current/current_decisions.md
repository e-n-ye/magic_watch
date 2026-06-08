# Magic Watch Current Decisions

日期：2026-06-07

本文件只保留当前仍有效、会影响下一轮开发的决策。历史原因查 `docs/decision_log.md`；它继续保留 docs 根目录作为稳定历史检索路径，但不再作为默认阅读文档。

## D001: 当前主线升级为 `LVGL XML + watch_core + PC/F411 双后端`

当前优先目标不是继续新增旧模拟器页面，而是用 LVGL XML 生成 UI、用纯 C `watch_core` 管理产品状态和事件，再分别通过 PC SDL 与 F411 平台端口验证。

## D002: XML 只作为 UI 源文件，固件只吃导出的 C

`ui/lvgl_pro` 中的 XML 是人维护的 UI 蓝图。进入 PC 和 F411 构建的是 LVGL Pro 导出的 C 文件。XML 与生成 C 都必须入库，避免构建依赖每台机器现场运行同版本 Editor。

## D003: `watch_core` 是纯 C 产品核心

`watch_core` 负责模型、事件、状态快照、页面意图和 Coordinator。它必须保持纯 C、固定容量、无堆分配，不依赖 LVGL、SDL、HAL、CubeMX 或具体传感器。

## D004: LVGL Subject 只允许作为 UI Adapter 内部绑定机制

业务状态权威源是 `watch_core` 的模型和 `UiModelSnapshot`。LVGL Subject 可以用于 UI Adapter 内部刷新生成 UI，但不得成为业务状态源，也不得跨过 `watch_core` 直接控制硬件或服务。

## D005: UI 输入必须转成 typed `UiEvent`

XML/LVGL 点击、返回、卡片选择等输入由 UI Adapter 转成固定大小的 `UiEvent`，进入 `watch_core` EventQueue。XML 不直接承担产品级导航。

## D006: 页面切换由 Coordinator 决定

Coordinator 根据 `UiEvent` 和模型状态产生页面意图，再由 UI Adapter 调用生成 screen 的 create/load API。这样 PC 占位页、未来真实详情页和 F411 页面迁移共享同一条事件链。

## D007: `magic_watch_xml_sim` 是新主线 PC 验证目标

PC 端需要一个独立目标运行 SDL + LVGL + XML 生成 C + `watch_core`。它可以复用现有 LVGL/SDL 端口，但不得继续扩建旧 C++ `AppStateMachine`、`PageManager` 和手写页面系统。

## D008: 旧 `magic_watch_sim` 只保留为行为参考

旧模拟器保留交互经验、页面行为和回归参考价值，但不是新主线产品核心。后续不得因为方便而把旧 C++ 页面或巨石状态机搬入 `watch_core`。

## D009: 第一条垂直闭环是主页健康四卡

首个 XML 页面是主页环中的健康快捷区，包含心率、血氧、呼吸、心情四张 1/4 卡。它不是完整心率详情应用。PC 验收是四卡可点击、进入占位详情页、可返回、事件链可追踪。

## D010: 屏幕首目标是 F411 当前 240x280

首轮 UI 优先精调 240x280。未来适配其他屏幕时，通过 Flex、百分比尺寸和集中式 layout token 降低改动，但不承诺未经适配的屏幕自动达到成品观感。

## D011: F411-Q3 排在 PC 垂直闭环之后

F411-Q3 的颜色、字体、flush、DMA 性能基线仍是主线的一部分，但当前只剩 DMA 前后指标和稳定性记录需要收口。DMA 已接通后不再作为主要优化方向，后续优先进入 V0.1 刷新率诊断和 V0.2 无效刷新优化。

## D012: 平台分叉只发生在 display/input/tick/fs 等 port 层

PC SDL 与 F411 LCD/Touch 的差异应收敛在平台端口。`watch_core`、模型快照、`UiEvent`、页面意图和 XML 生成 C 不应感知具体平台。

## D013: Action/Event 必须适合嵌入式环境

事件、快照、页面意图应固定大小、可按值传递、无动态资源。不在 Action/Event 中使用 `std::string`、`std::vector`、heap allocation 或复杂对象所有权。

## D014: 子 Controller 不直接操作 UI

后续如果 `watch_core` 内拆出子控制器，它们不得直接调用 LVGL、不得创建 UI 对象、不得访问页面内部。它们只能返回状态更新或页面意图。

## D015: 文档入口必须短

默认新会话只读 `AGENTS.md`、`docs/document_map.md` 和 `docs/00_current/` 下的当前入口文档。长架构、历史审计和旧决策只按任务补读。

## D016: 每轮必须 Scope Lock

每一轮实施前必须明确 `Allowed files`、`Forbidden files` 和 `Forbidden changes`。结束后检查 `git status`，实际修改文件必须落在允许范围内。

## D017: 新增 UI 页面必须先确定归属

新增 screen、component、surface、overlay 或详情页前，必须判断它属于 XML UI 资产、UI Adapter、`watch_core`、PC port、F411 port 还是旧 sim 参考，不得因为注册方便而塞进旧页面系统。

## D018: F411 XML 上板前必须先完成 V0.0-V0.3

旧 `F411-XML-Q2-1` 直接上板路线暂停。新的执行顺序是 `V0.0` DMA 收口、`V0.1` 刷新率诊断、`V0.2` 减少无效刷新、`V0.3` F411 XML 兼容性探针；只有 `V0.3-D` 明确可继续后，才规划新的 `V0.4` 真机最小闭环卡片。

## D019: F411 XML 兼容性探针允许止损

如果探针显示需要 runtime XML、PNG/FS 主路径、升级 LVGL 或大面积手改生成代码，应停止并重评 UI 生成路线。停止不是失败，而是避免把资源和版本问题拖进实现阶段。
