# Stage 03：第二种输入表示

## 新需求与约束

在 Stage 02 的终端命令之外，增加四个模拟实体按键：

- `key_next`
- `key_prev`
- `key_select`
- `key_back`

页面、菜单项、导航深度和三份既有状态保持不变。本阶段不接真实 GPIO、线程、异步事件或事件队列。

## 我的事前预测

完整预测保存在：

- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage03-q16.md`

本次选择方案 B：先把 `key_*` 翻译成 Stage 02 已有命令，再调用原 `handle_command()`。

## 第一版调用链

终端命令：

```text
输入字符串
-> handle_command()
-> 修改页面状态
-> render_current_page()
```

模拟实体按键：

```text
key_* 字符串
-> is_key_input()
-> translate_key_input()
-> 现有命令字符串
-> handle_command()
-> 修改页面状态
-> render_current_page()
```

## 当前状态所有权

- `s_current_page`：当前页面。
- `s_menu_selection`：菜单选中项。
- `s_detail_return_page`：详情页 Back 的返回目标。

第一版没有新增业务状态，但 `translate_key_input()` 为了解释 `key_select`，也开始读取 `s_current_page`。

## 实际修改范围

- Stage 02 的页面状态、导航规则和渲染代码原样保留。
- 新增按键输入识别。
- 新增按页面翻译按键的函数。
- `handle_command()` 不认识任何 `key_*` 字符串。

自动扫描得到的第一批事实：

- `translate_key_input()` 和 `handle_command()` 都读取 `s_current_page`。
- `handle_command()` 不认识 `key_*`，但仍直接认识 `next`、`prev`、`menu`、
  `enter`、`back` 等命令字符串。
- 把 `key_next` 改名为 `crown_cw`，当前至少涉及帮助文本、按键识别和页面翻译三处。
- 当前所有输入仍由同一个同步终端循环送入，尚未模拟编码器高频脉冲，因此“输入频率差异”仍未验证。

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

### 模拟按键

```text
key_select
key_next
key_select
key_back
key_back
```

### 混合输入

```text
menu
key_next
enter
key_back
key_prev
key_select
key_back
back
```

还要分别确认：

- Home 输入 `key_next`，输出 `Input unavailable on this page.`。
- Detail 输入 `key_select`，输出 `Input unavailable on this page.`。
- 未知字符串仍输出 `Unknown command.`。

## 运行后由用户回答

1. `handle_command()` 是否真的完全不知道 `key_*`？
handle_command完全不知道key_命令的存在，因为所有key_命令在if (is_key_input(input))中已被拦截翻译

2. `translate_key_input()` 为什么必须读取 `s_current_page`？
不同s_current_page，相同的key_select语义不同，需要读取页面信息才能翻译

3. 方案 B 消除了哪种重复，又把哪种耦合移动到了翻译层？
方案 B消除了不同输入相同语义按键的重复命令处理
但是把页面信息引入了翻译层我觉得耦合影响不大

4. 如果把 `key_next` 改名为 `crown_cw`，实际需要修改哪些代码？
最小代码改动至少包括 `print_help()`、`is_key_input()` 和
`translate_key_input()`；如果 README、测试矩阵或后续脚本也记录了
`key_next`，它们也需要一起更新。

5. `menu_selection++` 是输入语义，还是执行“选择下一项”后的状态修改？
menu_selection++是输入语义转化后的状态修改

6. 当前是否已经有足够证据引入动作枚举？若没有，还缺什么压力？
这里的“动作枚举”不是消抖状态机，而是像 `ACTION_OPEN_MENU`、
`ACTION_MOVE_NEXT`、`ACTION_ACTIVATE_SELECTION`、`ACTION_NAV_BACK`
这样的内部动作语言。

当前还没有足够证据必须引入它。Stage 03 已经证明：

- 硬件名 `key_*` 可以被隔离在翻译入口之外。
- 但 `handle_command()` 仍直接消费 `next`、`enter`、`menu`、`back`
  这些字符串命令。

也就是说，我们已经看到了“硬件表示”和“终端命令语言”不是同一层问题，
但还没有看到字符串命令本身造成了无法接受的修改扩散。

下一阶段真正该补的压力，不是消抖，也不是异步，而是：

- 如果再来一个不想伪装成终端文本的调用入口，
- 我们是否还愿意让它继续产出 `"next"`、`"enter"` 这种字符串，
- 还是该引入最小内部动作语义。

## 本轮结论

Stage 03 可以冻结，结论是：

- 方案 B 足以隔离硬件名称，不需要让页面导航直接认识 `key_*`。
- 方案 B 没有消除上下文耦合，只是把它移动到了 `translate_key_input()`。
- 当前系统真正共享的仍是“命令字符串语言”，不是内部动作枚举。
- 仅凭 Stage 03 的证据，还不值得马上引入动作枚举。

下一阶段应只增加一个新压力：攻击“命令字符串语言是否已经变成隐式内部合同”。

## 冻结决定

本 Stage 冻结并保留：

- 保留单文件、同步循环、字符串翻译方案。
- 不引入动作枚举、Input Adapter、EventQueue、异步输入。
- 不把翻译层读取页面状态直接升级成“必须抽象”的结论。

## 暂不下结论

本 Stage 已冻结。下一阶段再决定：

- 保留字符串翻译。
- 引入最小动作语义。
- 或继续增加需求收集证据。

不得仅因为翻译函数读取页面状态，就直接宣布 Input Adapter 或 Intent 必然正确。
