# Stage 01：Console 直接调用

## 新需求与约束

使用 PC 标准输入输出模拟串口终端，不使用 LVGL、SDL、触摸或显示对象。

支持命令：

- `heart`：进入 Heart 详情。
- `steps`：进入 Steps 详情。
- `back`：返回 Home。
- `show`：重新输出当前页面。
- `help`：显示命令帮助。
- `quit`：退出程序。

本 Stage 仍使用直接调用，不提前建立 Intent、Core、Renderer、EventQueue、PageManager 或生命周期框架。

## 当前预期调用链

```text
stdin 命令
-> main 输入循环
-> handle_command()
-> s_current_page
-> render_current_page()
-> stdout
```

`help` 和 `quit` 属于终端程序控制，由输入循环处理。页面命令由你在 `handle_command()` 中实现。

## 首次编码后补记

本 Stage 在首次编码前没有填写预测。以下内容是首次写出可运行版本后补记，不能作为事前判断：

- 我认为 `s_current_page` 是否是唯一页面真相，为什么：
是的，当前处于什么页面只由他决定
- 我认为 `handle_command()` 本阶段是否可以直接修改页面状态：
可以，他接受输入，处理，修改页面
- 首页执行 `back` 应该是什么行为：
无处理，或者打印命令非法
- 非法命令是否应该重新渲染，为什么：
可以渲染，也可以不处理直接丢掉
- 增加到十个页面时，我预计最先变贵的代码是：
handle_command()
这里处理页面流转，不同页面接收不同指令，切到什么页面会随着页面增加指数级膨胀
渲染页面膨胀的速度只是o(n)
s_current_page枚举类型膨胀的速度也只是o(n)
- 什么实际证据出现后，我才会考虑 Intent 或 Renderer：
Intent和Renderer是什么意思

## 实际实现与观察

- `s_current_page` 是唯一页面状态来源。
- `heart`、`steps` 和有效 `back` 直接修改页面状态，再调用 `render_current_page()`。
- `show` 只读取状态并渲染，不修改页面。
- 首页 `back` 是合法 no-op，输出 `Already on Home.`。
- 非法命令不改变状态，输出 `Unknown command.`。

首次实现曾在 `show` 分支内重复一套页面 `switch`。这没有形成第二份页面状态，却形成了第二份页面输出规则。修正后，所有页面输出统一经过 `render_current_page()`。

当前实际调用链：

```text
stdin 字符串
-> handle_command()
-> 修改或读取 s_current_page
-> render_current_page()
-> stdout
```

## 收益、代价和判断修正

收益：

- 去掉 LVGL 后，页面状态必须显式存在，`s_current_page` 的所有权容易观察。
- 输入字符串、页面状态和输出表现已经能够分辨。
- 当前只有三个页面，直接 `if` 和 `switch` 足够完成行为。

代价与风险：

- `handle_command()` 同时识别字符串、判断导航规则、修改状态并决定何时渲染。
- 页面和命令增加后，它的困难主要来自职责与组合数量增长，不宜简单称为指数复杂度。
- 当前 `render_current_page()` 已是最朴素的 Renderer 职责，但还没有证据要求拆成模块。

概念校准：

- Intent 是输入被解释后的动作含义，例如 `"heart"` 可以被解释为“打开 Heart”。
- Renderer 根据当前状态生成输出，不决定系统应跳到哪里。
- 本 Stage 只理解这两个概念，不实现对应抽象。

## 冻结决定

Stage 01 冻结并保留：

- 保留单文件、直接命令判断和显式 `s_current_page`。
- 保留 `render_current_page()` 作为唯一页面输出入口。
- 延迟 Intent、Core、Renderer 接口、EventQueue、PageManager 和生命周期。
- 下一阶段必须通过新增需求观察 `handle_command()` 是否真的开始承担过多上下文规则。
