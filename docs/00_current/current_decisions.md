# Magic Watch Current Decisions

日期：2026-06-11

本文件只保留当前仍有效、会影响下一轮开发的决策。历史原因查 `docs/decision_log.md`；它继续保留 docs 根目录作为稳定历史检索路径，但不再作为默认阅读文档。

## D001: 当前主线升级为 `LVGL XML + watch_core + PC/F411 双后端`

当前优先目标不是继续新增旧模拟器页面，而是用 LVGL XML 生成 UI、用纯 C `watch_core` 管理产品状态和事件，再分别通过 PC SDL 与 F411 平台端口验证。

## D002: XML 是 PC UI 主线的 UI 源，不再要求 F411 消费同一份生成 C

`ui/lvgl_pro` 中的 XML 是人维护的 UI 蓝图。PC 主线继续编译 LVGL Pro 导出的 C 文件；XML 与生成 C 都必须入库，避免构建依赖每台机器现场运行同版本 Editor。F411 不再要求直接消费 PC 当前这份 LVGL 9.6 生成 C，而是固定共享语义合同、允许 UI View 技术实现分叉。

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

## D012: 平台必须共享 core 语义合同，但 UI View、LVGL 主版本和资源实现允许分叉

PC SDL 与 F411 LCD/Touch 的平台差异仍应尽量收敛在 display/input/tick/fs 等 port 层；但当前已确认 UI View、LVGL 主版本、generated C 和图片资源形式允许按目标平台分叉。必须固定共享的是 `UiEvent`、Coordinator、`PageIntent`、`UiModelSnapshot` 和行为语义，而不是强行共享同一份 UI 技术产物。

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

## D018: `V0.3` 已触发停止条件，原定义 `V0.4` 被 `V0.4R` 替代

旧 `F411-XML-Q2-1` 直接上板路线已停止。`V0.3-D` 已确认命中 `runtime XML / PNG-FS / 大改生成代码` 停止条件，因此原定义 `V0.4 F411 XML 真机最小闭环` 不再直接执行，替代路线改为 `V0.4R F411 Lite UI 垂直闭环`。

## D019: `V0.4R` 的固定边界是“共享语义合同，不固定 UI 技术实现”

`V0.4R` 保留 PC `LVGL 9.6 + XML` 主线，保留 F411 `LVGL 8.2 + DMA flush + 20 行 draw buffer + 当前显示基线`。F411 侧采用手写 LVGL 8.2 Lite View + `F411UiAdapter`，Lite View 只负责显示，不复制业务状态机、导航逻辑或状态源。暂缓的路线包括：F411 升级 LVGL 9.6、XML -> Lite IR/Generator、W25Q128 + LVGL FS + PNG 主链。
