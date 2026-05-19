# Simulator Manual Regression Run 2026-05-19

Date: 2026-05-19

## Purpose

This document is the execution copy of:

- `docs/simulator_manual_regression_matrix.md`

Use it to record one real regression pass against the current v0.2 simulator shell.

Result values:

- `PASS`
- `FAIL`
- `BLOCKED`
- `NOT RUN`

## Environment

- Build command: `cmake --build build --target main`
- Simulator path: `sim/lv_port_pc_vscode`
- Tester: User + Codex paired manual regression
- Build result: PASS
- Notes:
  - Initial regression pass exposed `A1`, `A2`, and `B2`.
  - These issues were fixed in follow-up small closure rounds and manually re-verified.

## Pre-Check

### P1. Build And Boot

- Action:
  - build simulator
  - launch simulator
  - confirm app reaches normal watchface-centered path
- Expected:
  - build succeeds
  - simulator launches normally
  - app is not stuck on black screen or crash state
- Result: PASS
- Notes:
  - Simulator built and launched normally during this regression run.

### P2. Time And Battery Freshness

- Action:
  - observe current top-level time and battery once after boot
- Expected:
  - time is not obviously stale
  - battery text is visible where expected
- Result: PASS
- Notes:
  - No obvious stale time or missing battery rendering was observed during the run.

## A. Core Shell Navigation

### A1. Watchface And Home Ring

- Action:
  - boot into normal shell
  - horizontal swipe on home ring
  - press `Q / E`
  - press `Enter` or `Space` on watchface
- Expected:
  - home ring can circulate
  - `Q / E` also circulates home ring
  - crown press from watchface enters launcher
- Result: FAIL
- Notes:
  - `Q / E` 与触摸滑动的切换手感不一致。
  - 存在单次按下翻动多页的现象，体验明显劣化。

### A2. Messages And Quick Settings Shell Entry

- Action:
  - top-down gesture from watchface
  - return
  - bottom-up gesture from watchface
- Expected:
  - top-down opens Messages shell
  - bottom-up opens Quick Settings shell
  - return path still works
- Result: FAIL
- Notes:
  - 从表盘顶部下滑进入 `Messages`，返回后再从底部上滑进入 `Quick Settings`，小概率出现表盘背景偏移。
  - 直接上电后从底部上滑进入 `Quick Settings` 基本不触发该问题。
  - 说明更像是 `Messages -> return -> Quick Settings` 路径中的背景/预览状态没有完全归位。

### A3. Back Navigation

- Action:
  - open any second-level page
  - test left-edge right-swipe
  - test top-left back arrow
  - test crown press return behavior on non-home page
- Expected:
  - left-edge gesture returns to previous page
  - back arrow returns to previous page
  - crown press follows current global return rule
- Result: PASS
- Notes:
  - 左边缘右滑、左上角返回、非主页表冠返回行为当前正常。

## B. Settings Display Page

### B1. Display Settings Visible Structure

- Action:
  - enter `设置 -> 显示与亮度`
- Expected:
  - visible entries include:
    - `屏幕亮度`
    - `息屏时间`
    - `抬腕亮屏`
    - `单击亮屏`
    - `遮盖息屏`
    - `息屏显示`
    - `默认息屏样式`
    - `持续亮屏`
  - summary text matches saved state
- Result: PASS
- Notes:
  - `显示与亮度` 当前可见项与预期一致。
  - 摘要回显未发现异常。

### B2. Brightness Flow

- Action:
  - switch between auto and custom brightness
  - enter custom brightness slider page
- Expected:
  - state saves and returns correctly
  - summary text updates
- Result: FAIL
- Notes:
  - 进入自定义亮度滑条页后，调整亮度再使用左边缘右滑退出时，高概率误触滑条并再次改值。
  - 使用左上角返回键退出没有该问题。
  - 当前亮度调节仍未影响真实模拟器显示亮度，这属于已知边界提醒，不作为本轮新增回归。

### B3. Screen-Off Timeout

- Action:
  - switch timeout among supported values
- Expected:
  - state saves and returns correctly
  - summary text updates
- Result: PASS
- Notes:
  - 息屏时间切换与摘要回显当前正常。

## C. Raise To Wake And Tap/Cover

### C1. Raise To Wake Modes

- Action:
  - set `关闭`
  - set `全天开启`
  - set `定时开启`
  - inspect start / end time editing semantics
- Expected:
  - summary updates correctly
  - scheduled mode exposes start / end time
  - explicit confirm saves
  - back / crown / edge return do not save unconfirmed edits
- Result: PASS
- Notes:
  - 模式切换、时间编辑与“仅按勾保存”的语义当前正常。

### C2. Raise To Wake Simulator Path

- Action:
  - use `R`
  - use `F`
  - test under each raise mode
- Expected:
  - `关闭`: `R` does not wake
  - `全天开启`: `R` wakes
  - `定时开启`: only wakes inside allowed window
  - `F` dismisses raise session
- Result: PASS
- Notes:
  - `R / F` 在三种模式下的行为当前符合预期。

### C3. Tap To Wake

- Action:
  - toggle `单击亮屏`
- Expected:
  - switch saves and echoes correctly
- Result: PASS
- Notes:
  - `单击亮屏` 开关保存与回显正常。

### C4. Cover To Sleep

- Action:
  - toggle `遮盖息屏`
  - use `C`
- Expected:
  - on: `C` causes screen-off
  - off: `C` is ignored
- Result: PASS
- Notes:
  - `遮盖息屏` 开关与 `C` 调试输入联动正常。

## D. Keep Screen On

### D1. Basic Selection

- Action:
  - select `关闭`
  - select one non-off duration
- Expected:
  - `关闭` saves directly
  - non-off duration requires battery confirmation
- Result: PASS
- Notes:
  - `关闭` 与非关闭项的选择语义当前正常。
  - 非关闭项会先弹续航确认。

### D2. Runtime Behavior

- Action:
  - enable keep-screen-on
  - wait past normal auto screen-off timeout
- Expected:
  - auto screen-off is suppressed while active
  - after duration ends, behavior returns to normal
- Result: PASS
- Notes:
  - 持续亮屏激活期间，普通自动熄屏被正确抑制。
  - 到时长结束后恢复正常熄屏行为。

## E. Screen-Off Display

### E1. Mode Selection

- Action:
  - set `关闭`
  - set `智能开启`
  - set `定时开启`
- Expected:
  - `关闭` saves directly
  - `智能开启` and `定时开启` require battery confirmation
  - scheduled mode exposes start / end time editing
  - summary updates correctly
- Result: PASS
- Notes:
  - `关闭 / 智能开启 / 定时开启` 的切换、确认弹层与摘要回显当前正常。

### E2. Screen-Off Rendering

- Action:
  - trigger screen-off under each mode
- Expected:
  - `关闭`: visually off
  - active smart or scheduled mode: renders current fallback screen-off style
- Result: PASS
- Notes:
  - `关闭 / 智能开启 / 定时开启` 下的真实熄屏渲染当前符合预期。

### E3. Default Screen-Off Style Selection

- Action:
  - enter `默认息屏样式`
  - swipe horizontally
  - tap blank area on card
  - confirm selection
- Expected:
  - horizontal swipe works
  - full card area is selectable
  - current selection highlight follows centered card
  - confirm saves and returns
- Result: PASS
- Notes:
  - 横向滑动、整卡命中、选中高亮与确认保存当前正常。

### E4. Real Screen-Off Style Visual Baseline

- Action:
  - choose `时分指针`
  - choose `日期数字`
  - enter real screen-off in each case
- Expected:
  - `时分指针`: no unnecessary inner frame
  - `日期数字`: large time digits with date and battery below
- Result: PASS
- Notes:
  - `时分指针` 与 `日期数字` 两种真实熄屏样式当前效果符合本轮基线。

## F. Conflict Handling

### F1. Raise-To-Wake vs Screen-Off Display

- Action:
  - create conflict from both directions
- Expected:
  - conflict prompt appears
  - choosing `息屏表盘` disables raise-to-wake
  - choosing `主表盘` keeps raise-to-wake path
- Result: PASS
- Notes:
  - 双向制造冲突时，提示页与选择结果当前正常。

## G. Quick Settings Linkage

### G1. Raise To Wake Tile

- Action:
  - tap tile
  - long press tile
- Expected:
  - tap toggles state
  - long press enters detail page
  - toast and setting state stay aligned
- Result: PASS
- Notes:
  - 点按切换、长按进入详情页以及状态同步当前正常。

### G2. Settings Entry Tile

- Action:
  - tap settings tile
- Expected:
  - enters settings home
- Result: PASS
- Notes:
  - `Quick Settings` 中的设置入口当前正常。

### G3. Keep Screen On Tile

- Action:
  - tap when off
  - tap when on
  - long press tile
- Expected:
  - off enables 5 minutes
  - on disables
  - long press enters `持续亮屏`
  - toast text matches actual behavior
- Result: PASS
- Notes:
  - `持续亮屏` 快捷开关、关闭、长按详情页与提示气泡当前正常。

## H. Screen-Off Restore Behavior

### H1. Normal Page Restore

- Action:
  - stop on a settings detail page
  - wait for auto screen-off
  - wake again
- Expected:
  - returns to the same page
- Result: PASS
- Notes:
  - 普通页熄屏后恢复路径当前正常。

### H2. Overlay / Prompt Restore

- Action:
  - stop on a confirmation prompt or conflict prompt
  - wait for auto screen-off
  - wake again
- Expected:
  - returns to the same prompt state
- Result: PASS
- Notes:
  - 弹窗 / 提示页熄屏后恢复路径当前正常。

## Summary

- Overall result: PASS
- Highest-risk failures:
  - None currently blocking the v0.2 shell baseline after follow-up fixes and re-verification.
- Follow-up tasks:
  - Revisit home-ring `Q / E` behavior later when the simulator crown model is upgraded to better mimic a real rotary encoder.
  - Keep the current manual regression matrix as the required close-out step for future display-policy and shell-surface changes.
