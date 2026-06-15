# Stage 06：时间维度、系统事件与 dirty render 压力

## 新需求与约束

Stage 05 已经证明：

- `shared_action_t` 值得保留为多输入入口的最小共同终点
- 但它仍然只属于用户动作语义
- 系统自己发生的变化，还没有进入当前结构

所以这一轮只做一件事：

- 在不增加页面规模的前提下，引入时间维度、系统事件和 dirty render

本轮仍然不引入：

- EventQueue
- 多线程
- RTOS
- 第二 View
- 更多页面
- 目标导航层

## 本轮最小事件分层

这一轮明确区分两层：

- `shared_action_t`：用户动作语义
- `app_event_t`：系统真正处理的外层事件

`app_event_t` 只承接四类事实：

- 用户动作
- `tick`
- `idle timeout`
- `battery changed`

这里最关键的边界是：

- `shared_action_t` 不是系统事件
- `battery changed`、`tick`、`idle timeout` 不能塞进 `shared_action_t`
- 第一拍唤醒行为也不等于“执行某个用户动作”

## 当前调用链

终端命令：

```text
输入字符串
-> translate_terminal_command()
-> shared_action_t
-> app_event_t(USER_ACTION)
-> dispatch_app_event()
-> 修改状态并置 dirty
-> render_if_dirty()
```

实体按键：

```text
key_* 字符串
-> translate_key_input()
-> shared_action_t
-> app_event_t(USER_ACTION)
-> dispatch_app_event()
-> 修改状态并置 dirty
-> render_if_dirty()
```

回放入口：

```text
replay_* 命令
-> replay_step_t
-> shared_action_t
-> app_event_t(USER_ACTION)
-> dispatch_app_event()
-> 修改状态并置 dirty
-> render_if_dirty()
```

系统事件：

```text
tick / battery 命令
-> app_event_t(TICK_1S / BATTERY_CHANGED)
-> dispatch_app_event()
-> 修改状态并置 dirty
-> render_if_dirty()
```

## 当前状态所有权

- `shared_action_t`：用户动作语义
- `app_event_t`：用户动作与系统事件的共同外壳
- `s_current_page`：当前页面状态
- `s_detail_return_page`：详情页 Back 返回目标
- `s_menu_selection`：菜单选中项
- `s_power_state`：最小电源状态
- `s_idle_seconds`：空闲秒数
- `s_clock_seconds`：系统秒计数
- `s_battery_percent`：电量状态
- `s_render_dirty`：等待输出的脏标记

## 运行矩阵

请至少人工运行以下四组。

### 基础用户动作回归

```text
menu
next
enter
back
back
heart
back
```

### 系统 tick 与 sleep

```text
tick 9
show
tick
show
```

### sleep 唤醒语义

```text
tick 10
next
show
next
show
```

### battery 与 dirty render

```text
battery 83
tick 3
show
replay_about_roundtrip
show
```

## 运行后由用户回答

1. `app_event_t` 和 `shared_action_t` 的边界，现在是否比规划阶段更具体？
2. sleep 下“第一拍只唤醒”的规则，看起来像产品规则还是技术补丁？
3. dirty render 这次真正减少了什么耦合？
4. 哪些地方仍然在直接依赖当前页面上下文？
5. 仅凭 Stage 06 第一版，你是否认为“系统事件外壳”值得保留？

## 自动观察点

- 一条顶层输入现在不一定只对应一次状态变化
- `tick 10` 可能触发多次 tick 和一次 idle timeout，但顶层只 render 一次
- sleep 下用户动作先经过电源门控，再决定是否执行共享动作
- `shared_action_t` 继续保持干净，没有被系统事件污染
