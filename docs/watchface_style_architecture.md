# Watchface Style Architecture

本文档定义 Magic Watch 当前阶段的表盘风格边界、职责拆分和扩散表盘 v0 的接入方式。

## 核心结论

- 表盘风格切换是 `watchface-only` 能力。
- 当前不做全局主题系统，不把风格切换扩散到 Launcher、主页环、快捷设置或消息页。
- 当前只实现一个真实风格：`Diffusion`。
- `CoolDigitsReserved` 仅作为未来接入点保留，不实现独立页面和设置入口。
- 当前不做表盘市场、不做设置页里的风格选择、不做持久化存储。

这样做的原因很直接：我们现在练的是“表盘模块如何可替换”，不是过早把整个系统拖进一套全局 theme 框架。

## 结构分层

当前表盘采用两层结构：

1. `WatchfacePage`
2. `IWatchfaceStyleRenderer`

### WatchfacePage

职责：

- 订阅公共数据：时间、电量。
- 接收输入事件，尤其是表冠旋转。
- 构建表盘公共宿主容器。
- 把公共数据整理成 `WatchfaceRenderState`。
- 持有 `WatchfaceConfig` 并创建当前风格渲染器。

它不负责：

- 直接写死某一种表盘的视觉布局。
- 直接管理扩散层的几何细节。
- 把表盘风格机制扩散到其他页面。

### IWatchfaceStyleRenderer

职责：

- 构建某一种表盘风格自己的视觉树。
- 消费 `WatchfaceRenderState`。
- 按自己的规则响应表冠输入。

它不负责：

- 决定当前页面是不是表盘页。
- 决定主页环导航。
- 做风格持久化。

## 当前公共接口

### WatchfaceStyleId

- `Diffusion`
- `CoolDigitsReserved`

### WatchfaceConfig

- `style_id`
- `spread_index`

说明：

- `spread_index` 当前只服务于扩散表盘。
- 后续第二种风格可以忽略它，或把它解释成自己的局部视觉参数，但解释权应留在风格实现内部。

### WatchfaceRenderState

- `hour_text`
- `minute_text`
- `battery_percent`
- `spread_index`

说明：

- `hour_text` 允许双位小时，例如 `08`、`10`、`23`。
- 当前扩散效果只作用于小时主数字。
- 分钟和电量继续由宿主页统一管理。

## 表冠分流规则

这条规则必须稳定，否则后续任何表盘视觉都会被输入语义污染。

- 当当前主页表面是 `Watchface`：
  - `CrownRotateCW / CCW` 进入表盘风格自己的消费逻辑。
  - 当前 `Diffusion` 用它调节 `spread_index`。
- 当当前主页表面是其他 4 个快捷页：
  - `CrownRotateCW / CCW` 继续用于主页环切换。

所以状态机层只做一件事：

- 判断“当前是否处于 watchface surface”。

它不应该直接操作扩散层，也不应该知道某个风格内部到底有几层图。

## Web 沙盘与 LVGL 的关系

`prototypes/web_visual_sandbox/watchfaces/diffusion/` 是扩散表盘的视觉合同层。

它负责回答：

- 表盘安全区有多大。
- 小时数字舞台占多大。
- 双位小时的间距关系是什么。
- 电量与分钟的中心轴关系是什么。
- `spread_index` 每一档对应多少切片宽度、偏移和透明度。

它不负责：

- 自动生成 LVGL 代码。
- 替代 LVGL 模拟器。
- 使用 LVGL 很难回译的视觉特效。

当前策略是：

- Web 允许更高反馈的比例细调和拖拽校准。
- LVGL 只做可落地近似：裁切、偏移、透明度、轻微形变。

## Diffusion 风格 v0 约束

信息结构固定为三层：

1. 顶部：电量
2. 中部：小时主数字
3. 底部：分钟

当前扩散实现约束：

- 小时支持双位显示。
- 每一位数字都有 5 层：
  - `core`
  - `left_inner`
  - `left_outer`
  - `right_inner`
  - `right_outer`
- 外层切片应明显比内层更窄。
- 表冠旋转只调节扩散强度，不改分钟或电量布局。

## 后续扩展规则

后续如果新增第二种表盘风格，应遵守：

1. 不改主页环、Launcher、消息页、快捷设置页现有代码路径。
2. 不把表盘风格切换升级成全局 theme 机制。
3. 新增一个独立的风格实现。
4. 新增该风格自己的数字资产或图形资产。
5. 继续由 `WatchfacePage` 统一喂入公共状态。

如果新增风格必须重写 `WatchfacePage` 本身，说明这次接口预留失败，需要回头复盘。
