# Agent 任务计划

本文件是内联任务卡片清单，内联卡片状态以本文件为权威源。

执行规则见：`docs/40_workflow/agent_batch/agent-rules.md`。
执行队列见：`docs/40_workflow/agent_batch/agent-queue.md`。
运行时进度见：`docs/40_workflow/agent_batch/agent-progress.md`。
并行规划草稿见：`docs/40_workflow/agent_batch/agent-inbox.md`。

---

## S8-CLOSE-1 修正阶段 8 文档旧表述

- 批次：阶段 8 收尾 Batch 1
- 状态：DONE
- 依赖：无
- 自检：
  - `git diff --check`
  - `rg -n "ShellPages.h 聚合声明头和 Application.cpp 注册组合根|拆 ShellPages.h 聚合声明头|注册按领域分流|未来硬件接入不重写上层 |手动 UI 回归"` 仅检查本轮目标文档中的旧表述是否已收口
  - 对本轮实际改动中文文档执行乱码哨兵检查：`锛|鏃|褰|闈|€|�`
- 建议提交信息：`docs: close out stage 8 architecture wording`
- Doc Impact：small
- 问题定位：
  - `docs/00_current/current_architecture.md` 的“阶段 8 验收地图”仍保留一部分 8A 时点的“下一步”措辞，没有完全反映 8B-8E 已完成的事实。
  - `docs/10_architecture/ui_page_split_status.md` 仍有少量表述容易让新会话误读为 `ShellPages.h` 聚合头拆分或 `Application.cpp` 注册分流尚未落地。
- 实施方案：
  - 只修正文档中的旧时态/旧风险描述，不扩写成长复盘。
  - 明确 8B、8C、8D、8E 当前已完成到什么程度。
  - 保留“手动 UI 回归尚未执行”这一事实，不把构建通过写成行为通过。
- 涉及位置：
  - Allowed files:
    - `docs/00_current/current_architecture.md`
    - `docs/10_architecture/ui_page_split_status.md`
  - Read-only files:
    - `docs/00_current/current_refactor_status.md`
    - `docs/document_map.md`
    - `sim/lv_port_pc_vscode/src/App/Application.cpp`
    - `sim/lv_port_pc_vscode/src/App/UI/Pages/ShellPages.h`
  - Forbidden changes:
    - 禁止修改任何 `sim/**` 代码文件。
    - 禁止修改 `docs/document_map.md`。
    - 禁止新增文档。
    - 禁止把未执行的手动 UI 写成已通过。
- 风险：
  - 若把“阶段 8 已完成”写得过满，会遮蔽 8F 仍未执行以及 8C 小尾巴仍待审计的事实。
- 验收标准：
  - `current_architecture.md` 不再把 8B / 8C / 8D / 8E 写成未来计划。
  - `ui_page_split_status.md` 不再用旧表述暗示 `ShellPages.h` 聚合头拆分或注册分流尚未落地。
  - 两份文档都明确手动 UI 回归尚未执行。
  - `git diff --check` 通过。
- 执行记录：
  - 完成时间：2026-06-04
  - 实际改动文件：`docs/00_current/current_architecture.md`、`docs/10_architecture/ui_page_split_status.md`
  - 自检结果：`git diff --check` 通过；目标旧表述定向扫描已收口；本轮实际改动中文文档乱码哨兵检查通过
  - 风险回应：只修正文档旧时态与阶段判断，没有把 8F 未执行写成已通过，也没有扩大到无关状态文档

## S8-CLOSE-2 审计 8C 注册尾巴并记录接受偏离

- 批次：阶段 8 收尾 Batch 1
- 状态：DONE
- 依赖：S8-CLOSE-1
- 自检：
  - `git diff --check`
  - `rg -n "register_shell_home_pages|register_shell_notification_pages|register_shell_quick_settings_pages|register_shell_power_pages|register_daily_pages|register_health_pages|PageId::Launcher|SettingsHome" sim/lv_port_pc_vscode/src/App/Application.cpp`
  - `git status --short -uall`
- 建议提交信息：`docs: record accepted stage 8 registration deviations`
- Doc Impact：small
- 问题定位：
  - `Application.cpp` 已完成 Home、Notifications、QuickSettings、Power、Daily、Health 的注册分流，但 `LauncherPage` 仍直接注册，Settings 仍保留长列表。
  - 需要判断这两个尾巴是否接受为“本阶段有意保留的偏离”，避免后续窗口重复把它们当成遗漏。
- 实施方案：
  - 只读审计 `Application.cpp` 当前注册结构。
  - 若确认本阶段不继续处理，则仅在允许文档内写明“接受偏离”与原因。
  - 不新增注册函数，不修改代码。
- 涉及位置：
  - Allowed files:
    - `docs/00_current/current_architecture.md`
    - `docs/10_architecture/ui_page_split_status.md`
  - Read-only files:
    - `sim/lv_port_pc_vscode/src/App/Application.cpp`
    - `docs/00_current/current_refactor_status.md`
    - `docs/30_testing/manual_ui_regression_checklist.md`
  - Forbidden changes:
    - 禁止修改 `Application.cpp`。
    - 禁止修改 `ShellPages.h`、`SettingsPages.h` 或任何页面头/实现文件。
    - 禁止新增“顺手补完 Launcher/Settings 注册分流”的代码。
- 风险：
  - 如果“接受偏离”记录不够具体，后续窗口仍可能把它误判成未完成 bug。
- 验收标准：
  - 明确写出 `Launcher` 仍直接注册是否可接受。
  - 明确写出 Settings 长列表是否留到后续阶段。
  - 若接受偏离，文档里能解释为什么当前不继续拆。
  - 不产生任何代码 diff。
  - `git diff --check` 通过。
- 执行记录：
  - 完成时间：2026-06-04
  - 实际改动文件：`docs/00_current/current_architecture.md`、`docs/10_architecture/ui_page_split_status.md`
  - 自检结果：`git diff --check` 通过；`Application.cpp` 注册结构只读审计完成；`Launcher` 直接注册与 Settings 长列表已记录为阶段性接受偏离
  - 风险回应：没有顺手扩成代码收口轮，也没有把 Launcher / Settings 的尾巴伪装成“已经不存在”

## S8-REG-1 刷新阶段 8F 手动 UI 回归 checklist

- 批次：阶段 8 收尾 Batch 1
- 状态：DONE
- 依赖：S8-CLOSE-2
- 自检：
  - `git diff --check`
  - `rg -n "HomeRing|Notifications|QuickSettings|Power|Daily|Health|Settings|未执行|待执行" docs/30_testing/manual_ui_regression_checklist.md`
  - 对本轮实际改动中文文档执行乱码哨兵检查：`锛|鏃|褰|闈|€|�`
- 建议提交信息：`docs: refresh stage 8 manual ui regression checklist`
- Doc Impact：small
- 问题定位：
  - 阶段 8 代码/文档收口后，需要把手动 UI 回归清单与当前结构事实重新对齐。
  - 本轮目标是先准备 checklist，再由用户决定是否实际跑模拟器。
- 实施方案：
  - 刷新 `manual_ui_regression_checklist.md` 中与阶段 8 收尾相关的结构说明、执行顺序和高风险路径。
  - 明确哪些检查项只是准备好、尚未执行。
  - 不在本卡中直接宣称模拟器已跑通；若需要实际执行，交由后续单独执行卡决定。
- 涉及位置：
  - Allowed files:
    - `docs/30_testing/manual_ui_regression_checklist.md`
  - Read-only files:
    - `docs/00_current/current_architecture.md`
    - `docs/10_architecture/ui_page_split_status.md`
    - `docs/00_current/current_refactor_status.md`
    - `sim/lv_port_pc_vscode/src/App/Application.cpp`
  - Forbidden changes:
    - 禁止修改任何 `sim/**` 代码文件。
    - 禁止在未实际执行模拟器时写“已通过”。
    - 禁止顺手改 `docs/document_map.md` 或其它测试文档。
- 风险：
  - 如果把“准备执行”写成“已经执行”，会直接污染后续回归事实基线。
- 验收标准：
  - checklist 与当前阶段 8 结构事实一致。
  - 高风险路径至少覆盖 HomeRing、Notifications、QuickSettings、Power，以及 Daily / Health 的代表路径。
  - 文档明确区分“待执行”和“已执行”。
  - `git diff --check` 通过。
- 执行记录：
  - 完成时间：2026-06-04
  - 实际改动文件：`docs/30_testing/manual_ui_regression_checklist.md`
  - 自检结果：`git diff --check` 通过；checklist 关键词扫描通过；本轮实际改动中文文档乱码哨兵检查通过
  - 风险回应：只刷新阶段 8F 的回归清单和结构说明，没有提前执行模拟器，也没有把“准备执行”写成“已通过”
