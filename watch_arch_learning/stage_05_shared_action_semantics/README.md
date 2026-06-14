# Stage 05：最小共享动作语义是否值得引入

## 新需求与约束

Stage 04 已经证明：

- `key_*` 能被隔离
- `replay_*` 天然想表达步骤语义
- 但旧导航只能消费字符串命令

所以这一轮只做一件事：

- 把三类入口都先翻译成一套最小共享动作语义
- 再由共享动作语义驱动同一份页面状态

本轮仍然不引入：

- 异步
- 队列
- 第二 View
- 更深导航
- Power

## 本轮选择的最小共享动作语义

这一轮选的不是“页面状态修改”，也不是“目标页面枚举”，而是更靠近共享行为的最小动作集合：

- `SHARED_ACTION_OPEN_MENU`
- `SHARED_ACTION_OPEN_HEART_SHORTCUT`
- `SHARED_ACTION_OPEN_STEPS_SHORTCUT`
- `SHARED_ACTION_MOVE_NEXT`
- `SHARED_ACTION_MOVE_PREV`
- `SHARED_ACTION_ACTIVATE_SELECTION`
- `SHARED_ACTION_NAV_BACK`

这些动作回答的是“想做什么”，不是“状态应该被赋成什么值”。

## 当前调用链

终端命令：

```text
输入字符串
-> translate_terminal_command()
-> shared_action_t
-> apply_shared_action()
-> 修改页面状态
-> render_current_page()
```

实体按键：

```text
key_* 字符串
-> translate_key_input()
-> shared_action_t
-> apply_shared_action()
-> 修改页面状态
-> render_current_page()
```

回放入口：

```text
replay_* 命令
-> replay_step_t
-> translate_replay_step()
-> shared_action_t
-> apply_shared_action()
-> 修改页面状态
-> render_current_page()
```

## 当前状态所有权

- `shared_action_t`：共享动作语义
- `s_current_page`：当前页面状态
- `s_menu_selection`：菜单选中项
- `s_detail_return_page`：详情页 Back 返回目标

这里最重要的区分是：

- `shared_action_t` 不是页面状态
- `shared_action_t` 也不是“去 About”这种具体目标页面
- `s_current_page` 和 `s_menu_selection` 仍然只是状态，不是动作语义

## 实际修改范围

- 去掉 Stage 04 的 `handle_command()`
- 保留终端命令、按键输入和回放入口三类入口
- 三类入口都先翻译成 `shared_action_t`
- 新增 `apply_shared_action()` 作为共享动作语义的唯一消费点

## 自动观察到的事实

- Stage 04 的“命令字符串”已经不再是三类入口的共同终点
- 三类入口现在共享的是 `shared_action_t`
- `apply_shared_action()` 仍然必须读取当前页面状态，因为动作是否可用取决于上下文
- 回放入口仍然保留本地 `replay_step_t`，说明“入口私有步骤语言”和“共享动作语义”不是同一层
- 为了表达“去 Steps/About”，回放入口仍要读取 `s_menu_selection` 并补发若干次 `MOVE_NEXT`
- 这说明最小共享动作语义减少的是“翻译终点不一致”，但还没有消除“更高层目标拆分”的问题
- Stage 04 中三类入口各自返回字符串；Stage 05 中三类入口都先返回 `shared_action_t`
- 终端命令字符串现在退回成“终端输入形式”，不再承担三类入口共同终点的角色

## 运行矩阵

请至少人工运行以下三组。

### 终端命令回归

```text
menu
next
enter
back
back
heart
back
```

### 按键输入回归

```text
key_select
key_next
key_select
key_back
key_back
```

### 回放与混合输入

```text
replay_about_roundtrip
replay_steps_roundtrip
menu
key_next
back
heart
back
```

## 运行后由用户回答

1. Stage 05 里三类入口共享的终点，是否已经比 Stage 04 更一致？
相对来说有更确定的语义了吧，标准语义参考
而且可以使得apply_shared_action函数有更加确定的组织模样

2. `shared_action_t` 和 `s_current_page` / `s_menu_selection` 的边界，你现在是否更清楚了？
shared_action_t只描述可能的动作，
apply_shared_action函数根据这些动作去修改s_current_page` / `s_menu_selection的值

3. Stage 05 真正减少了哪一层中间翻译？
Stage 05相比之前少了一个中间转译层，现在所有输入都有一个确定的终点shared_action_t

4. 哪些翻译仍然存在，比如“去 About”拆成多次 `MOVE_NEXT`？
menu页面选择的流转

5. 仅凭 Stage 05，你是否认为最小共享动作语义值得保留？
我认为值得保留，一切行为流转更加清晰，更有利于维护了

## 本轮结论

- Stage 05 已经证明，共享动作语义不只是“把字符串换个名字”
- 它至少把三类入口的共同终点，从命令字符串收口到了 `shared_action_t`
- 但它没有自动解决更高层目标拆分问题
- 它现在已经足够说明：`shared_action_t` 作为“多输入入口的最小共同动作语义”值得保留

所以本轮真正要判断的，不是“动作语义万能不万能”，而是：

- 这一级抽象的收益，是否已经大于它新增的样板

## 冻结决定

Stage 05 冻结并保留：

- 保留 `shared_action_t` 作为三类入口的共同终点
- 保留 `apply_shared_action()` 作为共享动作语义的唯一消费点
- 不把“去 About / 去 Steps”这种更高层目标塞进共享动作语义

下一阶段如果继续推进，最真实的新压力将不再是“还要不要再造一层目标语义”，而是：

- 时间维度和非用户事件进入系统后，`shared_action_t` 的边界还能不能保持干净
- 状态修改和显示刷新是否必须开始分离
