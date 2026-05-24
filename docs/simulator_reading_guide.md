# Simulator Code Reading Guide

这份文档面向“有 C/C++ 基础，但对当前这套 C++/LVGL 项目骨架还不熟”的阅读者。

目标不是一次讲完所有细节，而是帮你先建立一张稳定的地图：

- 这个模拟器现在想解决什么问题
- 程序从哪里启动
- 输入、状态、页面分别由谁负责
- 读代码时哪些文件是当前主线
- 看到某些 C++ 写法时，应该先怎么理解

## 1. 先建立总印象

这个模拟器已经不是“一个把 LVGL 画出来的小 demo”了。

它正在往这样的结构演化：

```text
Platform/HAL -> Application -> DataCenter/EventBus -> AppStateMachine -> PageManager -> Page/UI
                                      \----------------------------------------------> 页面数据刷新
```

你可以先把它理解成：

- `Platform/HAL` 负责把“PC 上的键盘、鼠标”伪装成“手表输入”
- `Application` 负责接线和翻译
- `DataCenter/EventBus` 负责传数据、发事件
- `AppStateMachine` 负责“系统现在该干什么”
- `PageManager` 负责“显示哪一页、怎么切页”
- `Page` 负责“这一页长什么样、点了按钮想去哪里”

这套思路和很多裸机/RTOS 项目里常见的“哪里都能直接互调”很不一样。它更强调：

- 输入先翻译成语义
- 页面不直接控制全局
- 导航和显示由专门模块负责

## 2. 当前主线先读哪些文件

建议按下面顺序读，而不是一上来就把 `src` 全部扫完。

### 第一轮：只看程序怎么跑起来

1. `sim/lv_port_pc_vscode/src/main.cpp`
2. `sim/lv_port_pc_vscode/src/App/Application.h`
3. `sim/lv_port_pc_vscode/src/App/Application.cpp`

这一轮只回答 3 个问题：

- 程序从哪里启动
- 谁创建了核心对象
- 主循环每次在驱动什么

### 第二轮：看事件和数据怎么流动

1. `sim/lv_port_pc_vscode/src/hal/HAL.h`
2. `sim/lv_port_pc_vscode/src/App/Input/InputIntentRouter.h`
3. `sim/lv_port_pc_vscode/src/App/Input/InputIntentRouter.cpp`
4. `sim/lv_port_pc_vscode/src/App/Common/AppEvents.h`
5. `sim/lv_port_pc_vscode/src/App/Common/DataCenter.h`
6. `sim/lv_port_pc_vscode/src/App/Common/DataCenter.cpp`
7. `sim/lv_port_pc_vscode/src/App/Common/EventBus.h`
8. `sim/lv_port_pc_vscode/src/App/Common/EventBus.cpp`

这一轮回答：

- HAL 原始事件长什么样
- 应用层事件长什么样
- 事件是同步广播还是排队处理
- 最近一次时间、电池数据存在哪里

### 第三轮：看“系统控制”和“页面显示”怎么分工

1. `sim/lv_port_pc_vscode/src/App/State/AppStateMachine.h`
2. `sim/lv_port_pc_vscode/src/App/State/AppStateMachine.cpp`
3. `sim/lv_port_pc_vscode/src/App/UI/PageBase.h`
4. `sim/lv_port_pc_vscode/src/App/UI/PageBase.cpp`
5. `sim/lv_port_pc_vscode/src/App/UI/PageManager.h`
6. `sim/lv_port_pc_vscode/src/App/UI/PageManager.cpp`

这一轮回答：

- 谁决定“回首页 / 熄屏 / 开电源菜单”
- 谁决定“push / pop / 临时页”
- 页面怎样发出“我想导航”的意图

### 第四轮：最后再看具体页面

1. `sim/lv_port_pc_vscode/src/App/UI/Pages/ShellPages.h`
2. `sim/lv_port_pc_vscode/src/App/UI/Pages/ShellPages.cpp`
3. `sim/lv_port_pc_vscode/src/App/UI/Pages/CommonPages.h`
4. `sim/lv_port_pc_vscode/src/App/UI/Pages/CommonPages.cpp`
5. `sim/lv_port_pc_vscode/src/App/UI/Pages/ToolPages.h`
6. `sim/lv_port_pc_vscode/src/App/UI/Pages/ToolPages.cpp`

这时候再看页面细节，你就不会被 LVGL API 淹没。

## 3. 运行入口到底做了什么

`main.cpp` 很短，但它定义了整个程序的节奏：

1. `lv_init()` 初始化 LVGL
2. `create_simulator_device()` 创建模拟器设备
3. `Application application(...)` 组装应用
4. `application.start()` 启动应用
5. 主循环中反复执行：
   - `application.tick(elapsed_ms)`
   - `lv_timer_handler()`
   - `sleep`

如果你有 MCU 背景，可以把它理解成：

- `application.tick()` 更像“系统逻辑推进”
- `lv_timer_handler()` 更像“GUI 框架自己的任务处理”

## 4. Application 是“组合根”

`Application` 现在最重要的职责不是做业务，而是“把系统拼起来”。

它主要做三件事：

1. 注册页面工厂
2. 接收 HAL 事件
3. 把 HAL 事件翻译成应用层事件或数据模型

你读 `Application.cpp` 时，可以重点抓这两个函数：

- `register_pages()`
- `handle_hal_event()`

`handle_hal_event()` 的意思非常接近：

- 如果 HAL 告诉我时间变了，就发布 `TimeModel`
- 如果 HAL 告诉我电池变了，就发布 `BatteryModel`
- 如果 HAL 告诉我按钮/表冠/触摸变了，就先交给 `InputIntentRouter`

也就是说，`Application` 不是最终决策者，它更像“翻译员 + 接线员”。

## 5. HAL 和 InputIntentRouter 的分工

这是理解整套骨架的关键。

### HAL 只表达“硬件侧发生了什么”

在 `HAL.h` 里，你会看到：

- `ButtonSample`
- `CrownSample`
- `TouchSample`
- `TimeSample`
- `BatterySample`
- `MotionSample`

这些都还是“底层语义”。

例如：

- 按键短按/长按
- 表冠顺时针/逆时针
- 边缘返回拖动进度
- 上滑、下滑、滚动、甩动

### InputIntentRouter 负责把原始输入翻译成更高层的意图

它把 HAL 事件翻译成 `InputCommand`，例如：

- `DebugToggleScreenOff`
- `DebugOpenPowerMenu`
- `CrownPress`
- `CrownRotateCW`
- `NavigateBack`
- `OpenLauncher`
- `OpenQuickSettings`

特别要注意这一点：

同一个触摸手势，在不同页面上下文里，语义可能不同。

例如左边缘右滑：

- 在主页上下文里，可能代表主页环流转
- 在普通页面里，代表返回

这就是为什么项目专门做了 `InputIntentRouter`，而不是把所有判断直接堆进页面或状态机。

## 6. DataCenter 和 EventBus 怎么理解

如果你以前更习惯“全局变量 + 直接函数调用”，这里会稍微陌生。

### DataCenter

你可以先把 `DataCenter` 理解成：

- “最近一次共享数据”的存放点
- “对外发布事件”的统一入口

它现在保存：

- `last_time_`
- `last_battery_`
- `last_motion_`

并提供：

- `publish_time()`
- `publish_battery()`
- `publish_motion()`
- `publish_navigation()`
- `publish_input()`

### EventBus

`EventBus` 是同步发布订阅。

意思是：

- 有人 `publish(event)` 时
- 所有订阅这个事件的人会立刻在当前线程被调用

它不是 RTOS 消息队列，也不是异步任务系统。

这点非常重要，因为它决定了：

- 当前代码比较容易读
- 但未来上多任务后，不能直接照搬这套同步模型

## 7. 状态机和页面管理器分别管什么

### AppStateMachine 管“系统行为”

它维护的不是页面控件细节，而是系统级状态，例如：

- `Booting`
- `Running`
- `ScreenOff`
- `PoweredOff`

以及临时壳层：

- `Launcher`
- `QuickSettings`
- `PowerMenu`

你可以把它理解成：

- 它决定“现在能不能进某个页面”
- 它决定“短按/长按后系统应该怎么反应”
- 它决定“是回首页、关临时页，还是熄屏”

### PageManager 管“页面切换”

`PageManager` 只关心：

- 这个 `PageId` 对应哪个页面对象
- 页面是否已经创建过
- 当前栈顶是谁
- 当前有没有临时页
- 用什么动画切页

最重要的区别是：

- `AppStateMachine` 决定“为什么切页”
- `PageManager` 决定“如何切页”

这个边界划分是这套骨架最值得保留的地方之一。

## 8. 页面不是直接操作全局，而是发导航意图

`PageBase` 里有一个很关键的辅助函数：

- `request_navigation(const NavigationCommand& command)`

页面点击按钮时，不会直接调用 `PageManager::push()`。

它做的是：

1. 页面发出 `NavigationCommand`
2. `DataCenter` 发布 `NavigationRequested`
3. `AppStateMachine` 收到后决定是否执行
4. 最终再调用 `PageManager`

这和很多“按钮回调里直接跳页”的写法相比，多了一层，但它的好处是：

- 页面不用知道全局导航细节
- 业务意图和显示实现分开
- 以后更容易替换规则

## 9. 一个完整例子：模拟器调试熄屏键是怎么走的

以键盘 `P` 模拟调试熄屏键为例，链路大致是：

1. `SimulatorDevice::process_button()` 识别短按/长按
2. 发出 `hal::EventKind::ButtonChanged`
3. `Application::handle_hal_event()` 收到事件
4. `InputIntentRouter` 翻译成 `InputAction::DebugToggleScreenOff`
5. `DataCenter::publish_input()` 广播事件
6. `AppStateMachine::handle_input()` 决定：
   - 如果在熄屏，就亮回主页
   - 如果在主页，就进入熄屏
   - 如果在其他页面，就先回主页

这条链路只是 PC 模拟器调试入口，不代表真实手表一定存在独立主按键。真实产品语义中，熄屏态亮屏应优先由表冠按下、翻腕亮屏等 `DisplayPolicyModel` 允许的唤醒来源触发。

当前 `5` / 小键盘 `5` 仍保留为历史兼容别名，但后续阅读和手测应优先把 `P` 理解为模拟器调试键。

## 10. 一个完整例子：页面点击按钮是怎么跳转的

以页面上的一个按钮为例，链路大致是：

1. LVGL 回调触发
2. 页面调用 `request_navigation(...)`
3. `DataCenter` 发布 `NavigationRequested`
4. `AppStateMachine::handle_navigation()` 判断是否合法
5. `PageManager` 执行 `push/pop/set_root/show_temporary`

所以页面更像“提出请求”，不是“直接发号施令”。

## 11. 当前代码里哪些“看起来奇怪”，但其实是正常的

### 1. 有不少 `std::optional`

这相当于“这个值可能有，也可能没有”。

比如：

- 当前时间可能还没拿到
- 当前电池信息可能还没发布
- 当前可见页可能还不存在

如果你以前常写 C，可以把它先类比成：

- 一个值
- 加一个 `valid` / `has_value` 标志

只是这里标准库帮你封装好了。

### 2. 有很多 `std::variant`

这相当于“一个变量在同一时刻只能装多种类型中的一种”。

例如一个事件的 payload 可能是：

- 时间
- 电池
- 输入命令
- 导航命令

你可以把它粗略类比成：

- `union` + 类型标签

只是 C++ 版本更安全。

### 3. 有很多 lambda

例如：

```cpp
[this](const Event& event) { handle_event(event); }
```

可以先把它理解成“临时定义了一个小回调函数”。

`[this]` 的意思是：

- 这个小函数里要访问当前对象

### 4. `std::unique_ptr`

这表示“独占所有权的指针”，大体可以理解为：

- 这个对象只有一个主人
- 主人销毁时，对象会自动释放

在这个项目里它常用来表达：

- `Application` 独占 `Device`
- `PageManager` 独占各个页面实例

### 5. `override`

例如：

```cpp
lv_obj_t* build() override;
```

意思是：

- 这个函数是在重写父类的虚函数

如果父类没有这个函数，编译器会报错。
这能防止手误写错函数签名。

## 12. 当前目录里有“残留资产”，不要一开始就把它当 bug

当前 `CMakeLists.txt` 实际编译的是：

- `CommonPages.cpp`
- `SettingsPages.cpp`
- `ShellPages.cpp`

当前主线应以 `CMakeLists.txt` 中真正参与编译的文件为准。

补充说明：

- 过去这里确实保留过 `WatchfacePage.cpp`、`QuickStatusPage.cpp` 这类未参与构建的旧文件
- 它们已在 2026-05-24 的孤儿源码清理轮次中删除

这和项目文档“不要因为它有残留就直接清理或重写”的原则是一致的。

所以现阶段读代码时，最稳妥的策略不是“先清干净”，而是：

1. 先确认哪些文件真的参与构建
2. 先讲清当前主线
3. 再决定哪些残留值得整理

## 13. 你可以先忽略哪些内容

第一次阅读时，不建议深挖这些部分：

- LVGL 大量样式 API 的每一个参数
- 动画资源 `MonicaAssets`
- 具体字体、颜色、布局微调
- `reinterpret_cast` 这类 UI 用户数据写法的细枝末节

第一次更应该抓住：

- 模块边界
- 事件流
- 导航流
- 哪些模块做决策，哪些模块只执行

## 14. 一个适合你的阅读策略

如果你此前主要是 C / C++ 语法级经验，我建议你这样读：

### 第一步

只看函数名和职责，不抠每一行。

比如先问自己：

- 这个类“拥有”什么
- 这个类“订阅”什么
- 这个类“发布”什么
- 这个类“决策”什么

### 第二步

顺着一条具体链路读。

例如只追：

- 模拟器调试熄屏键链路
- 表冠按下链路
- 左边缘右滑链路
- 点击 Launcher 某个 item 的链路

不要试图一次把所有页面都读懂。

### 第三步

最后再回头理解 C++ 语法细节。

因为很多时候不是“语法难”，而是“没有先知道这段代码在整个系统里扮演什么角色”。

## 15. 当前阶段的结论

如果你现在觉得“这个项目看起来有点杂”，这个判断不算错，但更准确的说法是：

- 它不是最终整洁架构
- 但它已经有一条很明确的事件驱动主线
- 这条主线值得保留并继续提炼

因此现在最重要的不是重写，而是先把它读懂：

- 哪些是主线
- 哪些是残留
- 哪些职责已经分出来了
- 哪些职责还堆在一起

理解到这里，你后面再看文档里的目标架构，就会更容易知道“为什么项目想往那个方向演化”。
