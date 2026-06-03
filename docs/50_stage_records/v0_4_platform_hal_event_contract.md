# v0.4 Platform / HAL 事件契约收口

日期：2026-05-22

## 目的

本文是 v0.4 的第三个小闭环：对齐当前 PC 模拟器 HAL 事件模型、T-Watch S3 Plus 参考验证结果和后续 service 边界。

本轮只做契约收口，不改代码，不新增页面，不恢复硬件实验。

本轮回答：

- 当前 `HAL.h` 中每类 sample 表达的语义层级是什么。
- 模拟器事件和 T-Watch 已验证能力分别能映射到哪些 app intent。
- 哪些能力已经可作为下一轮 service 输入，哪些仍只是 debug 替代。
- 第一批 `Battery / PowerStatus service` 应消费哪些输入、输出哪些模型。

## 当前事件链路

当前平台事件链路是：

```text
SimulatorDevice / future board platform
  -> hal::Event
  -> Application::handle_hal_event
  -> InputIntentRouter / DataCenter
  -> AppStateMachine / Page
```

契约原则：

- HAL 表达“平台侧发生了什么”，不表达“页面应该怎么跳”。
- `InputIntentRouter` 负责把输入样本翻译成 app intent。
- `DataCenter` 暂时保存 UI 可订阅模型。
- `AppStateMachine` 只消费高层 intent 或必要系统事件，不直接解释 PMU、触摸控制器或 IMU 细节。
- `DebugSample` 只能用于模拟器和开发注入，不应成为真实硬件接口。

## HAL 事件职责表

| HAL 类型 | 当前表达 | 模拟器来源 | T-Watch 已验证能力 | 契约判断 |
| --- | --- | --- | --- | --- |
| `TimeSample` | 当前系统时间快照。 | PC 本地时间，每秒发布。 | bring-up 未专项验证 RTC / 手机同步。 | 可保留为最小时间样本；暂不抽 `Time service`。 |
| `BatterySample` | 电池是否存在、是否充电、是否外部供电、电量百分比和电压。 | 模拟电池循环变化。 | AXP2101 可读 USB、充电、放电、电池电压、电量百分比、VBUS / SYS 电压。 | 已足够支持第一轮 `Battery / PowerStatus service`；VBUS、SYS、充电阶段暂不扩展。 |
| `ButtonSample` | 主按键短按 / 长按。 | `P` / `5` 键短按或长按。 | PMU / PEK 侧键可用于运行态 screen off/on 和 deep sleep 唤醒。 | 可作为无表冠硬件的主物理按键入口，但当前 app 语义仍带 debug 味道。 |
| `CrownSample` | 表冠按下、顺时针 / 逆时针旋转和 detent 数。 | `Enter` / `Space` 模拟按下，`E` / `Q` 或方向键模拟旋转。 | T-Watch 没有真实旋转表冠；PEK 侧键只能替代 press / wake。 | `Press` 可由按键替代；`RotateCW/CCW` 仍是模拟器能力，不算 T-Watch 已验证。 |
| `TouchSample` | 触摸活动、滚动、边缘进度、边缘提交、水平滑动提交。 | SDL 鼠标按下、拖动和释放，经 `SimulatorDevice` 识别成手势级事件。 | FT6336U 经 LVGL pointer 可读 press / move / release、坐标方向和基础边缘输入。 | 当前更像“平台手势事件”，不是裸触摸点；T-Watch 尚未验证完整手势识别。 |
| `MotionSample` | 三轴加速度快照。 | 当前模拟器没有常规运动样本发布；raise-to-wake 使用 debug action。 | BMA423 可读基础 X/Y/Z 和姿态变化。 | 可承载基础 IMU 数据；真实 raise-to-wake 中断和算法未验证。 |
| `DebugSample` | 开发注入：通知、电量低、翻腕亮屏、放下熄屏、遮盖熄屏。 | 键盘热键 `N`、`B`、`R`、`F`、`C`。 | 无真实硬件对应。 | 必须标记为 simulator-only，不进入正式硬件平台接口。 |

## 输入到 intent 的映射

| 平台样本 | 当前 app intent | 模拟器是否已跑通 | T-Watch 是否已验证 | 说明 |
| --- | --- | --- | --- | --- |
| `ButtonSample::ShortPress` | `DebugToggleScreenOff` | 是 | 部分。PEK 短按可切 screen off/on。 | 名称仍是 debug；下一轮不急着改名，但后续硬件平台不应把它理解为最终产品语义。 |
| `ButtonSample::LongPress` | `DebugOpenPowerMenu` | 是 | 部分。PEK 长按已用于进入 deep sleep 实验，不等同于 power menu。 | 真实产品里长按可进入 power menu 或 sleep，需要后续单独收口。 |
| `CrownSample::Press` | `CrownPress` | 是 | 可由 PEK 侧键替代验证。 | T-Watch 没有表冠，但可以验证“主确认 / 唤醒 / 回表盘”路径。 |
| `CrownSample::RotateCW/CCW` | `CrownRotateCW/CCW` | 是 | 否 | 仍是模拟器交互增强，不应用 T-Watch 结论支撑。 |
| `TouchSample::TouchActivity` | `TouchActivity` / tap-to-wake 入口 | 是 | 部分。基础触摸点已验证。 | T-Watch 尚未验证低功耗触摸唤醒；运行态触摸恢复屏幕已验证。 |
| `TouchSample::EdgeBackCommit` | `NavigateBack` 或 `HomeEdgeBackRight` | 是 | 否 | 需要真实手势识别闭环后才算硬件可用。 |
| `TouchSample::BottomEdgeCommit` | `OpenQuickSettings` | 是 | 否 | 当前只证明模拟器壳层逻辑可用。 |
| `TouchSample::TopEdgeCommit` | `OpenNotifications` | 是 | 否 | 当前只证明模拟器壳层逻辑可用。 |
| `TouchSample::HorizontalSwipeLeft/RightCommit` | `HomeSwipeLeft` / `HomeEdgeBackRight` | 是 | 否 | T-Watch 只验证基础 pointer，未验证完整水平手势阈值。 |
| `DebugSample::SimRaiseToWake` | `SimRaiseToWake` | 是 | 否 | 真实 BMA423 raise-to-wake 中断未验证，不能当作硬件能力。 |
| `DebugSample::SimRaiseDismiss` | `SimRaiseDismiss` | 是 | 否 | 当前只是测试 raise session 的模拟动作。 |
| `DebugSample::SimCoverSleep` | `SimCoverSleep` | 是 | 否 | 真实遮盖检测来源未定义。 |

## 关键样本契约

### `BatterySample`

当前字段：

- `present`
- `charging`
- `external_power`
- `percent`
- `millivolts`

T-Watch 对照：

- `present` 可由电池/PMU 状态推断，但 bring-up 未单独做无电池实验。
- `charging` 可由 AXP2101 充电状态映射。
- `external_power` 可由 USB / VBUS 状态映射。
- `percent` 可由 AXP2101 百分比读数映射。
- `millivolts` 可由电池电压映射。

暂不扩展：

- VBUS 电压。
- SYS 电压。
- 充电阶段。
- PMU IRQ。
- 电源轨开关状态。

判断：下一轮 `Battery / PowerStatus service` 可以先消费现有 `BatterySample`。如果 service 只做低电量、充电状态、外部供电状态和 UI 模型解释，现有字段足够。

### `TouchSample`

当前 `TouchSample` 的名字容易让人误会。它不是裸触摸点流，而是已经被平台层识别过的“手势级触摸事件”：

- `TouchActivity` 表示一次触摸活动开始。
- `ScrollDrag` / `ScrollFlick` / `ScrollRelease` 表示滚动手势。
- `TopEdgeProgress` / `BottomEdgeProgress` / `EdgeBackProgress` 表示边缘预览进度。
- `...Commit` / `...Cancel` 表示手势完成或取消。

T-Watch 对照：

- 已验证 FT6336U 经 LVGL pointer 能产生 press / move / release。
- 已验证坐标方向与视觉方向一致。
- 已观察基础边缘输入路径。
- 未验证完整边缘手势阈值、横向主页滑动、滚动惯性或手势冲突。
- 未验证触摸控制器作为 deep sleep 唤醒源。

判断：当前不改类型名，但文档中必须把它视为“gesture sample”。未来真实平台可以先在 board platform 内用 LVGL pointer 或自有识别器生成同类 `TouchSample`，不应让页面直接读裸坐标。

### `CrownSample` 与按键替代

当前模拟器提供完整表冠替代：

- `Press`
- `RotateCW`
- `RotateCCW`

T-Watch 对照：

- PEK 侧键可以替代 `Press` 类语义。
- PEK 侧键不能替代旋转。
- 当前没有 detent 计数来源。

判断：

- `CrownPress` 可以继续作为“主确认 / 回主页 / 唤醒”这类高层输入。
- `RotateCW/CCW` 仍用于模拟器交互建模，不能作为 T-Watch 已验证输入。
- 如果后续选择无表冠硬件，应明确哪些 UI 依赖旋转，哪些必须有触摸替代路径。

### `MotionSample`

当前字段只表达基础三轴加速度：

- `valid`
- `x_mg`
- `y_mg`
- `z_mg`

T-Watch 对照：

- BMA423 基础 X/Y/Z 可读。
- 平放、倒扣、竖立、侧边贴地时主导轴变化明显。
- 官方 `direction()` 有可观察结果。
- 未验证 BMA 中断。
- 未验证低功耗姿态检测。
- 未实现真实 `RaiseToWakeDetected`。

判断：`MotionSample` 可作为后续姿态 service 或 raise-to-wake 原型输入，但下一轮不应先做它。当前 `SimRaiseToWake` 只能证明状态机路径，不证明真实 IMU 唤醒路径。

### screen off / wake

当前 `HAL.h` 没有独立的 `ScreenOffSample` 或 `WakeSample`。这并不是当前阶段的缺陷。

当前模拟器中：

- screen off 是 `AppStateMachine` 的系统状态。
- 按键、表冠 press、触摸 activity、通知和 debug raise action 会触发不同 wake 路径。
- auto screen off、keep screen on 和 notification wake timer 是状态机策略。

T-Watch 对照：

- 运行态 screen off/on 已通过 PEK 侧键和触摸恢复验证。
- timer deep sleep 可进入并自动重启式恢复。
- PMU / PEK 侧键 deep sleep 唤醒可用。
- 触摸、BMA423、RTC 作为 deep sleep 唤醒源未验证。
- 真实低功耗电流未验证。

判断：

- v0.4 暂不新增 wake event 类型。
- 运行态 screen off / wake 继续由输入 intent 和状态机处理。
- deep sleep 后的重启式恢复属于未来平台生命周期契约，不应混入当前 `InputIntentRouter`。

## 不进入真实硬件接口的内容

以下能力目前只属于模拟器 / debug：

- `InjectMessageNotification`
- `InjectBatteryLowNotification`
- `SimRaiseToWake`
- `SimRaiseDismiss`
- `SimCoverSleep`
- `CrownSample::RotateCW/CCW` 在 T-Watch 参考板上的验证结论
- 完整边缘手势阈值在 T-Watch 参考板上的验证结论

这些内容仍然有价值，因为它们能验证状态机、页面恢复、壳层导航和通知 wake preview。但不能用它们证明真实平台已经具备对应外设能力。

## 对下一轮 Battery / PowerStatus service 的约束

下一轮只建议接 `BatterySample`，不要同时接触摸、IMU、通知或深睡生命周期。

建议输入：

- `hal::BatterySample`

建议 service 输出：

- 对 UI 暴露的电池 / 电源状态模型。
- 低电量事件或通知请求。
- 充电、外部供电和电压解释后的稳定字段。

仍留在其他层：

- Platform / HAL：读取或模拟原始电池样本。
- `DataCenter`：保存 UI 可订阅模型。
- UI 页面：只渲染模型，不解释 PMU。
- `AppStateMachine`：只消费必要的系统事件，不读 PMU 细节。
- Debug 注入：继续可用于测试，但不等同于真实电池 service。

## 当前结论

现有 `HAL.h` 不需要在第三轮扩展类型。它足够支撑 v0.4 的下一步：先抽出最小 `Battery / PowerStatus service` 边界。

真正需要记住的是：当前 HAL 事件中有三种不同层级混在一起，但边界已经可以解释清楚。

- `BatterySample`、`TimeSample`、`MotionSample` 更接近平台数据样本。
- `TouchSample`、`CrownSample`、`ButtonSample` 更接近平台输入事件，其中 `TouchSample` 已经是手势级。
- `DebugSample` 是开发注入，不属于真实硬件契约。

只要下一轮不把 debug 替代误认为真实能力，不把页面变成 PMU 解释器，就可以继续小步进入第一个 service 闭环。
