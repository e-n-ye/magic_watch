# Prototype Progress

## 2026-05-14: Baseline Freeze Note

The simulator now has a first coherent v0 shell baseline.

Verified in manual testing:

- `Q / E` crown rotation works
- home ring horizontal swipe is easier to trigger than before
- watchface top-down entry opens Messages
- watchface bottom-up entry opens Quick Settings
- Launcher list supports touch drag and inertia
- Settings list supports touch drag and inertia

For the clean current-state summary, read:

- `docs/v0_shell_baseline.md`

## 2026-05-13: Settings Scroll Prototype v0

### Intent

- 先验证输入语义层，而不是直接做完整主页环。
- 先验证流式页面中的三件关键事：
  - 触摸上下拖动是否跟手
  - 松手后的惯性语义是否成立
  - 表冠旋转和触摸滚动是否作用于同一滚动上下文

### What Changed

- 在模拟器 App 层新增 `InputIntentRouter`：
  - 将原始按键、触摸、表冠输入整理为高层意图
  - 当前已落地的意图包括：
    - `NavigateBack`
    - `HomeEdgeBackRight`
    - `CrownPress`
    - `CrownRotateCW`
    - `CrownRotateCCW`
    - `ScrollDrag`
    - `ScrollFlick`
    - `OpenLauncher`
    - `OpenQuickSettings`
- 在模拟器 HAL 层补了表冠模拟输入：
  - `Enter` / `Space` -> `CrownPress`
  - `E` -> `CrownRotateCW`
  - `Q` -> `CrownRotateCCW`
- 把 `SettingsHome` 改成第一版滚动样机页：
  - 列表保持 LVGL 原生滚动
  - 开启纵向滚动、弹性和动量
  - 页面订阅高层输入意图
  - 表冠旋转驱动同一列表滚动
  - 页面底部显示最近一次交互提示
- 补了主页环第一版骨架：
  - `Watchface + 4` 个快捷页作为同一组 `home surface`
  - 左边缘右滑可在主页环中反向流转
  - 右边缘左滑可在主页环中正向流转
  - `Q / E` 在主页环中也可直接切换页面
  - `Enter / Space` 在主页环中进入 `Launcher`
- 根据第一轮手测反馈，继续收紧了 v0 壳层：
  - 表盘顶部下滑现在进入 `Messages` mock page
  - 表盘底部上滑现在进入 `Quick Settings`
  - `Launcher` 从分页网格改成流式纵向列表
  - `Launcher` 和 `Messages` 都支持触摸滚动与 `Q / E` 表冠滚动
  - 模拟器补了整屏水平甩动识别，主页环不再只依赖边缘触发
  - 降低了边缘 / 水平 / 纵向手势提交阈值
  - 主循环最大 sleep 收紧到 `16ms`，提高键盘输入采样响应
  - 旧 GPS / LoRa / 红外等历史入口不再作为 launcher 主入口暴露

### Current Boundary

- 这不是最终主页环实现。
- 主页环当前还是轻量骨架，快捷页内容仍是占位页，不是最终业务页面。
- 触摸滚动物理当前优先复用 LVGL 默认能力，不在这个阶段自写滚动引擎。
- `Messages` 和 `Quick Settings` 仍是 v0 shell，不代表已接入真实手机同步或真实系统配置服务。

### Why This Step First

- 如果输入语义层没有先稳定，主页环、消息页、快捷设置页和表冠行为后面会重新缠在一起。
- `SettingsHome` 是最小但有效的交互验证页，能把“滚动”和“返回”的边界先做稳。

### Next Checkpoints

- 验证设置页中部拖动滚动是否自然
- 验证左边缘右滑是否稳定返回
- 验证 `Q / E` 表冠滚动与触摸滚动方向和节奏是否一致
- 验证主页环的循环流转是否符合“表盘中心”心智模型
- 验证 launcher 列表是否足够像手表应用入口，而不是桌面分页
- 通过后，再把主页环从占位页推进到真正的布局模板
