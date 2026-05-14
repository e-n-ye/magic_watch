# Magic Watch Learning Plan

## Learning Goal

本项目的核心学习目标，是从“能写嵌入式功能”升级为“能设计和维护复杂嵌入式系统”。

过去的经验包括：

- 前后台时间片轮询。
- FreeRTOS 下多个循环式任务。
- 简单 FSM/HSM 和 QPC 思想接触。
- 阅读 InfiniTime、X-TRACK 等较大项目时难以抓住骨干。

本计划要解决的问题是：如何把这些碎片变成可落地的系统设计能力。

## Stage 1: From Loop Thinking To Event Thinking

目标：

- 分清“任务循环”和“系统行为模型”的区别。
- 学会用事件描述系统变化，而不是让每个任务自己轮询世界。
- 理解输入、时间、电池、传感器、导航都可以作为事件源。

实践：

- 在模拟器中梳理现有 `Application / DataCenter / EventBus / PageManager / StateMachine` 的职责。
- 画出最小事件流：平台事件 -> 数据中心 -> 状态机/UI。
- 避免页面直接访问硬件。

## Stage 2: State Machine As System Backbone

目标：

- 把系统行为放进明确的状态边界，而不是散落在 UI 回调和任务循环中。
- 理解电源态、屏幕态、导航态、应用态之间的关系。

实践：

- 先维护一个轻量系统状态机。
- 只在确实需要层级状态时引入 HSM 概念。
- 学习 QPC 的思想，但不急于引入完整框架。

## Stage 3: Data-Driven UI

目标：

- UI 只表达状态，不拥有系统真相。
- 数据模型变化驱动页面更新。
- 页面生命周期清楚，避免页面间互相调用。

实践：

- 为时间、电池、运动、通知等建立稳定模型。
- 页面订阅模型变化。
- 导航通过命令/事件表达。

## Stage 4: Platform Abstraction

目标：

- 模拟器、TTGO_TWatch、未来低功耗硬件可以共享上层应用结构。
- 硬件选择不反向污染 App 层。

实践：

- `Platform` 层负责设备输入输出和系统 tick。
- `Service` 层负责领域服务，如时间、电池、传感器、存储。
- `App` 层只关心模型、状态和导航。

## Stage 5: RTOS And Power Awareness

目标：

- 不把 FreeRTOS 当成“多个 while 循环容器”。
- 任务应服务于事件、队列、服务生命周期和功耗状态。
- 低功耗不是最后补丁，而是系统状态的一部分。

实践：

- 明确哪些服务常驻，哪些按需启动。
- 为 screen off、sleep、deep sleep、charging 等状态定义行为。
- 记录唤醒源、数据一致性和恢复流程。

## Stage 6: Product-Level Completion

目标：

- 逐步接近 InfiniTime 级别的完整度。
- 形成可演示、可维护、可复盘的智能手表系统。

实践：

- 分阶段实现表盘、启动、设置、传感器、运动、存储、调试、升级等能力。
- 每个功能都经过需求、架构、实现、验证、复盘。
- 保持小步合入，避免一次性堆出不可维护系统。
