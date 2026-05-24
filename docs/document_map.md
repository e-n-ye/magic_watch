# Magic Watch 文档导航图

日期：2026-05-24

这份文档的目标很简单：

- 告诉后续会话“先读什么”
- 区分“当前主线文档”和“历史参考文档”
- 减少因为旧路线、旧页面或旧实验记录而产生的阅读噪音

---

## 一、默认阅读顺序

如果是一个全新的会话，推荐按这个顺序进入项目：

1. `AGENTS.md`
2. [document_map.md](/D:/MY_Desk/watch/magic_watch/docs/document_map.md)
3. [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/project_charter.md)
4. [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/v0_scope.md)
5. [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)
6. 再按当前任务类型选择下面对应的专题文档

如果时间非常紧，只能读最少集合，优先读：

- [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/project_charter.md)
- [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/v0_scope.md)
- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)

---

## 二、当前主线必读

这些文档描述的是当前仍然有效的项目目标、边界和代码主线。

### 1. 项目定位与方法

- [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/project_charter.md)
  - 项目目标、边界、成功标准
- [architecture_principles.md](/D:/MY_Desk/watch/magic_watch/docs/architecture_principles.md)
  - 架构判断原则
- [workflow.md](/D:/MY_Desk/watch/magic_watch/docs/workflow.md)
  - 会话推进方式
- [learning_plan.md](/D:/MY_Desk/watch/magic_watch/docs/learning_plan.md)
  - 学习与成长路径

### 2. 当前 v0 主线范围

- [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/v0_scope.md)
  - 当前 v0 的闭环范围
- [target_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/target_architecture.md)
  - 目标骨架
- [current_sim_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/current_sim_architecture.md)
  - 当前模拟器架构现状
- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)
  - 所有阶段决策与收口记录

### 3. 当前页面与交互主线

- [home_interaction_model.md](/D:/MY_Desk/watch/magic_watch/docs/home_interaction_model.md)
  - 表盘中心交互模型
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)
  - 当前可达页面 / 已清理遗留页面 / 页面命名约定
- [watchface_style_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/watchface_style_architecture.md)
  - 表盘宿主与风格边界

### 4. 当前回归与验证入口

- [simulator_manual_regression_matrix.md](/D:/MY_Desk/watch/magic_watch/docs/simulator_manual_regression_matrix.md)
  - 当前最重要的模拟器手工回归矩阵
- [simulator_reading_guide.md](/D:/MY_Desk/watch/magic_watch/docs/simulator_reading_guide.md)
  - 模拟器代码阅读入口

---

## 三、按任务类型选读

### 1. 如果当前任务是“系统骨架 / service / 事件边界”

优先读：

- [v0_4_system_skeleton_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_system_skeleton_entry.md)
- [v0_4_simulator_system_boundary_audit.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_simulator_system_boundary_audit.md)
- [v0_4_platform_hal_event_contract.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_platform_hal_event_contract.md)
- [v0_4_battery_power_service_boundary.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_battery_power_service_boundary.md)
- [v0_6_steps_activity_service_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_6_steps_activity_service_entry.md)

### 2. 如果当前任务是“电池 / 长续航路径”

优先读：

- [v0_5_power_status_page_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_5_power_status_page_entry.md)
- [v0_5_long_battery_regression_closure.md](/D:/MY_Desk/watch/magic_watch/docs/v0_5_long_battery_regression_closure.md)
- [simulator_manual_regression_matrix.md](/D:/MY_Desk/watch/magic_watch/docs/simulator_manual_regression_matrix.md)

### 3. 如果当前任务是“主页环 / 页面导航 / 页面归属”

优先读：

- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)
- [home_interaction_model.md](/D:/MY_Desk/watch/magic_watch/docs/home_interaction_model.md)
- [watchface_style_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/watchface_style_architecture.md)

### 4. 如果当前任务是“硬件参考 / 过去板级验证”

优先读：

- [hardware_frontier_entry.md](/D:/MY_Desk/watch/magic_watch/docs/hardware_frontier_entry.md)
- [twatch_s3_plus_reference_validation_plan.md](/D:/MY_Desk/watch/magic_watch/docs/twatch_s3_plus_reference_validation_plan.md)
- [twatch_s3_plus_reference_validation_run.md](/D:/MY_Desk/watch/magic_watch/docs/twatch_s3_plus_reference_validation_run.md)

注意：

- 这些文档属于“参考验证与历史判断”
- 不是当前主线的默认起点

---

## 四、历史参考文档

这些文档仍然有价值，但它们描述的是较早阶段的判断、阶段入口或中间状态。
阅读时应默认带着“历史语境”去看，而不是直接当成当前代码现状。

### 1. 阶段性收口 / 阶段入口

- [v0_1_architecture_closure.md](/D:/MY_Desk/watch/magic_watch/docs/v0_1_architecture_closure.md)
- [v0_2_shell_closure.md](/D:/MY_Desk/watch/magic_watch/docs/v0_2_shell_closure.md)
- [v0_4_stage_closure.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_stage_closure.md)
- [v0_shell_baseline.md](/D:/MY_Desk/watch/magic_watch/docs/v0_shell_baseline.md)
- [prototype_progress.md](/D:/MY_Desk/watch/magic_watch/docs/prototype_progress.md)

### 2. 已经过历史收口的页面路线

- [weather_shortcut_implementation_spec.md](/D:/MY_Desk/watch/magic_watch/docs/weather_shortcut_implementation_spec.md)
- [weather_shortcut_visual_direction.md](/D:/MY_Desk/watch/magic_watch/docs/weather_shortcut_visual_direction.md)

说明：

- 这两份文档依然能提供视觉和结构判断
- 但其中提到的 `HomeShortcutPage / WeatherShortcutPage` 已经不是当前代码主线

---

## 五、视觉与素材工作流

这些文档偏向视觉探索、图标生成和沙盒流程，不是默认必读，但在做 UI 或素材工作时非常有用。

- [visual_asset_workflow.md](/D:/MY_Desk/watch/magic_watch/docs/visual_asset_workflow.md)
- [weather_icon_workflow.md](/D:/MY_Desk/watch/magic_watch/docs/weather_icon_workflow.md)
- [web_visual_sandbox_workflow.md](/D:/MY_Desk/watch/magic_watch/docs/web_visual_sandbox_workflow.md)
- [settings_visual_reference.md](/D:/MY_Desk/watch/magic_watch/docs/settings_visual_reference.md)

---

## 六、当前不建议误判为“遗留垃圾”的内容

下面这些虽然仍有 mock / placeholder 成分，但它们仍属于当前主路径资产，不应简单当成遗留页删除：

- `AppHeartRate`
- `AppBloodOxygen`
- `AppBreathing`
- `AppStress`
- `AppSleep`
- `AppNfc`
- `AppAlipay`
- `AppWeChatPay`
- `SettingSound`
- `SettingDoNotDisturb`
- `SettingNotifications`
- `SettingSystemActions`
- `SettingAbout`

它们的性质是：

- 当前可达
- 当前仍是壳层或占位
- 未来用于逐步细化实现

---

## 七、当前最重要的阅读约定

从现在开始，默认按下面的约定理解仓库：

- “当前代码主线”优先看：
  - `Application.cpp`
  - `ShellPages.cpp`
  - `SettingsPages.cpp`
  - `AppStateMachine.cpp`
  - `page_reachability_audit.md`

- “文档里写到旧路线”时，先判断它是：
  - 当前有效结构
  - 还是历史探索记录

- 如果一个文档提到的页面类已经不在当前主线路径中，应以：
  - [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)
  - [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)
  为最终解释

---

## 八、后续维护规则

以后如果再新增一类文档，建议同时回答两个问题：

1. 它属于“当前必读”还是“历史参考”
2. 新会话应该在什么任务场景下读到它

如果一份文档已经不再适合作为当前主线入口，不一定要删，但应至少补一段“历史说明”。
