# Web 视觉沙盘工作流

## 目的

本工作流服务于 `Magic Watch` 的 UI 预研，不替代 LVGL，也不作为自动代码源。

它的作用是补一层高反馈的中间设计契约，把：

- 参考图片
- 文字描述
- 聊天中的审美判断

收敛成可量化、可复盘、可回译的页面规格。

当前第一块沙盘入口：

- `prototypes/web_visual_sandbox/home_ring_weather/index.html`
- `prototypes/web_visual_sandbox/launcher_grid/index.html`

当前视觉基线：

- 先以接近小米参考图的 `240 x 296` 圆角方屏比例作为审美基线
- 这不是最终硬件绑定，只是当前统一网页沙盘和 LVGL 模拟器的参考尺寸

## 明确决策

### 决策 1：Web 沙盘是视觉规格中间层

- 最终落地仍然是 `LVGL`
- Web 沙盘只负责快速试：
  - 比例
  - 安全区
  - 信息层级
  - 图标槽位
  - 文字预算

### 决策 2：不做网页自动转 LVGL

当前不做 HTML/CSS 到 LVGL 的自动转换。

原因：

- 这种转换链实现成本高
- 一旦网页使用了 LVGL 难还原的效果，自动转换会制造更大偏差
- 当前项目更需要稳定的中间规格，而不是新的复杂工具链

### 决策 3：Web 沙盘必须遵守 LVGL 友好约束

允许：

- 纯色块
- 圆角
- 简单透明度
- 位图图标
- 明确字号层级
- 比例驱动的间距和尺寸

不允许：

- 模糊
- 复杂阴影
- 混合模式
- 依赖浏览器排版技巧的自由特效
- 视觉上漂亮但无法稳定回译到 LVGL 的网页做法

## 页面规格接口

Web 沙盘至少保留以下 CSS 变量作为设计接口：

- `--watch-width`
- `--watch-height`
- `--safe-inset-x`
- `--safe-inset-top`
- `--safe-inset-bottom`
- `--stage-radius`
- `--card-gap`
- `--hero-ratio`
- `--metric-font-scale`

这些变量不是为了做网页主题系统，而是为了让后续回译时可以明确回答：

- 哪些尺寸跟屏幕宽高相关
- 哪些尺寸跟安全区相关
- 哪些文本层级会先失稳

## Weather 页当前预算

第一块视觉母版只允许保留 4 个焦点：

1. 当前温度
2. 温度区间
3. 睡眠值
4. 步数值

因此当前页面必须满足：

- 顶部只保留：
  - 温度
  - 温度区间
  - 单个天气图标
- 底部两卡只保留：
  - 单个主值
  - 单个小图标

当前明确不保留：

- `Home Ring 4/4`
- 解释性副标题
- 冗余状态说明
- `Sleep` / `Steps` 标签
- 大尺寸抢眼页码点

## Web -> LVGL 回译规则

### 一比一回落的内容

- 卡片层级关系
- 卡片数量
- 安全区边界
- 大致比例
- 文本主次关系
- 图标位置

### 允许简化的内容

- 位图图标的细节精度
- 颜色透明度的轻微偏差
- 边框明暗的轻微差异

### 不允许回译时自行加回来的内容

- 新的解释文字
- 新的页码说明
- 新的辅助标签
- 为了“信息完整”而额外加回的多层文案

## 第一版视觉基线

日期：`2026-05-14`

用途：

- 作为 `Weather / Sleep / Steps` 主页环第 4 页的第一版可接受视觉基线
- Web 沙盘与 LVGL 天气快捷页都应优先向这组参数对齐

基础屏幕参数：

- `watchWidth = 240`
- `watchHeight = 296`
- `safeInsetX = 10`
- `safeInsetTop = 20`
- `safeInsetBottom = 16`

关键布局参数：

```json
{
  "heroCard": { "x": 0, "y": 15.48, "width": 220, "height": 110 },
  "temperature": { "x": 18, "y": 20.26, "width": 85.29, "height": 40.90, "fontSize": 35.02 },
  "range": { "x": 21.87, "y": 65.23, "width": 73.29, "height": 24, "fontSize": 11.66 },
  "weatherIcon": { "x": 140.23, "y": 17.03, "width": 73, "height": 73 },
  "sleepCard": { "x": 0.97, "y": 135.68, "width": 106, "height": 106 },
  "stepsCard": { "x": 114, "y": 135.68, "width": 106, "height": 106 },
  "sleepIcon": { "x": 0, "y": 0, "width": 58.39, "height": 58.39 },
  "stepsIcon": { "x": 0, "y": 0, "width": 58.39, "height": 58.39 },
  "sleepValue": { "x": 12.13, "y": 68.39, "width": 71.42, "height": 24, "fontSize": 13.77 },
  "stepsValue": { "x": 12.45, "y": 70.32, "width": 75.10, "height": 30.06, "fontSize": 16.82 }
}
```

视觉结论：

- 顶部天气卡与底部两卡之间的留白已经达到可接受状态
- 天气卡高度应与两张底部卡的组合高度形成更紧凑的整体
- 睡眠与步数图标需要左上角占位明确，但不再叠加文字标签
- 步数值允许比睡眠值更强调一些，以匹配视觉重心

## 每轮迭代步骤

1. 先确定当前只优化一个表面
2. 在 Web 沙盘里调比例和内容预算
3. 截图并记录这轮参数
4. 如果需要图标资产，走 `gpt-image-2` 工作流
5. 评审通过后，再手工翻译回 LVGL
6. 把决定写回仓库文档，而不是只留在聊天里

## 图标资产协作规则

当前主页环第 4 页优先使用项目内正式图标资产：

- `assets/generated_icons/weather_cloud_sun.png`
- `assets/generated_icons/sleep_bed.png`
- `assets/generated_icons/steps_foot.png`

生成工作流仍然保留在：

- `.agents/generated/weather-icon-cloud-sun/`
- `.agents/generated/sleep-icon-bed/`
- `.agents/generated/steps-icon-foot/`

规则是：

- `.agents/generated/` 保存原始生成结果和 JSON 请求记录
- `assets/generated_icons/` 保存经过去背、裁剪、尺寸归一化后的项目消费版本
- Web 和 LVGL 默认都指向 `assets/generated_icons/`
- 如果正式资产缺失，页面允许退回几何 fallback，但这表示资产链未闭环，不表示页面完成

## 当前结论

对于本项目现阶段，最稳的协作范式是：

`参考图 / 讨论 -> Web 沙盘 -> 图标资产 -> 规格文档 -> LVGL 手工回译`

而不是：

`参考图 / 讨论 -> 直接写 LVGL -> 再靠截图返工`
