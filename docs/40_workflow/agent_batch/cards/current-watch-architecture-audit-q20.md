# Current Watch Architecture Audit

## 卡片 ARCH-AUDIT-PC-01 PC XML 主链架构地图与必要性审查

- ID：`ARCH-AUDIT-PC-01`
- 标题：PC XML 主链架构地图与必要性审查
- 批次：真实工程架构掌控力恢复
- 状态：DONE
- 依赖：`ARCH-LAB-06` DONE

### 问题定位

架构冲突实验场已经证明了输入语义、系统事件和刷新节奏会自然产生分层压力，
但继续在独立 Console 工程里模拟队列和时间片，学习收益开始下降。

本卡回到当前产品主线，只回答：

> `magic_watch_xml_sim` 现在实际有哪些层，每层解决什么问题，哪些是必要边界，
> 哪些只是暂时借用、未接通或重复实现？

### 实施方案

- 只读追踪 `magic_watch_xml_sim` 的构建和启动入口
- 追踪健康卡点击到页面切换的完整调用链
- 追踪 Snapshot 到 LVGL label 的数据链
- 审查 PC HAL 在新 XML 主线中的真实作用
- 审查是否已经需要独立 ScreenManager、Service 或 EventBus
- 输出事实地图和必要性矩阵，不修改实现

### 涉及位置

Allowed files

- `docs/30_testing/current_watch_architecture_map_and_necessity_audit.md`
- `docs/document_map.md`
- `docs/40_workflow/agent_batch/cards/current-watch-architecture-audit-q20.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files

- `sim/lv_port_pc_vscode/**`
- `ui/lvgl_pro/**`
- `watch_core/**`
- `docs/00_current/**`
- `docs/10_architecture/**`

Forbidden changes

- 不修改 PC/F411/`watch_core` 代码
- 不清理旧 C++ 模拟器
- 不实现事件队列、时间片调度或 Power executor
- 不把架构蓝图描述成当前已接通事实
- 不在审计中直接提出大重构

### 风险

- 把旧 `magic_watch_sim` 与当前 `magic_watch_xml_sim` 混成一套架构
- 只依据目录名判断层次，不检查真实调用
- 因为发现未接通路径就立即重构
- 把“当前规模不需要独立模块”误写成“未来永远不需要”

### 自检

- `cmake --build sim/lv_port_pc_vscode/build --config Debug`
- `magic_watch_core_contract_test.exe`
- `git diff --check`
- 本轮中文 Markdown 乱码哨兵检查
- 核对审计中的每条“当前事实”都有代码入口支持

### 验收标准

- 能画出当前 PC XML 启动、输入、状态、页面和渲染链
- 能区分真实运行路径、未接通路径和历史参考路径
- 每层都回答“输入、输出、状态、解决的问题、删除后果”
- 明确判断当前是否需要独立 ScreenManager、Service、EventBus
- 不修改任何产品代码

### Doc Impact

`required`

### 建议提交信息

`docs: audit current pc xml architecture`

### 执行记录

- 2026-06-15：从架构冲突实验场转回真实工程，第一轮只审当前
  `magic_watch_xml_sim` 主链，不扩到旧 C++ 模拟器或 F411。
- 2026-06-15：已完成启动、输入、Snapshot、页面执行和 LVGL render 主链地图。
  代码证据确认：XML 主线未注册 HAL event callback；健康卡 generated component
  未挂 helper callback；实际点击由 Adapter 透明 hit target 产生。
- 2026-06-15：必要性结论为：Platform Runtime、XML View、Adapter 和
  `watch_core` 必要；独立 ScreenManager、Service、EventBus 和自定义 dirty
  renderer 当前没有实现压力。`cmake --build` 与 Core 合同测试通过。

### Stop policy

- 已完成并停止，先由用户审阅架构地图，再决定下一条真实链路。
