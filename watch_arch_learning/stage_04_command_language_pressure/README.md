# Stage 04：命令字符串是否已变成隐式内部合同

## 新需求与约束

在 Stage 03 的终端命令和 `key_*` 之外，再增加一个新的调用入口：

- `replay_about_roundtrip`
- `replay_steps_roundtrip`

它不是终端逐条输入，也不是实体按键模拟，而是“脚本回放入口”。
这个入口天然更像“我要完成一组行为步骤”，而不是“我要发送一串终端文本”。

本阶段仍然保持：

- 相同页面
- 相同导航深度
- 相同同步循环
- 相同 `handle_command()`

不引入异步、队列、共享动作枚举或更多页面。

## 我的事前预测

完整预测保存在：

- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage04-q17.md`

本轮故意不把整个系统升级成共享动作枚举，而是先给新入口一个本地步骤语言，
看它是否会被迫再翻译回 `"menu"`、`"next"`、`"enter"`、`"back"`。

## 当前调用链

终端命令：

```text
输入字符串
-> handle_command()
-> 修改页面状态
-> render_current_page()
```

实体按键：

```text
key_* 字符串
-> translate_key_input()
-> 命令字符串
-> handle_command()
-> 修改页面状态
-> render_current_page()
```

回放入口：

```text
replay_* 命令
-> run_replay_command()
-> replay_step_t
-> translate_replay_step()
-> 命令字符串
-> handle_command()
-> 修改页面状态
-> render_current_page()
```

## 当前状态所有权

- `s_current_page`：当前页面
- `s_menu_selection`：菜单选中项
- `s_detail_return_page`：详情页 Back 返回目标

本轮没有增加新的业务状态，但增加了一个新的“调用入口私有步骤语言”：

- `replay_step_t`

注意它不是系统共享动作枚举，只是回放入口本地使用的步骤表示。

## 实际修改范围

- 保留 Stage 03 的终端命令与 `key_*` 行为不变
- 新增 `is_replay_command()`
- 新增 `run_replay_command()`
- 新增 `translate_replay_step()`
- `handle_command()` 仍然只认识命令字符串

## 新入口矩阵

请至少人工运行以下三组。

### 回放入口回归

```text
replay_about_roundtrip
show
replay_steps_roundtrip
show
```

### 与旧输入混用

```text
menu
key_next
back
replay_steps_roundtrip
heart
back
```

### 非法时机观察

```text
menu
replay_about_roundtrip
```

此时应观察：

- 回放会从当前状态继续，不会偷偷重置系统
- 如果步骤在当前页面不成立，会输出 `Replay step unavailable on this page.`
- 回放入口虽然有自己的步骤语言，最终仍要翻译成旧命令字符串

## 自动观察到的事实

- `translate_replay_step()` 不是终端输入，但仍要返回 `"menu"`、`"next"`、`"enter"`、`"back"`。
- 回放入口本地已经出现了“步骤语言”，但旧导航逻辑并不理解它。
- 如果想摆脱命令字符串，当前第一刀不会落在实体按键，而会落在
  `handle_command()` 这一层。
- 当前步骤语言还只是回放入口私有表示，尚未证明必须把它扩张成系统共享动作枚举。
- 为了让 `replay_steps_roundtrip` 和 `replay_about_roundtrip` 不受当前菜单选中项污染，
  回放入口还必须读取 `s_menu_selection`，并额外发出若干次 `REPLAY_STEP_MOVE_NEXT`。
- 这说明“我要去 Steps/About”这种更高层语义，当前还不能直接被旧导航消费，
  必须先降格为多次 `"next"` 加一次 `"enter"`。

## 运行后由用户回答

1. 回放入口最自然表达的是“字符串命令”，还是“步骤语义”？
字符串只是一种输入形式，回放测试直接表达步骤语义会更直接

2. 为什么 `translate_replay_step()` 仍然被迫返回字符串？
因为核心的修改页面流转的handle_command函数只认字符串命令

3. Stage 04 比 Stage 03 多暴露了哪一层别扭？
回放入口在确认方向后还需二次翻译回字符串

4. 现在如果引入最小共享动作语义，最可能替代的是哪一层？
handle_command层，改为使用共享语义修改s_current_page

5. 仅凭 Stage 04，是否已经足够证明共享动作枚举值得引入？
还不足以直接证明“必须马上引入”，但已经足以证明这不再只是“代码风格优化”。
Stage 04 首次给出了真实证据：

- 新入口天然想表达步骤语义，而不是终端文本。
- 旧导航却只能消费字符串命令。
- 为了表达更高层目标，回放入口还要自己补一段把目标拆成 `"next"` 和
  `"enter"` 的中间逻辑。

所以现在已经值得单开下一阶段，专门验证“最小共享动作语义”是否真的能减少
这种中间翻译和扩散。但这仍然是“值得验证”，不是“无需验证就该直接上”。

## 本轮结论

- Stage 03 证明了硬件名可以被隔离。
- Stage 04 则开始证明：就算没有硬件名，新入口仍要伪装成命令字符串，才能复用现有导航。
- 而且一旦新入口想表达“去某个具体菜单目标”，它还要自己补一段把高层意图拆成
  多次 `"next"` 的中间逻辑。

这已经足以说明：当前系统里的字符串命令，不再只是方便的输入形式，而是开始承担
“隐式内部合同”的角色。

## 冻结决定

Stage 04 冻结并保留：

- 保留 Stage 03 的终端命令和 `key_*` 入口。
- 保留 Stage 04 的本地回放步骤语言。
- 不在本轮直接把本地步骤枚举升级成系统共享动作枚举。

下一阶段如果继续推进，唯一合理的新压力就是：

- 验证最小共享动作语义，是否真的能替代字符串命令这层隐式合同。
