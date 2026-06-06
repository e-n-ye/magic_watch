# Magic Watch Current Project Brief

日期：2026-06-06

## 当前定位

Magic Watch 当前不是以继续堆智能手表页面为目标，也不是立刻绑定最终芯片、RTOS 或完整商业产品路线。

当前主线是：

```text
LVGL XML UI 源
-> 生成 C 入库
-> watch_core 纯 C 产品核心
-> PC SDL 可交互验证
-> F411 真实显示链路验证
```

v0 的成功标准不是页面数量，而是：

1. UI 资产、产品状态、平台硬件边界清楚。
2. 新增一个功能时，修改范围可预测。
3. XML 不访问硬件和业务服务。
4. `watch_core` 不依赖 LVGL、SDL、HAL 或具体传感器。
5. PC 能快速验证页面、数据绑定和点击事件链路。
6. F411 能验证真实颜色、字体、flush、DMA 和显示性能。
7. 未来换主控、传感器或屏幕时，UI 资产和产品核心尽量不重写。
8. 文档入口足够短，历史资料可检索但不污染默认上下文。

## 当前阶段

阶段 8 架构收口和阶段 9 真实 MCU 经验，已经证明 Magic Watch 需要更硬的边界：UI、事件、状态、平台端口不能继续混在旧 C++ sim 里生长。

当前阶段进入 `LVGL XML + watch_core + PC/F411 双后端` 主线。

本阶段优先证明一件事：

即使不搬当前 C++ 模拟器页面代码，也能用 LVGL XML 生成 UI，并通过纯 C `watch_core` 在 PC 与 F411 两端复用同一套产品状态和事件边界。

当前蓝图入口：

- `docs/10_architecture/lvgl_xml_watch_core_architecture.md`
- `docs/10_architecture/lvgl_xml_watch_core_architecture_uml.html`

F411 C-first 蓝图仍保留为真机落地参考：

- `docs/10_architecture/f411_future_watch_architecture.md`
- `docs/10_architecture/f411_future_watch_architecture_uml.html`

## 当前路线

后续按以下顺序推进：

1. 文档固化：架构、UML、卡片队列。
2. 建 `ui/lvgl_pro`：主页健康四卡 XML，可预览。
3. 建 PC 新目标：`magic_watch_xml_sim`。
4. 接 `watch_core`：模型快照、`UiEvent`、UI Adapter。
5. PC 验收：四卡可点，进入占位详情页，可返回，事件链可追踪。
6. F411-Q3：颜色、字体、flush、DMA 性能基线。
7. F411 接 XML UI：同一套生成 C 上真机。

## 当前不做

- 不推倒重写旧模拟器。
- 不继续扩建旧 C++ 页面系统作为新主线。
- 不继续盲目填充页面。
- 不把 XML runtime parser 作为 MCU 主路径。
- 不让 XML 直接承担产品级导航。
- 不让 LVGL Subject 成为业务状态源。
- 不把 F411 当最终硬件选型结论。
- 不迁 Zephyr，不绑定最终 RTOS。
- 不接真实传感器、BLE、OTA、TinyML 或双芯片/AON 域。
- 不跳过 PC 可交互闭环直接做 F411 XML 真机迁移。

## 当前优先级

1. 维护 `LVGL XML + watch_core` 架构入口。
2. 把 UI 资产放入 `ui/lvgl_pro`，XML 与生成 C 都入库。
3. 把产品核心放入 `watch_core`，保持纯 C、固定容量、无堆分配。
4. 新增 `magic_watch_xml_sim`，复用 PC SDL/LVGL 端口，但不拉旧 C++ 业务页。
5. 用主页健康四卡完成第一条垂直闭环。
6. PC 闭环后继续 F411-Q3，再迁移同一套 XML 生成 UI 到 F411。

## 本阶段完成标准

本阶段完成不以新增页面数量、文件变短或 AI 生成代码量为标准，而以“人能否掌控架构”为标准：

1. 新会话只读 `AGENTS.md`、`docs/document_map.md` 和当前入口文档即可理解主线。
2. 能说清 `ui/lvgl_pro`、`watch_core`、`magic_watch_xml_sim`、F411 各自负责什么、不负责什么。
3. 能用 `UiModelSnapshot` 和 `UiEvent` 解释模型更新与点击跳页。
4. 能判断一个新功能应该进入 XML、UI Adapter、`watch_core`、PC port、F411 port 还是真实服务。
5. 每轮开发都有明确 Scope Lock，实际 diff 不越过本轮允许文件。
6. 后续第一批 XML、PC、`watch_core`、F411 模块按卡片小步生长，而不是一次性生成完整工程。
