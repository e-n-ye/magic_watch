# F411 Future Watch Architecture Blueprint

日期：2026-06-04

本文是 Magic Watch 架构思想向 STM32F411 手表项目迁移前的蓝图文档。

它不是当前模拟器实现说明，也不是 T-Watch S3 Plus 阶段 9 的延续。它的目的更朴素：先把未来项目想清楚，让后续 C 代码可以沿着一个自己能掌控的骨架慢慢长出来。

配套可视化页面：

- [f411_future_watch_architecture_uml.html](/D:/MY_Desk/watch/magic_watch/docs/10_architecture/f411_future_watch_architecture_uml.html)

## 核心目标

未来 F411 手表项目要训练的是：

- 用 C 语言亲手搭出清楚的系统骨架。
- 新增功能时知道应该改哪一层，而不是到处找入口。
- UI 不直接碰硬件，硬件不直接碰 UI。
- 传感器、电源、按键、显示、页面切换都能通过稳定边界组合起来。
- 先有骨架，再一点点填入真实驱动、真实数据和真实页面。

当前允许使用“未来接口已经存在”的想象魔法，但这些接口必须满足一个标准：以后可以用普通 C、清楚的结构体、固定大小消息和可测试函数逐步实现。

## 总体分层

固定主线：

```text
Board / Driver
-> Platform Port
-> Services
-> ModelStore (DataCenter-lite)
-> EventQueue / EventBus-lite
-> Coordinator (StateMachine-lite)
-> Screen / App Modules
-> UI Adapter
```

每层都要回答三件事：

- 它负责什么。
- 它不负责什么。
- 第一批代码可以怎么长出来。

## 分层职责

| 层 | 负责什么 | 不负责什么 | 第一批可以怎么长 |
| --- | --- | --- | --- |
| Board / Driver | 寄存器、GPIO、I2C/SPI/UART、EXTI、DMA、屏幕总线、传感器原始读写 | 不解释业务，不切页面，不直接改 UI | `drv_button.c`、`drv_display.c`、`drv_imu.c`、`drv_battery.c` |
| Platform Port | 把板级输入整理成平台样本；提供时间、日志、临界区、队列适配 | 不持有页面，不做复杂策略 | `platform_time_ms()`、`platform_poll()`、`platform_post_isr_event()` |
| Services | 把原始样本翻译成稳定应用模型；去抖、限频、阈值判断 | 不创建 UI，不直接切页 | `power_service`、`input_service`、`motion_service` |
| ModelStore | 保存最后一份可信模型快照 | 不暴露可变裸引用，不做策略 | `model_store_get_battery()`、`model_store_set_steps()` |
| EventQueue / EventBus-lite | 传递固定大小领域事件，连接服务、协调器和 UI 刷新 | 不保存复杂对象，不使用动态分配 | 环形队列 + 固定事件枚举 + payload union |
| Coordinator | 消费事件和模型，做跨域决策，输出页面动作 | 不读寄存器，不创建 LVGL 对象，不直接操作驱动 | `coordinator_handle_event()` 返回 `ui_action_t` |
| Screen / App Modules | 表盘、启动器、通知、快捷设置、健康、设置等页面状态机 | 不访问 Driver/BSP/HAL | 每个页面一个 `screen_xxx.c`，只消费模型和 UI 事件 |
| UI Adapter | 把页面描述翻译成具体显示库调用 | 不决定业务，不读传感器 | 先可以是简化绘制函数，后续再接 LVGL/u8g2/自绘 |

## C-first 模块形态

默认模块不是 C++ class，而是 C 的 context + 函数接口：

```c
typedef struct {
    /* private-by-convention state */
} power_service_t;

void power_service_init(power_service_t *svc);
void power_service_handle_sample(power_service_t *svc,
                                 const battery_sample_t *sample,
                                 model_store_t *store,
                                 event_queue_t *events);
void power_service_tick(power_service_t *svc,
                        uint32_t now_ms,
                        model_store_t *store,
                        event_queue_t *events);
```

约束：

- init 明确初始化状态。
- handle 处理输入样本。
- tick 处理时间推进。
- publish 只发布固定大小事件。
- 不默认使用 heap。
- 不在事件里放字符串、vector、复杂对象所有权。
- 如果未来引入少量 C++，只能包住局部资源管理，不能改变主架构对 C 的可理解性。

## 从 Magic Watch 保留的思想

这些思想值得带走，但要换成 F411/C-first 形态：

| Magic Watch 思想 | F411 中的名字 | 保留原因 |
| --- | --- | --- |
| `InputIntentRouter` | `input_service` / `input_mapper` | 把按键、触摸、旋钮等原始输入变成语义输入 |
| `Service` | `*_service` | 把硬件样本变成应用模型，不让页面解释硬件 |
| `DataCenter` | `model_store` | 给 UI 和协调层一个稳定快照入口 |
| `EventBus` | `event_queue` / `event_bus_lite` | 用事件连接模块，但不引入复杂动态分发 |
| `AppStateMachine` | `coordinator` | 处理跨域冲突和页面动作 |
| `PageManager` | `screen_manager` | 管理当前页面、页面栈和临时界面 |
| 页面领域归属 | `screen_home`、`screen_settings` 等 | 避免所有 UI 回流到一个大文件 |

## 不照搬的东西

这些东西留在模拟器和参考板经验里，不直接搬进 F411：

- 不搬模拟器 LVGL 页面实现。
- 不搬 C++ 通用 `EventBus`。
- 不搬 `std::function`、`std::vector`、`std::string`。
- 不搬全量 `DataCenter`。
- 不绑定 T-Watch S3 Plus、ESP32-S3、AXP2101、BMA423。
- 不让 AI 自动堆页面细节，先让人能说清骨架。
- 不为了“像完整项目”提前生成一堆空模块。

## 关键数据流

### 输入流

```text
Button EXTI / Touch Poll / Crown Encoder
-> Driver
-> Platform input sample
-> input_service 去抖与语义映射
-> event_queue: INPUT_CLICK / INPUT_BACK / INPUT_SCROLL
-> coordinator
-> screen_manager 或当前 screen
-> ui_adapter 重绘
```

规则：

- ISR 只记录事实或投递轻量事件。
- 去抖不写在页面里。
- 页面只看语义输入，不看 GPIO。

### 传感器 / 电源流

```text
Battery / IMU / Heart sensor / RTC
-> Driver sample
-> service 过滤、限频、判断
-> model_store 更新快照
-> event_queue 发布模型变化事件
-> coordinator / screen 按需刷新
```

规则：

- 高频数据先降频或聚合。
- UI 不跟随每个原始样本刷新。
- 事件 payload 是小结构体或模型 ID，不传复杂对象。

### 页面流

```text
coordinator 产生 ui_action_t
-> screen_manager 切换 root / push / pop / overlay
-> screen_xxx enter/update/exit
-> ui_adapter 绘制
```

规则：

- 页面可以保存页面局部状态。
- 页面不拥有全局业务状态。
- screen_manager 不解释业务原因，只执行页面动作。

## 第一批骨架建议

不要一上来做完整手表。第一批只做 5 个模块，把数据流走通：

1. `platform_time`：提供 `now_ms`。
2. `event_queue`：固定数组环形队列。
3. `model_store`：只保存 battery + input debug 状态。
4. `input_service`：一个按键短按 / 长按语义。
5. `screen_manager`：Home 与 Debug 两个屏幕切换。

第一批验收不是“像手表”，而是：

- 能说清主循环每一拍做什么。
- 能从按键事件走到页面动作。
- 能从一个模拟 battery sample 写入 model_store 并触发 UI 刷新。
- 每个模块文件都能回答“我属于哪一层”。

## 未来生长顺序

```text
Batch A：C-first 最小运行骨架
Batch B：输入服务与屏幕管理
Batch C：电源 / 电池模型
Batch D：运动 / 步数模型
Batch E：真实显示 UI adapter
Batch F：低功耗与唤醒路径
Batch G：更多页面和功能
```

每个批次都必须小到能亲手解释，不用“AI 替我想好了”来蒙混过关。

## 判断一个新功能该放哪

| 新功能问题 | 应该先去哪里 |
| --- | --- |
| 是寄存器、总线、中断、DMA 吗 | Driver |
| 是把硬件事实整理成平台样本吗 | Platform Port |
| 是去抖、滤波、限频、阈值吗 | Service |
| 是保存当前事实吗 | ModelStore |
| 是通知别人发生了什么吗 | EventQueue |
| 是跨域策略或页面动作吗 | Coordinator |
| 是当前界面怎么显示吗 | Screen / App Module |
| 是具体画点、画字、刷屏吗 | UI Adapter |

如果一个函数同时想做三层的事，先停下来拆开。这个停顿本身就是掌控感。

## 和阶段 9 的关系

阶段 9 的 T-Watch Battery 切片仍有价值：它证明了“硬件样本 -> Service -> ModelStore -> Event -> observer”的思想可以下放到真实 MCU。

但 F411 蓝图不是阶段 9 的继续执行，不继承 T-Watch 的板级绑定，也不要求沿用 C++ 模拟器接口。F411 蓝图吸收的是职责边界，不是具体代码。

## 下一步卡片草案

后续如果要进入目标模式，建议先开一个很小的批次：

- `F411-SKEL-1`：在 F411 项目中建立空的 C-first 目录骨架和 README。
- `F411-SKEL-2`：实现 `event_queue` 固定数组队列。
- `F411-SKEL-3`：实现 `model_store` 最小快照。
- `F411-SKEL-4`：实现一个按键输入到 screen action 的闭环。

每张卡只做一层或一条细流，不允许一口气生成完整工程。
