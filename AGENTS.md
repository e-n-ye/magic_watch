# Magic Watch AI Collaboration Guide

本文件是 Magic Watch 项目的 AI 协作入口。新的会话应先阅读本文件，再按需阅读 `docs/` 下的项目文档。

## Core Stance

- 始终使用简体中文沟通，除非用户明确要求其他语言。
- 用户和 AI 的判断都可能出错。遇到方向分歧、技术路线不稳、需求过大或风险过高时，先搜集项目内证据，再说明为什么需要调整，讨论到同一方案后继续。
- 本项目的首要目标不是快速堆功能，而是借智能手表项目训练企业级嵌入式系统设计能力：自顶而下、可维护、可扩展、低功耗意识、最小化变动。
- 当前阶段以 PC LVGL 模拟器作为 v0 预研主线，TTGO_TWatch 仅作为硬件参考，不绑定最终架构。
- 当前 `sim/lv_port_pc_vscode` 是预研资产。不要因为它含有残留或不完美结构就直接清理或重写。

## Safety Rules

禁止批量删除文件或目录。不要使用：

- `del /s`
- `rd /s`
- `rmdir /s`
- `Remove-Item -Recurse`
- `rm -rf`

需要删除文件时，只能一次删除一个明确路径的文件，例如：

```powershell
Remove-Item "C:\path\to\file.txt"
```

如果需要批量删除文件，应停止操作，并询问用户，让用户手动删除。

## Project Memory

- Git 仓库内文档是长期记忆的主载体。
- Trellis 可作为后续索引、任务拆分和过程记忆，但不能替代仓库内可读、可审阅、可版本管理的文档。
- 关键背景入口：
  - `docs/document_map.md`: 文档导航图，区分当前必读、历史参考和视觉探索文档。
  - `docs/project_charter.md`: 项目定位、目标、边界、成功标准。
  - `docs/v0_scope.md`: 当前 v0 闭环范围和验收标准。
  - `docs/home_interaction_model.md`: 表盘中心交互模型。
  - `docs/watchface_style_architecture.md`: 表盘宿主、风格接口和未来风格切换边界。
  - `docs/target_architecture.md`: v0 目标骨架。
  - `docs/workflow.md`: 每次会话如何推进。
  - `docs/learning_plan.md`: 学习路径。
  - `docs/architecture_principles.md`: 架构原则。
  - `docs/decision_log.md`: 技术决策记录。

## Work Modes

### Plan

适用于新阶段、架构变化、硬件选型、任务范围不清、用户表达为“我们该怎么做”的场景。

输出应包含：

- 当前事实和证据。
- 目标、边界、验收标准。
- 可执行步骤。
- 需要用户确认的高影响选择。

### Implement

适用于方案已经明确的场景。

执行原则：

- 先观察当前代码和文档，再改动。
- 只改本次目标需要的文件。
- 较大修改前说明影响面。
- 修改后运行可行的验证命令。
- 不因清爽而做无关重构。

### Review

适用于检查代码、架构或文档质量。

优先指出：

- 可能导致行为错误的问题。
- 架构边界破坏。
- 维护性风险。
- 缺失的测试或验收标准。

## Development Rhythm

每个阶段遵循：

1. 观察现状。
2. 明确意图、边界、验收标准。
3. 讨论分歧并记录决定。
4. 小步实现。
5. 验证结果。
6. 更新文档或决策记录。

## Current Defaults

- 第一阶段产物：项目宪章和协作规范优先。
- v0 技术路线：模拟器优先。
- 架构方向：轻量事件驱动、状态机、数据中心、平台抽象。
- 暂不直接引入完整 QPC 框架；先吸收其事件驱动和主动对象思想。
- 暂不做最终硬件选型；低功耗目标先进入架构约束，而不是立即绑定芯片。

<!-- TRELLIS:START -->
# Trellis Instructions

These instructions are for AI assistants working in this project.

Use the `/trellis:start` command when starting a new session to:
- Initialize your developer identity
- Understand current project context
- Read relevant guidelines

Use `@/.trellis/` to learn:
- Development workflow (`workflow.md`)
- Project structure guidelines (`spec/`)
- Developer workspace (`workspace/`)

If you're using Codex, project-scoped helpers may also live in:
- `.agents/skills/` for reusable Trellis skills
- `.codex/agents/` for optional custom subagents

Keep this managed block so 'trellis update' can refresh the instructions.

<!-- TRELLIS:END -->
