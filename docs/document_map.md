# Magic Watch 文档任务路由器

日期：2026-06-02

本文件不再是“推荐阅读清单”，而是任务路由器。默认上下文必须短；历史文档可以检索，但不能污染每次新会话。

## 默认新会话

默认只读：

1. `AGENTS.md`
2. [document_map.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/document_map.md)
3. [project_brief.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/project_brief.md)
4. [current_decisions.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_decisions.md)

如果时间很紧，最少读：

- [document_map.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/document_map.md)
- [project_brief.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/project_brief.md)
- [current_decisions.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_decisions.md)

默认入口只回答三件事：

- 项目当前为什么继续做
- 当前哪些决策仍有效
- 遇到具体任务时下一步该补读什么

默认新会话不要一开始通读长事实文档、长拆分状态文档或长回归清单。

## 当前默认不读

以下文档不再作为默认入口：

- [project_charter.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/project_charter.md): 旧阶段项目宪章，已由 `00_current/project_brief.md` 接管当前定位。
- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_refactor_status.md): 当前代码事实基线，内容较长，只在需要确认“代码实际拆到哪”时补读。
- [current_architecture.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_architecture.md): 架构主线入口，不是每次新会话都必须先读。
- [v0_scope.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/v0_scope.md): 当前范围摘要，在需要确认阶段边界时补读。
- [decision_log.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/decision_log.md): 历史决策流水账，只作历史检索。
- [page_reachability_audit.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/page_reachability_audit.md): 历史页面可达性审计，只在查旧入口、旧页面或历史判断时选读。

## 当前代码事实补读

- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_refactor_status.md): 当前代码重构落点总览。

读取时机：

- 需要确认各页面域、helper、聚合头和构建登记的真实现状
- 需要确认某个页面是否已迁出 `ShellPages.cpp`
- 需要确认当前文档是否反映已提交代码事实

定位说明：

- 它是当前代码事实基线，不是默认必读。
- 它记录已提交事实，不记录未提交中间态。
- 读完它后，再按任务进入页面拆分、测试回归或架构专题文档。

## 按任务选读

### 我想知道当前项目是什么

先读：

- [project_brief.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/project_brief.md)
- [current_decisions.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_decisions.md)
- [v0_scope.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/v0_scope.md)，仅在需要确认当前阶段边界时补读。

### 我想知道当前重构到哪了

先读：

- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_refactor_status.md)
- [ui_page_split_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/10_architecture/ui_page_split_status.md)

### 我想继续拆 `ShellPages.cpp`

- [current_architecture.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_architecture.md)
- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_refactor_status.md)
- [ui_page_split_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/10_architecture/ui_page_split_status.md)

### 我想新增 UI 页面

先读：

- [ui_page_ownership.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/10_architecture/ui_page_ownership.md)
- [current_architecture.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_architecture.md)
- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_refactor_status.md)，仅在需要确认当前实现文件归属时补读。

### 我想判断页面归属

先读：

- [ui_page_ownership.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/10_architecture/ui_page_ownership.md)
- [ui_page_split_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/10_architecture/ui_page_split_status.md)

### 我想继续 HeartRate 测量流

先读：

- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_refactor_status.md)
- [ui_page_split_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/10_architecture/ui_page_split_status.md)

说明：

- HeartRate 后续风险重点已经不是继续从 `ShellPages.cpp` 迁页面。
- 当前更需要关注 `HeartRateAppPage` 内部 measurement timer、stage 切换和 wear prompt 生命周期边界。

### 我想做手动 UI 回归

先读：

- [manual_ui_regression_checklist.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/30_testing/manual_ui_regression_checklist.md)
- [current_refactor_status.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_refactor_status.md)，仅在需要把回归包映射到当前结构风险时补读。

说明：

- 构建通过不等于手动 UI 通过。
- 未执行的手动 UI 不能写成通过。

### 我想写简历项目描述

先读：

- [project_story_notes.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/90_resume/project_story_notes.md)

说明：

- 这里只整理项目叙事素材，不影响工程决策。
- 它不是默认工程必读。
- 后续如需继续整理简历，可再结合 `D:/MY_Desk/watch/jianli`，但这不属于当前工程默认上下文。

### 我想理解 `DataCenter / EventBus / AppStateMachine / PageManager`

先读：

- [current_architecture.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_architecture.md)
- [current_decisions.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/00_current/current_decisions.md)

### 我想查历史决策

先读：

- [decision_log.md](/D:/MY_Desk/watch/magic_watch_docs_sync/docs/decision_log.md)

说明：

- 它是历史检索材料，不应回到默认入口。
- 读取时只检索相关日期或关键词，不默认通读全文。

## 文档维护规则

- 新增文档必须说明它属于当前入口、架构专题、开发指南、审计资料还是历史检索。
- 当前入口文档应短、稳定、面向新会话。
- 历史文档保留价值，但不得默认进入上下文。
- 中文文档修改后，对本轮实际改动的中文文件做乱码哨兵检查。
