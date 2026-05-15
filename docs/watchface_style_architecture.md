# Watchface Style Architecture

本文件定义 Magic Watch 当前阶段的表盘风格架构边界。

## 核心结论

- 表盘风格切换是 **watchface-only** 能力。
- 当前不做全局主题系统。
- 当前不做表盘市场、表盘选择页、持久化设置。
- 当前只实现一个真实风格：`Diffusion`。
- `CoolDigitsReserved` 只作为未来接入点占位。

这份边界很重要，因为“切换表盘风格”和“切换整机 UI theme”不是一回事。  
用户现在想训练的是表盘模块的可替换性，不是把整个系统拖进一套过早的主题框架。

## 结构分层

当前表盘采用两层结构：

1. `WatchfacePage`
2. `IWatchfaceStyleRenderer`

### WatchfacePage

职责：

- 订阅时间、电池等公共数据
- 接收表冠输入
- 建立统一表盘根容器
- 把公共数据整理成 `WatchfaceRenderState`
- 决定当前使用哪一个 `WatchfaceStyleId`

它不负责：

- 写死某一种表盘布局
- 直接持有具体扩散层的几何细节
- 把风格切换扩散到主页环、快捷设置或 Launcher

### IWatchfaceStyleRenderer

职责：

- 构建某一种表盘风格自己的视觉树
- 消费 `WatchfaceRenderState`
- 按自己的规则响应表冠旋转

它不负责：

- 决定当前页面是不是表盘
- 决定主页环导航
- 持久化用户偏好

## 当前公共接口

### WatchfaceStyleId

- `Diffusion`
- `CoolDigitsReserved`

### WatchfaceConfig

- `style_id`
- `spread_index`

这里的 `spread_index` 当前只服务于扩散表盘。  
后续如果有第二种风格，它可以忽略这个字段，或者把它解释成自己的局部视觉参数，但这种解释权应该留在风格实现里，而不是塞回宿主页。

### WatchfaceRenderState

- `hour_digit`
- `minute_text`
- `battery_percent`
- `spread_index`

当前 v0 里，扩散效果只作用于小时主数字。  
分钟和电量保持宿主页统一管理。

## 表冠分流规则

这条规则必须稳定，否则后续表盘风格再漂亮也会被输入语义污染。

- 当当前主页表面是 `Watchface`：
  - `CrownRotateCW / CCW` 进入表盘风格自己的消费逻辑
  - 当前 `Diffusion` 用它调节 `spread_index`
- 当当前主页表面是其他 4 个快捷页：
  - `CrownRotateCW / CCW` 继续用于主页环切换

所以状态机层只做一件事：

- 判断“当前是否正处于 watchface surface”

它不应该直接操作扩散图层，也不应该知道某个风格内部到底有几层图。

## Web 沙盘与 LVGL 的关系

`prototypes/web_visual_sandbox/watchfaces/diffusion/` 是表盘风格的视觉合同层。

它负责回答：

- 安全区是多少
- 小时主数字舞台占多大
- 分钟和电量的中心轴关系是什么
- `spread_index` 每一档对应多少偏移、透明度和轻微缩放

它不负责：

- 自动生成 LVGL 代码
- 替代 LVGL 模拟器
- 引入 LVGL 很难回译的复杂特效

当前策略是：

- Web 可以更接近真实模糊观感
- LVGL 只做偏移、裁切、透明度、轻微缩放的近似

## 后续扩展规则

当我们未来新增第二种表盘风格时，应遵守以下约束：

1. 不改主页环、Launcher、Quick Settings 的既有代码路径
2. 不把风格切换升级成全局 theme 机制
3. 新增一个独立的风格实现
4. 新增一套该风格自己的数字/图形资产
5. 由 `WatchfacePage` 选择风格实现并喂入公共状态

如果新增风格需要重写 `WatchfacePage` 本身，说明这次接口预留没有成功，需要回头复盘。
