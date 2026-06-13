# Magic Watch 架构掌控力学习工作簿

> 状态：Paused，保留为历史学习现场  
> 开始日期：2026-06-13  
> 路由类型：历史学习记录  
> 原卡片：`LEARN-A`

## 路线暂停说明

本工作簿记录了从 F411 真实主链出发的阅读、推演和纠偏过程，用户已填写的内容全部保留。

2026-06-13 起，当前学习主线改为[架构冲突实验场总纲](/D:/MY_Desk/watch/magic_watch/docs/20_guides/watch_arch_learning_lab.md)：通过独立 PC 工程中的需求变化、方案选择和代价观察训练架构判断，而不是继续沿最终架构逐函数学习。

本文件不再作为自动执行入口。只有未来某个实验暴露出明确的 LVGL、FreeRTOS 或 Magic Watch 主链知识缺口时，才按需回到对应认知结，不恢复整套 `LEARN-A~D` 线性路线。

## 1. 使用方式

这不是一份考试题，也不是要求从陌生代码中一次逆向出完整架构。

从 2026-06-13 的第一次阅读反馈开始，学习方式修正为：

```text
先获得一张最小地图
-> 只追一个具体问题
-> 沿调用链验证
-> 写下自己的理解和质疑
-> Agent 纠偏
-> 再进入下一个认知结
```

固定规则：

- 一次只学习一个认知结，不要求先通读整个文件。
- 可以先读简化解释，再去代码中验证；学习不是闭卷考试。
- 优先沿调用关系阅读，不按文件清单横向扫读。
- 遇到不懂或认为设计不合理的地方，直接写在对应代码锚点下。
- 首次理解只回答“开始有什么、结束得到什么、为什么不能直接跨过去”。
- 状态所有权、跨平台边界和大表格放到整条链跑通之后再讨论。

## 2. 当前进度

已完成：

- 认知结 1：`scan_key()` 如何用 10ms 扫描、50ms 消抖和约 1s 阈值区分短按/长按。
- 认知结 2：Input FIFO 如何连接 10ms 的输入生产任务和 5ms 的 UI 消费任务。
- 认知结 3：LVGL 如何把 `CONFIRM` 转换成当前焦点卡片的点击回调。

当前进入：

- 认知结 4：卡片点击后，谁把 View 事实转换成产品事件，谁决定进入详情页。

这一结回到 Magic Watch 自己的框架，只区分 View、Adapter 和 Core 的职责，不学习 Power 或触摸手势。

## 3. 认知结 3：Confirm 如何变成卡片点击

### 3.1 先看最小地图

我们的代码没有直接调用 `on_card_clicked()`。

实际方向是：

```text
DefaultTask 排空 Input FIFO
-> WATCH_INPUT_INTENT_CONFIRM
-> watch_lvgl_port_feed_input_intent()
-> 设置一次 s_encoder_press_pulse

稍后的 watch_lvgl_port_task()
-> lv_timer_handler()
-> LVGL 输入设备定时器主动调用 watch_lvgl_encoder_read()
-> 第一次读到 PRESSED
-> 下一次读到 RELEASED
-> LVGL 找到 encoder group 当前焦点对象
-> LVGL 向该对象发送 LV_EVENT_CLICKED
-> 之前注册的 on_card_clicked() 被调用
```

这段链路中有两条方向：

```text
应用主动喂状态：Intent -> s_encoder_press_pulse
框架主动拉取状态：LVGL -> read_cb -> 当前焦点对象事件
```

### 3.2 第一段：应用只保存一次按压脉冲

只读：

1. `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
   - `publish_intent()`
2. `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
   - `watch_lvgl_port_feed_input_intent()`
   - `watch_lvgl_encoder_read()`

阅读时只追三个变量：

- `intent`
- `s_encoder_press_pulse`
- `data->state`

我的时间线推演：

> 待填写：`CONFIRM` 到来后，哪个变量被写成什么值？ 
watch_lvgl_port_feed_input_intent函数中接收到 WATCH_INPUT_INTENT_CONFIRM
s_encoder_press_pulse = 1U;

> 待填写：LVGL 第一次读取时得到什么状态？变量随后怎样变化？ 
 watch_lvgl_encoder_read函数不知道什么时候被调用，
 但是如果s_encoder_press_pulse = 1U;的话
 data->state = LV_INDEV_STATE_PR;

 ps.
 data->enc_diff = s_encoder_diff;
    s_encoder_diff = 0;
    这是在干啥，看不懂

> 待填写：LVGL 下一次读取时为什么会得到释放状态？
有点没看懂题目。
指的是
data->state = LV_INDEV_STATE_PR;
        s_encoder_press_pulse = 0U;
        被重置的事情吗



### 3.3 第二段：是谁调用 `watch_lvgl_encoder_read()`

先看项目注册代码：

1. `watch_lvgl_port_init()`
   - `s_encoder_drv.type = LV_INDEV_TYPE_ENCODER`
   - `s_encoder_drv.read_cb = watch_lvgl_encoder_read`
   - `lv_indev_drv_register(&s_encoder_drv)`
2. `watch_lvgl_port_task()`
   - `lv_timer_handler()`

再只看 LVGL 内部四个锚点，不要通读整个文件：

1. `third_party/lvgl/lvgl/src/hal/lv_hal_indev.c`
   - `lv_indev_drv_register()` 中创建 `read_timer`
   - `_lv_indev_read()` 中调用 `indev->driver->read_cb(...)`
2. `third_party/lvgl/lvgl/src/core/lv_indev.c`
   - `lv_indev_read_timer_cb()`
   - `indev_encoder_proc()`

我的理解：

> 待填写：为什么业务代码里搜索不到“调用 `watch_lvgl_encoder_read()`”的直接调用点？  
因为这个函数作为函数指针在watch_lvgl_port_init初始化时，被导入lvgl内部的体系里了

> 待填写：`lv_timer_handler()`、输入设备 timer 和 `read_cb` 三者是什么关系？
lv_timer_handler函数看的头晕，
但是他感觉是一个时钟基准，时间到了做某事
输入设备 timer，每一个输入设备可能都有自己的timer,
到了时间会调用read_cb，这个时候watch_lvgl_encoder_read之前在初始化的时候注册进去的函数指针被调用
其实我不是很喜欢回调函数这种方式去构建一个模型，作为一个库融入其他工程，我觉得留一个参考实现，和一个空模板，移植的时候往里面写就好了，这种回调函数，看着很绕


### 3.4 第三段：LVGL 怎么知道要点击哪张卡

只读：

1. `watch_lite_view.c`
   - `watch_lite_view_init()`
   - `create_shortcuts_view()`
   - `rebuild_focus_group()`
2. `watch_lvgl_port.c`
   - `watch_lvgl_port_set_group()`
3. LVGL `indev_encoder_proc()`
   - `lv_group_get_focused(g)`
   - 按下时的 `LV_EVENT_PRESSED`
   - 释放时的 `LV_EVENT_CLICKED`

先建立这三个事实：

- 每张卡创建时注册了同一个 `on_card_clicked()`，但携带不同的 feature 作为 `user_data`。
- 当前页面把可操作对象加入 `s_focus_group`，并指定初始焦点。
- encoder 输入设备绑定到这个 group；LVGL 把点击事件发给 group 当前焦点对象。

我的推演：

> 待填写：进入四卡首页后，初始焦点是哪一个对象？证据是什么？  
初始焦点对象应该是WATCH_CORE_HEALTH_FEATURE_INVALID，因为watch_lite_view_init默认配置
s_current_page.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;

> 待填写：表冠旋转后，谁改变当前焦点？项目代码是否需要知道下一张卡是谁？  
我对于1vgl不熟悉，很难看的明白这里的代码在干啥，实际的触发在哪


> 待填写：按下表冠后，`on_card_clicked()` 如何知道被激活的是哪个 feature？
看不懂


### 3.5 本认知结只回答六个问题

1. `watch_lvgl_port_feed_input_intent()` 是在“调用 LVGL”，还是在“准备 LVGL 下次读取的数据”？

> 待填写
watch_lvgl_port_feed_input_intent()只是修改了一个也会被lvgl那边的编码器语义转化读取的一个值s_encoder_press_pulse

2. 谁真正调用 `watch_lvgl_encoder_read()`？

> 待填写
lvgl内部的timer调用watch_lvgl_encoder_read函数

3. 为什么一次 `s_encoder_press_pulse` 可以形成 LVGL 看到的“按下 -> 释放”？

> 待填写
他感觉从一个现实的脉冲，被转化为了内存空间的一个变量的读写01脉冲

4. 如果 encoder 没有绑定 group，或者 group 中没有焦点对象，会发生什么？

> 待填写
不知道，看不懂group，不知道什么意思

5. 为什么 InputIntent 不需要知道当前焦点是 Heart Rate 还是 SpO2？

> 待填写
不知道

6. 这条链中，硬件输入语义、LVGL 框架机制和产品业务语义分别从哪里开始？

> 待填写
不知道

### 3.6 一个值得保留的质疑

`s_encoder_press_pulse` 只有一个 `uint8_t` 标志。若在 LVGL 来得及读取之前连续写入两次 `CONFIRM`，两次输入可能合并为一次。

本认知结只判断：

> 待填写：结合 5ms DefaultTask、LVGL input read period 和正常人按压速度，这在当前 V0 是可接受限制，还是已经需要修改？

不要在本轮修改代码。

### 3.7 Agent 纠偏：你缺的不是整套 LVGL

这次作答已经抓住了本认知结最难的结构：

- `watch_lvgl_port_feed_input_intent()` 没有直接点击卡片，只是准备 LVGL 下次读取的数据。
- `watch_lvgl_encoder_read()` 作为函数指针注册给 LVGL，所以业务代码中找不到直接调用点。
- `lv_timer_handler()` 推动 LVGL 的 timer，输入设备 timer 再调用已注册的 `read_cb`。
- 一次现实按键最终被转换成 LVGL 可观察的状态变化。

目前真正缺少的是四个最小概念，而不是先系统学习整个 LVGL：

| 概念 | 在当前工程中的含义 |
| --- | --- |
| `read_cb` | 平台提供给 LVGL 的“读取当前输入状态”函数。应用保存输入结果，LVGL 决定何时来读。 |
| `group` | 一组可以被键盘或编码器导航的 LVGL 对象，同时保存当前焦点对象。 |
| `focus` | group 中当前被选中的对象。旋转表冠时，LVGL 在 group 内切换焦点。 |
| `event + user_data` | LVGL 向焦点对象发送点击事件；对象注册回调时携带的 `user_data` 用来还原它代表的业务 feature。 |

#### 纠正一：按下脉冲如何变成“按下 -> 释放”

时间线是：

```text
CONFIRM 到达
-> s_encoder_press_pulse = 1

LVGL 第一次调用 watch_lvgl_encoder_read()
-> 返回 LV_INDEV_STATE_PR
-> s_encoder_press_pulse 清零

LVGL 下一次调用 watch_lvgl_encoder_read()
-> 发现 pulse 已经是 0
-> 返回 LV_INDEV_STATE_REL
```

所以并不是变量自己连续产生两个状态，而是 LVGL 的两次读取看到不同结果。LVGL 根据前后状态变化识别出一次完整点击。

`data->enc_diff = s_encoder_diff; s_encoder_diff = 0;` 使用的是同一种“交付后清空”思路：

- `s_encoder_diff` 累积 LVGL 上次读取之后发生的旋转步数。
- 本次读取把这些步数交给 `data->enc_diff`。
- 随后清零，避免下一次重复消费相同旋转。

#### 纠正二：初始焦点不是 `WATCH_CORE_HEALTH_FEATURE_INVALID`

`s_current_page.feature` 表示 Lite View 当前页面对应的业务 feature，不是 LVGL 的焦点状态。首页没有单一详情 feature，所以它可以是 `INVALID`。

真正的焦点证据在 `rebuild_focus_group()`：

```c
for (i = 0U; i < WATCH_LITE_CARD_COUNT; ++i) {
    lv_group_add_obj(s_focus_group, s_card_objs[i]);
}
lv_group_focus_obj(s_card_objs[0]);
```

因此四卡首页初始焦点是 `s_card_objs[0]`，也就是 Heart Rate 卡片对象。

这正好暴露了一个重要的架构区别：

```text
s_current_page.feature    业务页面状态
s_focus_group 的焦点      LVGL View 交互状态
```

两者相关，但不是同一个状态，也不能互相代替。

#### 旋转和点击如何找到具体卡片

每张卡片都被加入同一个 group。表冠旋转产生的 `enc_diff` 交给 LVGL 后：

- 正方向由 LVGL 切到 group 中下一个对象。
- 负方向由 LVGL 切到上一个对象。
- 项目代码不需要计算“Heart Rate 的下一张是 SpO2”。

每张卡注册回调时，都附带自己的 feature：

```c
lv_obj_add_event_cb(
    card,
    on_card_clicked,
    LV_EVENT_CLICKED,
    (void *)(uintptr_t)s_card_meta[i].feature);
```

当 LVGL 向当前焦点对象发送 `LV_EVENT_CLICKED` 时，`on_card_clicked()` 通过 `lv_event_get_user_data(event)` 取回该卡片的 feature。

因此 `InputIntent` 只需表达“确认当前选择”，不需要知道当前选择是 Heart Rate 还是 SpO2。当前选择由 LVGL group 的焦点保存，卡片的业务身份由 `user_data` 保存。

#### 六个问题的当前结论

1. `feed_input_intent()` 在准备 LVGL 下次读取的数据，不是在直接点击 LVGL 对象。
2. LVGL 输入设备 timer 最终调用注册的 `watch_lvgl_encoder_read()`。
3. 第一次读取消费 pulse 并返回按下，下一次读取因 pulse 已清零而返回释放。
4. encoder 没有绑定 group，或 group 没有焦点对象时，LVGL 无法确定操作对象，不会形成卡片点击业务回调。
5. InputIntent 不知道 feature，因为 LVGL 的焦点对象和对象 `user_data` 共同保存了上下文。
6. 三类语义的边界是：

```text
WATCH_INPUT_ENCODER_PRESS -> WATCH_INPUT_INTENT_CONFIRM
    硬件输入语义到应用输入意图

pulse / read_cb / group / focus / LV_EVENT_CLICKED
    LVGL 框架机制

on_card_clicked() 取出 feature -> Adapter 创建 typed UiEvent
    产品业务语义
```

#### 关于回调方式的质疑

“回调看起来很绕”是准确的成本判断。回调把控制权交给框架，阅读时不能只搜索“谁直接调用了这个函数”，而要追三处：

```text
注册点 -> 框架触发点 -> 回调携带的状态
```

它换来的收益是：LVGL 可以用同一套输入处理、焦点导航和事件分发机制适配 encoder、键盘和不同平台，而不要求平台代码直接操纵具体页面对象。

这里不必先判断回调设计整体好坏。本认知结只需掌握一种阅读方法：看到函数指针时，先找“在哪里注册、由谁保存、什么条件下调用”。

#### `uint8_t` pulse 的当前判断

如果两个 `CONFIRM` 在 LVGL 两次读取之间连续写入，单 bit 标志确实可能把它们合并。这不是完全可靠的通用输入队列。

但当前输入来自需要按下、消抖、松手后才形成的物理短按，正常人的连续按压速度远低于 5ms 消费任务和 LVGL 输入轮询速度。因此它可以作为 V0 当前受约束场景下的可接受实现，不需要在本学习结修改。未来若接入高频合成输入或要求每次确认绝不丢失，才应改成计数器或队列。

### 3.8 你的复述任务

先不要继续读新文件。只用自己的话复述下面这条链，重点解释“谁保存状态、谁主动调用、谁知道当前卡片”：

```text
CONFIRM
-> s_encoder_press_pulse
-> read_cb 的两次读取
-> group 当前焦点
-> LV_EVENT_CLICKED
-> user_data 中的 feature
-> on_card_clicked()
```

能够不看上面的解释讲通这条链后，本认知结才算完成。下一认知结再从 `on_card_clicked()` 进入 `F411UiAdapter` 和 `watch_core`，不会同时混入更多 LVGL 内部知识。

## 4. 认知结 4：谁决定进入详情页

### 4.1 先回答你的判断

是的，从 `on_card_clicked()` 开始，主角重新变成 Magic Watch 自己的框架。

LVGL 在上一认知结已经完成它的职责：

```text
识别 encoder 点击
-> 找到焦点 LVGL 对象
-> 调用对象的 on_card_clicked()
```

LVGL 不知道下面这些产品概念：

- Heart Rate 或 SpO2 是什么业务 feature。
- 点击健康卡片后是否应该进入详情。
- 当前权威页面是什么。
- PC 和 F411 是否应采用相同导航语义。

这些问题由 View、`F411UiAdapter` 和 `watch_core` 继续处理。

### 4.2 最小地图

先只看“点击 SpO2 卡片进入 SpO2 详情”：

```text
watch_lite_view.c
on_card_clicked()
-> 从 LVGL user_data 取出 feature
-> 调用 s_callbacks->activate_health_card(feature)

f411_ui_adapter.c
on_activate_health_card()
-> f411_ui_adapter_activate_health_card(feature)
-> watch_core_make_health_card_clicked_event(feature)
-> f411_ui_adapter_dispatch_event(event)

watch_core
watch_core_push_event()
-> watch_core_process_pending_events()
-> watch_core_process_next_event()
-> 修改权威 WatchCorePageState
-> 返回瞬时 WatchCorePageIntent

F411UiAdapter
-> 重新读取 PageState
-> 重新读取 Snapshot
-> 把权威状态同步给 Lite View

watch_lite_view_apply_page_intent()
-> 隐藏四卡 panel
-> 显示详情 panel
-> 重建详情页 focus group
```

这一节先不要记函数全集，只抓住一句：

> View 报告“哪张卡被激活”，Core 决定“现在应该在哪一页”，Adapter 负责把两边接起来。

### 4.3 第一段：View 有没有决定页面

只读两个位置：

1. `watch_lite_view.c`
   - `on_card_clicked()`
2. `f411_ui_adapter.c`
   - `f411_ui_adapter_init()` 中的 `s_callbacks`
   - `on_activate_health_card()`

先追这三个值：

- `feature`
- `s_callbacks`
- `activate_health_card`

我的推演：

> `on_card_clicked()` 得到了什么输入，又输出了什么？  
> 它有没有调用“显示详情页”的 LVGL 代码？

待填写：


> `s_callbacks` 是谁提供给 Lite View 的？  
> 为什么 Lite View 不直接 `#include` Adapter 并调用固定函数？

待填写：


> `on_activate_health_card()` 看起来只转调一次，它的价值是什么？

待填写：


### 4.4 第二段：为什么还要创建 typed `UiEvent`

只读：

1. `f411_ui_adapter.c`
   - `f411_ui_adapter_activate_health_card()`
   - `f411_ui_adapter_dispatch_event()`
2. `watch_core.h`
   - `WatchCoreUiEventType`
   - `WatchCoreUiEvent`
3. `watch_core.c`
   - `watch_core_make_health_card_clicked_event()`

我的推演：

> 此时已经有 `feature`，为什么不直接调用 `watch_core` 的“打开详情页”函数，而要先形成 `WatchCoreUiEvent`？

待填写：


> `WatchCoreUiEvent` 相比 LVGL 的 `LV_EVENT_CLICKED`，增加了什么信息，又丢掉了什么平台细节？

待填写：


> 如果未来 PC 不使用 F411 的 encoder、group 和 Lite View，它仍然能产生同一种 `WatchCoreUiEvent` 吗？为什么？

待填写：


### 4.5 第三段：页面决定第一次出现在哪里

只读：

1. `watch_core.c`
   - `watch_core_push_event()`
   - `watch_core_process_pending_events()`
   - `watch_core_process_next_event()`
2. 只追：
   - `event.type`
   - `event.feature`
   - `core->current_page`
   - 返回的 `WatchCorePageIntent`

请以 SpO2 为例写时间线：

```text
push 前：
current_page = ?

入队事件：
event.type = ?
event.feature = ?

process_next_event 后：
current_page.type = ?
current_page.feature = ?
返回 intent.type = ?
返回 intent.feature = ?
```

待填写：


然后回答：

> 第一个真正写出“当前进入健康详情页”的模块是谁？  
> View、Adapter 和 Core 中，谁拥有导航决定权？

待填写：


### 4.6 本轮先停在这里

暂时不要继续阅读 `f411_ui_adapter_sync_view()` 和 `watch_lite_view_apply_page_intent()`。

完成 4.3、4.4、4.5 后，我们下一步专门解决一个容易混淆的问题：

```text
PageIntent 是瞬时动作
PageState 是权威持久状态
为什么 Adapter 最后根据 PageState 同步 View
```

这样不会把“事件如何形成”和“状态如何投影到 View”混成一个大问题。

## 附录 A：第一次自由阅读现场

以下内容保留第一次阅读时的真实思考和质疑。它不是错误答案，也不要求现在一次解决。

### A.1 第一条链：表冠短按进入详情

只按以下顺序打开代码，不要先扩散阅读：

1. `try/my_watch_f411_v2.1/Core/Src/freertos.c`
   - `StartKeyTask()`
   - `StartDefaultTask()`

   freertos.c
   这里有四个任务都是在cubemx中配置生成的，我认为是历史遗留问题，当前仅仅使用了其中的StartKeyTask和StartDefaultTask
   我认为把这样的任务放在这个cubemx生成的文件中，不是很有利于维护
   
   StartKeyTask中做了while (!g_bsp_initialized)未初始化保护，
   g_bsp_initialized = 1;
   在StartDefaultTask中watch_bringup_init初始化之后被置1，我认为watch_bringup_init的命名有点意义不明，bringup是什么意思，这里后续应该扩展为全部底层bsp驱动的路由路口，里面映射到各部分bsp实际的初始化函数，我认为这里的初始化顺序之后可以注意一下，写一些注释

   StartKeyTask任务内，每10ms执行一次watch_bringup_scan_input_10ms，
   void watch_bringup_scan_input_10ms(void)
   {
      watch_input_service_scan_10ms();
   }
   这个函数在bringup文件内被二次封装，不明白二次封装的意义，
   实际调用的watch_input_service_scan_10ms完成的是三种按键语义的扫描，这个一个不输出的函数，或者说是一个循环扫描的任务，他应当修改了某些全局变量，作为标记，
   scan_key()这个函数完成了什么，其他的部分我有点看不懂，我只看到，如果满足某个条件，他会调用 
   input_put(short_event_for_key(key));
   也就是发送一个短按事件
   后续拓展应该可以增加一个其他的判断，调用
   input_put(long_event_for_key(key));也许可以发送长按事件
   input_put把按键事件写入fifo数组中，
   有点好奇scan_key是怎么做到长短按的分发和计时的

   总结StartKeyTask做的内容是，每10ms扫描一次，如果有按键事件，塞入按键fifo数组中（按键在哪里被取出，暂时不知道）

   StartDefaultTask
   调用MX_USB_DEVICE_Init();可能是cubemx生成的，可能是历史遗留问题
   watch_bringup_init();完成初始化
   watch_lcd_init屏幕初始化
   watch_lcd_backlight_on打开背光
   watch_lvgl_port_init，lvgl初始化，
   lv_disp_draw_buf_init(&s_draw_buf,
                          s_draw_buf_1,
                          s_draw_buf_2,
                          (uint32_t)WATCH_LCD_WIDTH * WATCH_LVGL_DRAW_BUF_LINES);
                          应该是实现了双缓存区
   lv_disp_drv_register应该是把回调函数注册进去
   watch_touch_hw_init初始化触摸屏，这个位置合适吗，为什么在这个位置直接调用带硬件操作的，触摸屏初始化
   判断在watch_lvgl_port_init中最重要的是
   watch_lvgl_flush，watch_lvgl_wait_cb，watch_lvgl_monitor，watch_lvgl_touch_read，watch_lvgl_encoder_read这几个回调接口
   watch_lvgl_touch_read函数直接调用了底层合理吗？
   watch_lvgl_encoder_read不知道是如何起作用的
   lvgl需要深入学习理解

   f411_ui_adapter_init做了什么暂时看不懂

   watch_input_service_init对按键和编码器硬件进行了初始化
   s_keys[i].stable_pressed = watch_key_hw_is_pressed((watch_key_id_t)i);这一行是检测上电是的按键状态作为默认电平吗？

   StartDefaultTask初始化后，每5ms执行一次watch_bringup_task，
   bringup层为什么存在，是为了解决什么问题，有点不懂

   watch_bringup_task做的是
   pump_input_events
   如果while (event != WATCH_INPUT_NONE);
   就获取按键编码器事件，不过我提出一点质疑，触摸屏也是输入事件，但是现在好像还没有语义转化，也就是按键编码器走的和触摸屏不是同一层转化
   watch_input_intent_from_event感觉是把这些事件转化出具体的语义，但是后续事件膨胀，页面变得，是不是会变得更加庞大，臃肿？
   publish_intent处理返回的intent
   watch_lvgl_port_feed_input_intent感觉又没做什么，只是对语义进行转化?这是这一层应该做的工作吗？


   watch_lvgl_port_task好像掌管页面，但是当前只有两类，健康四卡页，和健康页详情，好像还比较简陋，具体的刷新怎么做到了，我还不明原因

   f411_ui_adapter_task在做什么也不清楚

2. `try/my_watch_f411_v2.1/user/services/input/watch_input_service.c`
   - `scan_key()`
   - `watch_input_service_scan_10ms()`
   - `watch_input_service_get_event()`

   关于这部分内容的阅读，我好像在之前已经做过了，
   但是我认为函数不应该是这样单文件阅读的，函数是一条线，是一条调用链，我应该链式阅读，我关心的是他什么时候被调用


3. `try/my_watch_f411_v2.1/user/app/input/watch_input_intent.c`
   - `watch_input_intent_from_event()`


4. `try/my_watch_f411_v2.1/user/app/watch_bringup.c`
   - `pump_input_events()`
   - `watch_bringup_task()`

5. `try/my_watch_f411_v2.1/user/ui/lvgl_port/watch_lvgl_port.c`
   - `watch_lvgl_port_feed_input_intent()`
   - encoder `read_cb`
   - `watch_lvgl_port_set_group()`

   
6. `try/my_watch_f411_v2.1/user/app/lvgl_demo/watch_lite_view.c`
   - `rebuild_focus_group()`
   - `on_card_clicked()`
7. `try/my_watch_f411_v2.1/user/app/f411_ui_adapter.c`
   - `f411_ui_adapter_activate_health_card()`
   - `f411_ui_adapter_dispatch_event()`
   - `f411_ui_adapter_sync_view()`
8. `watch_core/src/watch_core.c`
   - `watch_core_push_event()`
   - `watch_core_process_pending_events()`
   - `watch_core_process_next_event()`
9. 回到 `watch_lite_view.c`
   - `watch_lite_view_apply_page_intent()`
   - `watch_lite_view_apply_snapshot()`

### A.2 第二条链：左边缘右滑返回

按以下顺序阅读：

1. `watch_lvgl_port.c`
   - touch `read_cb`
   - `s_touch_swipe_back_candidate`
   - `s_touch_swipe_back_progress`
   - `s_touch_swipe_back_ready`
   - `watch_lvgl_port_take_left_edge_swipe_back()`
2. `f411_ui_adapter.c`
   - `f411_ui_adapter_task()`
   - `f411_ui_adapter_back()`
   - `f411_ui_adapter_dispatch_event()`
3. `watch_core.c`
   - `watch_core_make_back_event()`
   - `watch_core_process_next_event()`
4. `watch_lite_view.c`
   - `watch_lite_view_update_swipe_hint()`
   - `watch_lite_view_apply_page_intent()`

## 附录 B：暂停使用的整链表格

这组表格要求在建立整体画面前同时判断层次、状态和跨平台边界，不符合本轮实际学习节奏。

暂时不要填写。完成认知结 3、4、5 后，再决定是否需要把它改成简短的整链复盘。

### B.1 表冠短按链路表

请亲自填写，不要只写文件名。

| 层 | 具体函数 | 输入 | 输出 | 本层持有的状态 | 本层为什么存在 | 换到 PC/未来平台后保留还是替换 |
|---|---|---|---|---|---|---|
| FreeRTOS 调度 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| Input Service | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| Input Intent | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| Bring-up 装配 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| LVGL Port / encoder | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| Lite View callback | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| F411UiAdapter | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| watch_core | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| Lite View apply | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |

用自己的话写出完整链路：

> 待填写

### B.2 左边缘右滑链路表

| 层 | 具体函数/状态 | 输入 | 输出 | 本层持有的状态 | 本层为什么存在 | 哪部分属于平台特有实现 |
|---|---|---|---|---|---|---|
| Touch HW / Port | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| 手势进度显示 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| F411UiAdapter task | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| typed Back event | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| watch_core | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |
| Lite View apply | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 | 待填写 |

用自己的话写出完整链路：

> 待填写

### B.3 原必答判断题

1. 表冠短按为什么不是在 `watch_input_intent_from_event()` 中直接生成健康卡点击 `UiEvent`？

> 待填写

2. 表冠旋转为什么可以停留在 LVGL encoder/focus 语义，而不进入 `watch_core`？

> 待填写

3. 左边缘右滑的距离阈值为什么属于 F411 Platform Port，而 Back 语义属于共享合同？

> 待填写

4. `PageIntent` 和最终 `PageState` 在这两条链中分别起什么作用？

> 待填写

5. 如果 `watch_core_push_event()` 失败，Adapter 为什么不能继续 drain 或切 View？

> 待填写

6. 未来接入 Power 后，熄屏态表冠门控应插在哪个函数调用之前？请写出具体调用点和理由。

> 待填写

7. 为什么 Power 门控不能放在 `watch_lite_view.c`？

> 待填写

## 附录 C：LEARN-A 最终自我验收

在不打开架构文档的情况下，确认自己可以回答：

- [ ] 我能从 `StartKeyTask()` 讲到详情面板显示。
- [ ] 我能从 touch sample 讲到 Back 后首页恢复。
- [ ] 我能指出每一层拥有的状态。
- [ ] 我能区分“当前平台实现”和“跨平台共享语义”。
- [ ] 我能指出未来 Power 门控位置。
- [ ] 我能解释为什么当前旧 Wake 键不属于产品合同。

完成首次作答后，停止修改其它卡片，把本文件交给 Agent 审查。

## 附录 D：后续卡片占位

`LEARN-B`、`LEARN-C`、`LEARN-D` 的具体作答区域将在前一卡验收后开启。本轮不要提前填写，也不要提前新增追踪程序。
