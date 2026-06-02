---
name: doc-memory-workflow
description: Use when a Magic Watch task is large, multi-step, cross-module, a refactor/plan, or needs persistent documentation as memory across sessions. Trigger for planning big modules, splitting complex work into small verifiable loops, creating or updating docs before implementation, validating completion against docs, deciding whether a new doc becomes a new-session entry or on-demand route, and keeping docs current without loading irrelevant documents into context. 适用于大模块重构、复杂计划、文档持续记忆、任务验收、新窗口必读判断、按需阅读路由和避免文档追代码滞后。
---

# Doc Memory Workflow

让长期任务始终和项目文档同步，同时保护上下文预算。把文档当作持久记忆，但按路由和任务需要加载，而不是凭惯性全读。

## Start Gate

进入实现前，先判断任务形态：

- 如果任务触及多个模块、页面层级、状态/数据/事件流、回归范围或后续窗口，拆成可独立验收的小闭环。
- 如果任务过大，先说明为什么不能一轮做完，再拆分，并且本轮只执行当前闭环。
- 如果需要跨窗口记忆，在实现前或实现过程中创建/更新最小相关文档。
- 如果不需要持久记忆，不要为了“留痕”新增文档。

只读取最低必要文档：

- 先读仓库要求的入口文档；存在 `docs/document_map.md` 时，用它选路由。
- 从文档地图中选择 1-3 个任务相关文档。
- 长篇当前状态文档只作为按需事实基线，不自动放进默认必读。
- 历史审计和旧决策只在当前任务明确需要时读取。

## Planning Loop

规划大模块、重构或复杂计划时，记录：

- Goal: 本闭环要完成什么。
- Boundary: 本轮涉及哪些文件、模块、文档和行为。
- Non-goals: 哪些相邻工作刻意不做。
- Evidence: 计划依据的当前代码/文档事实。
- Verification: 结束时要跑的命令、检查、人工验证或文档复核。
- Next-window routing: 后续窗口应默认读取，还是只按需读取。

计划应短到能服务实现。优先使用 checklist 或路由卡，不写成长篇说明书。

## Execution Loop

实现过程中：

- 当代码改动和文档记忆描述同一个已成立事实时，把它们放在同一个逻辑闭环里收口。
- 默认把“本轮代码文件 + 对应状态文档”一起纳入实现类 prompt 的 Allowed files；只有协作判定规则本身发生变化时，才额外允许修改 `.agents/skills/doc-memory-workflow/SKILL.md`。
- 如果 prompt 没有明确禁止文档修改，就不要把代码收口和文档收口默认拆成两轮。
- 如果 prompt 明确禁止改文档，则不得越权补文档；应在收尾报告里显式给出 `Doc Impact`、`Suggested docs` 和 `Need docs sync before next code round`。
- 只有事实已经成立时才更新文档，不把计划中的事情写成已完成。
- 对不确定、待验证、仅人工检查的内容如实标注。
- 不把构建通过写成手动 UI 回归通过。
- 修一个路由或事实缺口时，不顺手改无关文档。

如果新事实改变了后续窗口进入项目的方式，优先更新文档路由，而不是扩散改写所有下游文档。

## Completion Gate

收尾前，用本闭环的文档反向验收：

- 复读本轮修改过或作为验收依据的文档。
- 检查文档事实是否匹配实际代码/文档 diff。
- 如果任务声明了禁止范围，检查是否越权。
- 运行可用的验证命令。
- 对本轮改动过的中文文件做一次乱码哨兵检查。
- 报告改了什么、验证了什么、什么没验证、是否需要后续闭环。

收尾时始终显式给出 `Doc Impact` 判断，并按“是否新增持久结构事实”而不只按“是否改了行为/归属”来判：

- `none`：只改实现细节，没有新增会影响后续理解代码结构的稳定事实。
- `small`：新增了模块、子域、helper 分类、构建登记或同层级结构边界，但没有改变页面归属、注册入口或行为主线。
- `required`：如果本轮不补文档，下一轮会明显误判页面归属、结构落点、文档路由、验收边界或实施顺序。

如果代码轮新增了新的单域 helper 文件、二级目录、域内 primitives / helpers 分类，即使页面未迁、行为未变，通常也至少是 `small`。

如果没有执行人工验证，直接说明未执行。

## New-Session Routing

当新建或更新的文档可能影响后续窗口时，判断它属于哪类入口：

- Default entry: 短、稳定、每个新窗口都确实需要的文档。
- On-demand current fact baseline: 回答“当前真实状态是什么”的较长状态文档。
- Task route: 只在继续特定模块、回归、简历叙事、设计区域或历史问题时读取的文档。
- Historical reference: 审计、旧决策和旧调查，保留可检索，但不默认读取。

如果某个文档成为新的默认入口或重要按需路由，在同一闭环更新 `docs/document_map.md` 或项目等价地图；如果当前任务明确禁止改路由，则在报告中说明需要下一轮补路由。

## Context Budget Rules

- 优先看文档地图、摘要和标题，再决定是否读完整长文档。
- 已知关键词先定向搜索，再打开大文件。
- 不把“读取所有 current 文档”设成默认路径。
- 新文档只服务一个持久目的：计划、当前事实基线、路由地图、回归清单或叙事素材。
- 默认路径变臃肿时，把细节拆回按需路由。
