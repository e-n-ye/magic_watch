# LVGL XML + watch_core Architecture

日期：2026-06-11

## 当前主线

Magic Watch 当前主线从单纯的 F411 / C-first 蓝图，升级为：

```text
文档固化
-> ui/lvgl_pro 健康四卡 XML
-> magic_watch_xml_sim PC 目标
-> watch_core 模型快照 + UiEvent + UI Adapter
-> PC 可交互验收
-> V0.0 F411 DMA 收口
-> V0.1 刷新率诊断
-> V0.2 减少无效刷新
-> V0.3 F411 XML 兼容性探针
-> V0.4R F411 Lite UI 垂直闭环
```

这条路线的目标不是立刻做完整智能手表，而是先证明一条能长期扩展的 UI 与业务解耦路径：

- XML 负责 UI 资产、样式、布局、组件和 screen 组织。
- PC 主线编译 LVGL Pro 导出的 C 代码，不在 MCU 上运行时解析 XML。
- `watch_core` 负责产品状态、事件、页面意图和 UI 快照。
- PC 通过 SDL 作为显示/输入端口，先验证页面、事件和数据绑定。
- F411 作为真实 LCD、触摸/按键、flush、DMA、字体和性能约束场，但不再要求直接消费 PC 当前这份 LVGL 9.6 生成 C。

## 目录角色

后续工程按以下同层结构生长：

```text
magic_watch/
├─ watch_core/          # PC 与 F411 共用的纯 C 产品核心
├─ ui/
│  └─ lvgl_pro/         # XML 源文件、资源和已入库的生成 C
├─ sim/
│  └─ lv_port_pc_vscode # 现有 PC LVGL/SDL 端口与旧 sim 参考
└─ try/my_watch_f411_v2.1/
                         # 当前 F411 试点工程
```

`watch_core/` 不表示 LVGL 运行环境。它是产品大脑，负责“发生了什么、状态是什么、下一步该去哪里”。它不负责画 UI，也不直接访问屏幕、SDL、HAL 或传感器。

`ui/lvgl_pro/` 是 UI 作者输入和生成产物所在位置。XML 是人维护的 UI 蓝图，生成 C 是固件和 PC 构建真正使用的 UI 代码，两者都纳入 Git。

`sim/lv_port_pc_vscode` 中应新增独立 PC 验证目标 `magic_watch_xml_sim`。它可以复用现有 LVGL 和 SDL 端口，但不得继续扩建旧 C++ 页面系统。

F411 工程共享 `watch_core` 公共语义合同，但允许保留独立的 LVGL 8.2 Lite View、资源形式和 UI Adapter 装配方式。

## 固定边界

当前固定共享边界：

- `UiEvent`
- Coordinator
- `PageIntent`
- `UiModelSnapshot`
- 行为语义与验收用例

当前允许平台分叉：

- LVGL 主版本
- UI View 实现
- generated C
- 图片资源形式
- 页面视觉复杂度

### `ui/lvgl_pro`

职责：

- 维护 XML screens、components、widgets、styles、assets。
- 导出并提交生成 C，例如 `*_gen.c`、`*_gen.h`、`ui.c`、`file_list_gen.cmake`。
- 用 LVGL Pro Editor 预览 240x280 首目标页面。
- 通过 UI Adapter 暴露必要 LVGL 对象或事件挂接点。

不职责：

- 不访问传感器、HAL、SDL、F411 driver。
- 不持有业务状态源。
- 不把 LVGL Subject 当业务模型。
- 不在 XML 中直接决定产品级导航。

### `watch_core`

职责：

- 纯 C、固定容量、无堆分配的产品核心。
- 定义 `UiEvent`、模型状态、只读 UI 快照和页面意图。
- 接收来自 UI Adapter 或平台服务的 typed event。
- 由 Coordinator 决定页面切换、状态更新和下一步动作。

不职责：

- 不 include LVGL、SDL、HAL、CubeMX 或具体传感器头文件。
- 不创建 LVGL 对象。
- 不直接调用 screen create/load API。
- 不使用 `std::string`、`std::vector`、`std::function` 或 C++ 继承。

当前 Q1-4 最小落点：

- `watch_core/include/watch_core/watch_core.h` 定义健康四卡 feature、固定大小 `UiEvent`、`UiModelSnapshot` 和 `PageIntent`。
- `watch_core/src/watch_core.c` 持有默认健康快照、固定容量事件队列和 Coordinator 状态机。
- `watch_core` 只输出页面意图，不知道 LVGL screen、Subject、SDL 或 F411 平台。

### `magic_watch_xml_sim`

职责：

- PC 上运行 SDL + LVGL + XML 生成 C + `watch_core`。
- 验证模型注入后 UI 刷新。
- 验证四张健康卡点击后产生不同 `UiEvent`。
- 验证 Coordinator 决定进入同一个占位详情页，并支持返回。
- 为后续输入回放、截图回归和状态注入保留入口。

不职责：

- 不复用旧 C++ `AppStateMachine`、`PageManager` 和手写页面系统作为新主线。
- 不把 PC 结果当成真实 LCD 性能结论。
- 不替代 F411 真机颜色、字体、flush、DMA 和触摸验证。

当前 Q1-4 最小 adapter 落点：

- `sim/lv_port_pc_vscode/src/XmlUi/watch_core_ui_adapter.*` 持有 LVGL 对象绑定、health metric subjects、点击回调和 screen create/load；该 adapter 使用 C struct + C 函数，便于后续 F411 侧复用同一方向。
- `ui/lvgl_pro/magic_watch_ui.*` 只提供生成 UI 的稳定挂接 helper，不保存业务状态。
- `magic_watch_xml_sim` 初始化 `WatchCore`，再由 adapter 加载 XML 健康四卡页面。

### F411 平台

职责：

- 提供 display、input、tick、fs 等平台端口。
- 验证 LCD 方向、RGB565 字节序、字体观感、触摸/按键、flush 耗时和 DMA。
- 在 PC 闭环完成后，以手写 LVGL 8.2 Lite View + `F411UiAdapter` 落地共享语义合同。

不职责：

- 不承载业务核心的唯一实现。
- 不在平台 driver 中解释页面业务。
- 不复制业务状态机、导航逻辑或状态源。
- 不直接消费当前 LVGL 9.6 runtime XML 生成产物。
- 不在 `V0.3` 已止损后继续执行原定义 `V0.4 F411 XML 真机闭环`。

## 数据与事件链路

模型更新链路固定为：

```text
Hardware / Simulator
-> Service
-> watch_core ModelStore
-> UiModelSnapshot
-> UI Adapter
-> LVGL Subject / generated C
-> Screen
```

UI 输入链路固定为：

```text
XML / LVGL callback
-> UI Adapter
-> UiEvent
-> watch_core EventQueue
-> Coordinator
-> PageIntent
-> UI Adapter
-> generated screen create/load API
```

其中 LVGL Subject 只允许作为 UI Adapter 内部绑定机制。业务状态的权威源必须是 `watch_core` 的模型和快照。

## 首个 UI 闭环

第一条垂直闭环是主页环里的健康快捷页，不是完整心率详情应用。

页面内容：

- 心率卡
- 血氧卡
- 呼吸卡
- 心情卡

PC 验收：

- 四张 1/4 健康卡在 240x280 target 中可见。
- Editor preview 正常，无 invalid。
- `magic_watch_xml_sim` 可显示同一套生成 UI。
- 点击四张卡分别产生不同功能 ID 的 `UiEvent`。
- Coordinator 根据事件进入同一个占位详情页。
- 占位详情页显示来源功能名，并可返回主页健康快捷页。
- 事件链路可通过日志或调试输出追踪。

F411 验收在 PC 闭环之后进行。当前已完成 V0.0-V0.3，并确认原定义 `V0.4 F411 XML 真机最小闭环` 不再继续；后续改走 `V0.4R F411 Lite UI 垂直闭环`，先做 Lite View，再接 `F411UiAdapter`，最后做真机验收。

## 适配策略

首目标屏幕是 F411 当前 240x280。该尺寸优先精调，不假装未经适配的屏幕一定美观。

跨屏适配策略：

- 使用 LVGL Flex、百分比尺寸和集中式 layout token 控制安全区、间距、圆角、字体和触摸尺寸。
- 页面布局优先用相对关系，而不是到处硬编码绝对坐标。
- 未来新增屏幕尺寸时，先新增 display target，再调 token 和少量关键 screen。
- 不承诺一个 XML 在所有屏幕上零调试达到成品观感。

## 与旧 sim 的关系

旧 `magic_watch_sim` 保留为行为参考、交互经验和回归对照，不作为新主线继续扩建。

可保留的经验：

- 输入语义、服务、共享模型、事件、Coordinator、Screen Manager、UI Adapter 的分层思想。
- 主页环和健康入口的产品行为参考。
- PC 端快速运行和观察 UI 的工作流。

不照搬的内容：

- 旧 C++ 页面实现。
- 旧 `AppStateMachine` 巨石。
- `DataCenter` 无边界聚合。
- `std::function`、动态容器和 C++ 继承式页面多态。

## 当前不做

- 不迁 Zephyr。
- 不决定最终芯片。
- 不做双芯片/AON 域实现。
- 不接真实传感器、BLE、OTA、TinyML。
- 不把运行时 XML parser 作为 MCU 主路径。
- 不用 XML 直接承担产品级导航。
- 不在本轮创建工程或写运行时代码。

## 下一步入口

后续按 `docs/40_workflow/agent_batch/agent-queue.md` 执行：

1. `V0.0-F411-DMA-CLOSE`：只记录 DMA 前后指标和稳定性，不继续改 DMA。
2. `V0.1-F411-REFRESH-DIAG`：拆分刷新瓶颈，判断绘制、转换、传输、面积和调度问题。
3. `V0.2-F411-REFRESH-OPT`：减少无效刷新，隔离 debug probe 和真实 UI 负载。
4. `V0.3-F411-XML-PROBE`：评估 LVGL 8.2 / 9.6 API、资源和 Flash/RAM 风险。
5. `V0.4R`：执行 F411 Lite UI 垂直闭环，固定共享语义合同，不固定 UI 技术实现。
