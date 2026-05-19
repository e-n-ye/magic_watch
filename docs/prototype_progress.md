# Prototype Progress

## 2026-05-19: DisplayPolicyRules Narrow Extraction

### What Changed

- Extracted a pure rule layer for display-policy judgments:
  - `sim/lv_port_pc_vscode/src/App/Common/DisplayPolicyRules.h`
  - `sim/lv_port_pc_vscode/src/App/Common/DisplayPolicyRules.cpp`
- Centralized these pure decisions:
  - time-window evaluation
  - raise-to-wake allowance
  - screen-off display activation
  - auto screen-off suppression
  - raise-to-wake vs screen-off-display conflict detection
- Replaced duplicated call sites in:
  - `AppStateMachine`
  - `SettingsPages`
  - `ShellPages`

### Why This Step Matters

- The project had already started to accumulate parallel copies of the same display-policy rules.
- This was the smallest useful architecture refinement that improved consistency without pulling UI,
  timers, or state ownership into a larger rewrite.
- It creates a cleaner seam for any future coordinator work, while still preserving the current
  v0.2 shell behavior.

### Validation

- `cmake --build build --target main` passed.
- Targeted manual regression checks passed for:
  - `C2`
  - `D2`
  - `E2`
  - `F1`
  - `H1 / H2`

### Current Boundary

- This is not a `DisplayPolicyCoordinator` rollout.
- `AppStateMachine` still owns timers, sleep/wake transitions, and restore logic.
- `SettingsPages` still owns confirmation and conflict UI.
- `ScreenOffPage` still owns rendering.
- No new feature or intended visible behavior change was introduced.

## 2026-05-19: v0.2 Shell Closure Baseline

### What Changed

- Declared the project stage as "late v0 shell prototype" rather than "startup only".
- Froze the current display-policy-oriented shell into a documented v0.2 closure.
- Recorded the currently landed display and screen-off loops:
  - `遮盖息屏`
  - `息屏显示`
  - `默认息屏样式`
  - `持续亮屏`
  - `抬腕亮屏 / 息屏显示` 冲突提示
  - real screen-off rendering
  - restore pre-screen-off page after wake
- Added a dedicated simulator manual regression matrix for the current shell.
- Completed one paired manual regression run against the v0.2 shell baseline.
- Closed the regression issues found in that run:
  - home-ring `Q / E` overshoot on shortcut surfaces
  - `Messages -> return -> Quick Settings` backdrop offset residue
  - manual brightness slider accidental value changes during edge-back
  - raise-to-wake scheduled-card selection-dot layout inconsistency
- Added:
  - `docs/v0_2_shell_closure.md`
  - `docs/simulator_manual_regression_matrix.md`
  - `docs/simulator_manual_regression_run_2026_05_19.md`

### Why This Step Matters

- The shell is now rich enough that undocumented behavior has become a real source of regressions.
- The highest-value work is no longer "add another page", but "make the current shell explainable and testable".
- This creates a stable comparison point before later architecture cleanup or hardware migration.

### Current Boundary

- This is a documentation and regression-baseline closure, not a new feature round.
- It does not introduce watchface-specific paired screen-off styles yet.
- It does not move the project onto real hardware.
- The simulator crown model is still a useful approximation, not a true rotary-encoder simulation.

## 2026-05-16: v0.1 Display Policy Model

### What Changed

- Added a lightweight `DisplayPolicyModel` in the shared app model layer.
- Moved notification wake policy out of `NotificationCenterModel`.
- Updated Quick Settings so display-related toggles write display policy state instead of notification state or page-local state.
- Updated `AppStateMachine` so notification wake and crown wake decisions read the display policy model.
- Added a visible simulator input contract: `P` is now a simulator-only debug key, while `5` remains a legacy alias.
- Connected the default 5-second auto screen-off timeout to the state machine and made `ScreenOff` render as a near-black simulator state.
- Re-aligned the product model around screen + pressable crown: crown press wakes from screen-off, opens Launcher on the watchface, and returns home from other pages.
- Added the architecture re-entry note:
  - `docs/v0_1_architecture_closure.md`

### Current Boundary

- This is a policy ownership cleanup, not real low-power hardware support.
- Raise-to-wake, tap-to-wake, always-on display, and auto screen-off are still simulator-level policy placeholders.
- Auto screen-off is only a simulator state transition, not real display power control.
- Quick Settings still contains shell toggles that do not all map to real services yet.

## 2026-05-14: Web Visual Sandbox + Home Ring Cleanup

### What Changed

- Started a dedicated Web visual sandbox for the weather shortcut surface:
  - `prototypes/web_visual_sandbox/home_ring_weather/`
- Locked the sandbox to a LVGL-friendly rule set instead of using web-only effects.
- Added two screen presets so the page can be pressure-tested against small geometry changes.
- Tightened the current LVGL home-ring surfaces:
  - removed visible `Home Ring x/4` copy from the shortcut pages
  - weakened the bottom pager dots
  - stopped generic pages from reserving visual space for empty subtitle text
  - simplified the Weather bottom cards to label + value + progress only
- Added a dedicated workflow doc:
  - `docs/web_visual_sandbox_workflow.md`

### Why This Step Matters

- We now have a reusable middle layer between "reference screenshot" and "LVGL code".
- This should reduce the current failure mode where layout intent gets lost during direct simulator editing.
- The cleanup pass also makes the current LVGL shell less noisy while we prepare the stronger visual spec path.

### Current Boundary

- The Web sandbox is a design contract, not a production UI runtime.
- LVGL remains the final implementation target.
- The other three home-ring pages still need a later dedicated visual pass.

## 2026-05-14: Weather Shortcut Layout Variant v1

### What Changed

- Kept `HomeShortcutPage` as the reusable home-ring baseline for generic shortcut surfaces.
- Added `WeatherShortcutPage` as the first dedicated layout variant for the home ring.
- Switched `HomeShortcutWeather` registration from the generic 4-metric template to the new specialized page.
- Landed the current approved visual structure in simulator code:
  - top weather hero card
  - bottom sleep / steps dual cards
  - weak pager dots
  - obvious black outer breathing room
- Used relative sizing derived from screen dimensions instead of locking the page to one fixed pixel composition.

### Why This Step Matters

- We now have our first real proof that the home ring should be built as "shared skeleton + small number of high-value variants", not as one rigid template forced onto every page.
- This is the first time the generated visual workflow has been closed back into simulator UI structure, instead of staying only as reference imagery.

### Current Boundary

- This page is still a shell surface with mock data, not a live Weather / Sleep / Steps service page.
- The Weather page has been specialized, but the other three home-ring pages still use the generic template.
- The current implementation focuses on proportion and visual hierarchy first; icon polish and richer card internals can come in later rounds.

## 2026-05-14: Home Shortcut Template + Visual Workflow

### What Changed

- Replaced the 4 home-ring shortcut placeholders with a reusable `HomeShortcutPage` template.
- Unified the four pages around the same structure:
  - orbit label
  - title and subtitle
  - one focus card
  - four compact metric cards
- Kept the current home-ring behavior intact while improving the skeleton quality of the surfaces.
- Added a project-scoped image workflow:
  - `.agents/skills/watch-visual-workflow/`
  - Right Code image-generation script
  - Right Code chat-analysis script
  - reusable prompt templates
  - local config example and generated-output convention

### Why This Step Matters

- The home ring is no longer just four unrelated placeholder pages.
- We now have a stable surface template that can later absorb real service data with smaller code churn.
- Visual exploration is no longer trapped in chat memory; it has become a repo-level workflow.

### Current Boundary

- The 4 shortcut pages are still shell surfaces, not real business implementations.
- The new image workflow is scaffolded and documented, but it was not live-tested against the remote API because no project key is configured in this session.

## 2026-05-14: Weather Shortcut Visual Brief v1

### What Changed

- Locked the first visual exploration target to `Weather / Steps / Sleep`.
- Used the Xiaomi reference set to extract a concrete layout direction instead of writing a generic image prompt.
- Added:
  - `docs/weather_shortcut_visual_direction.md`
  - a tighter Weather-page prompt
  - a dedicated screenshot-analysis prompt
- Ran the first actual `gpt-image-2` generation round for the Weather shortcut surface.
- Stored the first round result under:
  - `.agents/generated/home-shortcut-weather-round1/20260514_125859_image_generation.json`
  - `.agents/generated/home-shortcut-weather-round1/20260514_125859_image_generation.png`

### Important Design Correction

- The earlier prompt language assumed a round watch display.
- For the current simulator and the current Xiaomi reference set, that assumption is not ideal.
- The first visual generation round should target a rounded-square watch safe area first, then later evaluate portability to other hardware shapes.

### Current Boundary

- This is still a first-round mood-and-structure image, not a production asset.
- We have now crossed from “workflow scaffold only” into “first real generated reference exists”.

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
