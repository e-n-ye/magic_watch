# Stage 02：上下文相关菜单导航

## 本轮新增压力

Stage 01 只有一条详情进入路径。Stage 02 增加 App Menu，并保留 Home 快捷入口：

```text
Home -> heart/steps -> Detail
Home -> menu -> select -> enter -> Detail
```

同一个详情页现在可能有两个 Back 目标。你需要亲自判断，仅有 `current_page` 是否足够，以及还需要保存什么事实。

## 预测校准

你写了：

> 仅有 `current_page` 可以支持两种详情入口。

同时又提出：

> 使用一个全局变量记忆上一页面。

这两句不能同时成立。只要 Back 规则需要读取“上一页面”，系统就已经拥有 `current_page` 之外的第二个导航事实。

这不代表“上一页面变量”一定错误，也不代表必须使用页面栈。本 Stage 要通过直接实现验证：

- 保存一个返回页面是否已经足够。
- 它应该在什么时候更新。
- 菜单选择是否是另一种独立状态。
- 快捷入口、菜单入口和 Back 是否容易漏改。

## 目标行为

- `menu`：仅从 Home 进入 App Menu。
- `next` / `prev`：在 Heart、Steps、About 三项间循环。
- `enter`：进入当前菜单项对应详情。
- `heart` / `steps`：保留从 Home 直接进入详情。
- 菜单进入详情后，`back` 返回菜单并保留选中项。
- Home 快捷进入详情后，`back` 返回 Home。
- 菜单中 `back` 返回 Home。
- Home 中 `back` 输出 `Already on Home.`。
- 合法但当前页面不可用的命令输出 `Command unavailable on this page.`。
- 未知字符串输出 `Unknown command.`。

## 实现边界

本 Stage 实际补充了四部分：

1. 选择并定义菜单选中项状态。
2. 选择并定义详情 Back 所需的来源或返回状态。
3. 完成 `menu`、`next`、`prev`、`enter` 和上下文相关的 `back`。
4. 完成 App Menu 与 About 的输出。

第一版继续使用：

- 单个 `main.c`。
- 全局静态状态。
- 字符串直接判断。
- `handle_command()` 直接修改状态。
- `render_current_page()` 直接打印。

实现保持在单文件直接结构中，没有引入 Intent、Core、Renderer 接口、页面栈或 PageManager。

## 实现前导航表

编码前先填写：

| 当前页面 | 命令 | 状态变化 | 输出/目标页面 |
| --- | --- | --- | --- |
| Home | `menu` | `current_page = App Menu` | App Menu，保留原菜单选中项 |
| Home | `heart` | `detail_return = Home`，进入 Heart | Heart |
| Home | `back` | 不改变状态 | `Already on Home.` |
| App Menu | `next` | 选中项循环后移 | App Menu 新选中项 |
| App Menu | `prev` | 选中项循环前移 | App Menu 新选中项 |
| App Menu | `enter` | `detail_return = App Menu`，显式映射到详情 | Heart / Steps / About |
| App Menu | `back` | `current_page = Home` | Home |
| Detail（快捷入口） | `back` | 读取 `detail_return` | Home |
| Detail（菜单入口） | `back` | 读取 `detail_return` | App Menu，保留选中项 |

## 本次实现观察

- `current_page` 只能说明当前页面，无法独立决定详情 Back 的目标。
- 当前需求只需要一份 `detail_return_page`，还没有出现不定深度的导航历史，因此不引入页面栈。
- 菜单选中项是独立状态，离开菜单后保留，使详情 Back 返回时仍显示原选中项。
- 菜单项到页面使用显式 `switch` 映射，不依赖两个枚举的数值排列。
- 已区分未知字符串与“命令已知但当前页面不可用”。

## 收益、代价和判断修正

收益：

- 单文件直接实现仍能清楚表达当前有限的导航深度。
- 三份状态分别回答“在哪”“选了谁”“详情返回哪”，所有权仍容易定位。
- 菜单选中项离开菜单后保留，返回菜单时无需重建。

代价：

- `handle_command()` 已同时按页面和命令组织规则，新增行为需要在较大的控制流中寻找位置。
- 每个新命令都要同时考虑“是否已知”和“哪些页面可用”。
- 进入详情的每条路径都必须记得设置 `detail_return_page`，漏写会产生错误 Back。

判断修正：

- Stage 01 的 `current_page` 在单一返回路径下足够；Stage 02 的两种详情入口证明它不再足够。
- 一个返回目标可以解决固定两层导航，但它不是通用历史。
- 只有出现不定深度或连续嵌套页面时，才有引入 push/pop 页面栈的证据。

## 冻结决定

Stage 02 冻结并保留：

- 保留单文件直接结构和三份显式状态。
- 保留页面相关命令判断与直接渲染。
- 延迟页面栈、Intent、Core、Renderer 接口和 PageManager。
- 下一阶段不继续增加导航深度，而应加入第二种输入表示，观察字符串命令是否开始泄漏到共享行为。
