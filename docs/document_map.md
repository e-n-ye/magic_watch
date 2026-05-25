# Magic Watch 文档导航图

日期：2026-05-24

这份文档只回答一件事：新的会话进入项目时，哪些文档是当前主线必读，哪些只是历史参考。

---

## 一、默认阅读顺序

如果是一个全新的会话，推荐按这个顺序进入项目：

1. `AGENTS.md`
2. [document_map.md](/D:/MY_Desk/watch/magic_watch/docs/document_map.md)
3. [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/project_charter.md)
4. [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/v0_scope.md)
5. [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)
6. 再按当前任务类型选择下面对应的专题文档

如果时间很紧，最少先读：

- [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/project_charter.md)
- [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/v0_scope.md)
- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)

---

## 二、当前主线必读

### 1. 项目定位与方法

- [project_charter.md](/D:/MY_Desk/watch/magic_watch/docs/project_charter.md)
- [architecture_principles.md](/D:/MY_Desk/watch/magic_watch/docs/architecture_principles.md)
- [workflow.md](/D:/MY_Desk/watch/magic_watch/docs/workflow.md)
- [learning_plan.md](/D:/MY_Desk/watch/magic_watch/docs/learning_plan.md)

### 2. 当前 v0 主线范围

- [v0_scope.md](/D:/MY_Desk/watch/magic_watch/docs/v0_scope.md)
- [target_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/target_architecture.md)
- [current_sim_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/current_sim_architecture.md)
- [decision_log.md](/D:/MY_Desk/watch/magic_watch/docs/decision_log.md)

### 3. 当前页面与交互主线

- [home_interaction_model.md](/D:/MY_Desk/watch/magic_watch/docs/home_interaction_model.md)
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)
- [watchface_style_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/watchface_style_architecture.md)

### 4. 当前回归与验证入口

- [simulator_manual_regression_matrix.md](/D:/MY_Desk/watch/magic_watch/docs/simulator_manual_regression_matrix.md)
- [simulator_reading_guide.md](/D:/MY_Desk/watch/magic_watch/docs/simulator_reading_guide.md)

---

## 三、按任务类型选读

### 1. 如果当前任务是“系统骨架 / service / 事件边界”

优先读：

- [v0_4_system_skeleton_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_system_skeleton_entry.md)
- [v0_4_simulator_system_boundary_audit.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_simulator_system_boundary_audit.md)
- [v0_4_platform_hal_event_contract.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_platform_hal_event_contract.md)
- [v0_4_battery_power_service_boundary.md](/D:/MY_Desk/watch/magic_watch/docs/v0_4_battery_power_service_boundary.md)
- [v0_6_steps_activity_service_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_6_steps_activity_service_entry.md)
- [v0_7_notification_service_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_7_notification_service_entry.md)
- [v0_7b_notification_preview_shell_linkage.md](/D:/MY_Desk/watch/magic_watch/docs/v0_7b_notification_preview_shell_linkage.md)
- [v0_7c1_notification_read_state_and_detail_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_7c1_notification_read_state_and_detail_entry.md)
- [v0_7c2a_notification_clear_semantics.md](/D:/MY_Desk/watch/magic_watch/docs/v0_7c2a_notification_clear_semantics.md)
- [v0_7c2b1_notification_swipe_delete.md](/D:/MY_Desk/watch/magic_watch/docs/v0_7c2b1_notification_swipe_delete.md)
- [v0_7d_notification_stage_closure.md](/D:/MY_Desk/watch/magic_watch/docs/v0_7d_notification_stage_closure.md)
- [v0_8a_health_monitoring_settings_model_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8a_health_monitoring_settings_model_entry.md)
- [v0_8b_health_monitoring_settings_model_impl.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8b_health_monitoring_settings_model_impl.md)
- [v0_8c1_sleep_home_settings_shell.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8c1_sleep_home_settings_shell.md)
- [v0_8c2_sleep_info_body.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8c2_sleep_info_body.md)
- [v0_8d1_blood_oxygen_home_settings_shell.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8d1_blood_oxygen_home_settings_shell.md)

### 2. 如果当前任务是“电池 / 长续航路径”

优先读：

- [v0_5_power_status_page_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_5_power_status_page_entry.md)
- [v0_5_long_battery_regression_closure.md](/D:/MY_Desk/watch/magic_watch/docs/v0_5_long_battery_regression_closure.md)
- [simulator_manual_regression_matrix.md](/D:/MY_Desk/watch/magic_watch/docs/simulator_manual_regression_matrix.md)

### 3. 如果当前任务是“续航优化 / 健康监测设置共享模型”

优先读：

- [v0_8a_health_monitoring_settings_model_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8a_health_monitoring_settings_model_entry.md)
- [v0_8b_health_monitoring_settings_model_impl.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8b_health_monitoring_settings_model_impl.md)
- [v0_8c1_sleep_home_settings_shell.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8c1_sleep_home_settings_shell.md)
- [v0_8c2_sleep_info_body.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8c2_sleep_info_body.md)
- [v0_8d1_blood_oxygen_home_settings_shell.md](/D:/MY_Desk/watch/magic_watch/docs/v0_8d1_blood_oxygen_home_settings_shell.md)
- [v0_5_power_status_page_entry.md](/D:/MY_Desk/watch/magic_watch/docs/v0_5_power_status_page_entry.md)
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)

### 4. 如果当前任务是“主页环 / 页面导航 / 页面归属”

优先读：

- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch/docs/page_reachability_audit.md)
- [home_interaction_model.md](/D:/MY_Desk/watch/magic_watch/docs/home_interaction_model.md)
- [watchface_style_architecture.md](/D:/MY_Desk/watch/magic_watch/docs/watchface_style_architecture.md)

### 5. 如果当前任务是“硬件参考 / 过去板级验证”

优先读：

- [hardware_frontier_entry.md](/D:/MY_Desk/watch/magic_watch/docs/hardware_frontier_entry.md)
- [twatch_s3_plus_reference_validation_plan.md](/D:/MY_Desk/watch/magic_watch/docs/twatch_s3_plus_reference_validation_plan.md)
- [twatch_s3_plus_reference_validation_run.md](/D:/MY_Desk/watch/magic_watch/docs/twatch_s3_plus_reference_validation_run.md)

注意：

- 这些文档属于“参考验证与历史判断”
- 不是当前主线的默认起点

---

## 四、历史参考文档

这些文档仍然有价值，但描述的是较早阶段的判断、阶段入口或中间状态。阅读时默认带着“历史语境”去看，而不是直接当成当前代码现状。

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

- 这两份文档仍能提供视觉和结构判断
- 但其中提到的 `HomeShortcutPage / WeatherShortcutPage` 已经不是当前代码主线

---

## 五、视觉与素材工作流

这些文档偏向视觉探索、图标生成和沙盒流程，不是默认必读，但在做 UI 或素材工作时很有用。

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
