# Simulator Manual Regression Matrix

Date: 2026-05-23

## Purpose

This checklist is the manual regression baseline for the current simulator shell.

Use it when:

- closing a shell-related task
- closing a battery / power-mode task
- changing display policy behavior
- changing settings detail flow
- changing quick settings behavior
- changing screen-off / wake behavior
- changing input routing or page restoration

This matrix is intentionally manual-first.
It is meant to prevent shell regressions before any heavier automation exists.

## Input Contract

Primary simulator inputs:

- `Enter` / `Space`: crown press
- `Q / E`: crown rotate
- `S`: simulate +100 daily steps
- `R`: simulate raise-to-wake
- `F`: simulate raise-dismiss
- `C`: simulate cover-to-sleep
- touch:
  - tap
  - drag
  - flick
  - edge swipe

## Pre-Check

Before testing:

1. Build with `cmake --build build --config Debug`
2. Start the simulator from `sim/lv_port_pc_vscode`
3. Make sure the app can boot to the normal watchface path
4. Record the current time and battery display once, so obvious stale rendering is easy to spot

## A. Core Shell Navigation

### A1. Watchface And Home Ring

- Expected:
  - boot enters watchface or home-centered shell
  - horizontal swipe can circulate the home ring
  - `Q / E` can also circulate the home ring
  - crown press from watchface enters launcher

### A2. Messages And Quick Settings Shell Entry

- Action:
  - top-down gesture from watchface
  - bottom-up gesture from watchface
- Expected:
  - top-down opens Messages shell
  - bottom-up opens Quick Settings shell
  - close drag and return behavior still work

### A3. Back Navigation

- Action:
  - left-edge right-swipe on second-level pages
  - top-left back arrow tap
  - crown press on non-home pages
- Expected:
  - left-edge gesture returns to previous page
  - top-left back arrow returns to previous page
  - crown press returns to watchface-centered home surface according to current shell rule

### A4. Launcher Shell Structure

- Action:
  - enter `Launcher`
  - switch `设置 -> 应用布局` among `多列布局 / 列表布局 / 分类布局`
  - return to `Launcher` after each switch
  - scroll through the full page by touch drag / flick
  - scroll through the full page by `Q / E`
- Expected:
  - `Launcher` follows the saved `应用布局` mode
  - current exposed entries remain limited to:
    - `Settings`
    - `Weather`
    - `Steps`
    - `Sleep`
    - `Heart`
    - `SpO2`
    - `Stress`
    - `Breathe`
    - `NFC`
    - `Alipay`
    - `WeChat`
  - `多列布局`:
    - shows an icon wall without per-app text labels
    - still supports vertical drag / flick and `Q / E` scrolling
  - `列表布局`:
    - shows a vertical one-row-per-app list
    - app labels remain visible
  - `分类布局`:
    - shows grouped sections:
      - `System`
      - `Daily`
      - `Health`
      - `Wallet`
  - touch scroll and crown scroll both keep working

### A5. Weather App Shell

- Action:
  - enter Weather from the home-ring Weather card
  - enter `Launcher`, switch among `多列布局 / 列表布局 / 分类布局`, and open Weather from each mode
  - vertically drag / flick through the Weather app sections
  - horizontally drag the hourly forecast section
  - horizontally drag the seven-day forecast section
  - press `Q / E`
  - use left-edge right swipe
  - press crown from Weather
- Expected:
  - Weather is a real mock app page, not a placeholder
  - home-ring and all three Launcher layouts enter the same Weather app
  - vertical touch scrolling snaps to Weather sections instead of resting between pages
  - hourly and seven-day forecast sections support horizontal browsing
  - `Q / E` pages through Weather sections
  - left-edge right swipe returns to the previous page
  - crown press returns to the watchface-centered home surface
  - no weather API, city management, settings linkage, or other app behavior is introduced

### A6. Steps App Stream Shell

- Action:
  - enter Steps from the home-ring Steps card
  - enter `Launcher`, switch among `多列布局 / 列表布局 / 分类布局`, and open Steps from each mode
  - vertically drag / flick through the Steps app
  - press `Q / E`
  - tap `数据说明`
  - tap the top-left back button on the data-description page
  - vertically drag / flick through the data-description page
  - use left-edge right swipe from both Steps pages
  - press crown from both Steps pages
- Expected:
  - Steps is a real mock app page, not a placeholder
  - home-ring and all three Launcher layouts enter the same Steps app
  - vertical touch scrolling is continuous / flowing, not page-snapped
  - `Q / E` scrolls the same vertical content context
  - `数据说明` opens a normal pushed page
  - the data-description top-left back button returns to the Steps page
  - the data-description page supports vertical scrolling
  - the home-ring Steps foot icon does not block card navigation
  - the first Steps overview card is sized against the current simulator display height
  - left-edge right swipe returns to the previous page
  - crown press returns to the watchface-centered home surface
  - no sensor stream, health data model, goal setting, historical trend, or settings linkage is introduced

### A7. Home Shortcut Shared StepsModel

- Action:
  - stay on the home-ring Weather shortcut page
  - note the current steps value shown on the small bottom-right steps card
  - press `S`
  - confirm the Weather shortcut page steps card updates by `+100`
  - enter `Steps`
  - confirm the key step numbers match the Weather shortcut card
- Expected:
  - the Weather shortcut steps card is no longer a fixed mock value
  - `S` updates the home shortcut card and the Steps page through the same shared `StepsModel`
  - the Weather hero card and sleep card remain unchanged in this round

## B. Settings Display Page

### B1. Display Settings Visible Structure

- Action:
  - enter `设置 -> 显示与亮度`
- Expected:
  - visible entries include:
    - 屏幕亮度
    - 息屏时间
    - 抬腕亮屏
    - 单击亮屏
    - 遮盖息屏
    - 息屏显示
    - 默认息屏样式
    - 持续亮屏
  - row order matches current product understanding
  - summary text matches current saved state

### B2. Brightness Flow

- Action:
  - switch between auto and custom brightness
  - enter custom brightness slider page
- Expected:
  - state saves and returns correctly
  - summary text updates

### B3. Screen-Off Timeout

- Action:
  - switch timeout among supported values
- Expected:
  - state saves and returns correctly
  - summary text updates

## C. Raise To Wake And Tap/Cover

### C1. Raise To Wake Modes

- Action:
  - set `关闭`
  - set `全天开启`
  - set `定时开启`
- Expected:
  - summary updates correctly
  - scheduled mode expands or exposes start / end time
  - time editing requires explicit confirm to save
  - back / crown / edge return do not save unconfirmed time edits

### C2. Raise To Wake Simulator Path

- Action:
  - under each raise mode, use `R`
  - after a raise wake session, use `F`
- Expected:
  - `关闭`: `R` does not wake
  - `全天开启`: `R` wakes
  - `定时开启`: only wakes inside allowed time window
  - `F` dismisses the raise view session

### C3. Tap To Wake

- Action:
  - toggle `单击亮屏`
- Expected:
  - switch saves and echoes correctly

### C4. Cover To Sleep

- Action:
  - toggle `遮盖息屏`
  - use `C`
- Expected:
  - on: `C` causes screen-off
  - off: `C` is ignored

## D. Keep Screen On

### D1. Basic Selection

- Action:
  - select `关闭`
  - select a non-off duration
- Expected:
  - `关闭` saves directly
  - non-off durations require battery confirmation

### D2. Runtime Behavior

- Action:
  - enable a keep-screen-on duration
  - wait beyond the normal auto screen-off timeout
- Expected:
  - auto screen-off is ignored while keep-screen-on is active
  - after the chosen duration ends, policy clears and screen can turn off again

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

### E2. Screen-Off Rendering

- Action:
  - let the device enter screen-off under each mode
- Expected:
  - `关闭`: visually off
  - active `智能开启` or valid `定时开启`: renders current fallback screen-off style

### E3. Default Screen-Off Style

- Action:
  - enter `默认息屏样式`
  - swipe horizontally between two cards
  - tap card blank area
  - tap confirm
- Expected:
  - horizontal swipe works
  - full card area is selectable
  - current selection highlight follows the centered card
  - confirm saves the selected style
  - display page summary updates after return

### E4. Current Real Screen-Off Styles

- Action:
  - choose `时分指针`
  - choose `日期数字`
  - enter real screen-off in each case
- Expected:
  - `时分指针`: no unnecessary inner frame; battery is positioned below the hands cluster
  - `日期数字`: large time digits, with date and battery below; no unnecessary inner frame

## F. Conflict Handling

### F1. Raise-To-Wake vs Screen-Off Display

- Action:
  - make `抬腕亮屏` and `息屏显示` enter a conflicting state from both directions
- Expected:
  - conflict prompt appears
  - choosing `息屏表盘` disables raise-to-wake
  - choosing `主表盘` keeps raise-to-wake semantics

## G. Quick Settings Linkage

### G1. Raise To Wake Tile

- Action:
  - tap the tile
  - long press the tile
- Expected:
  - tap toggles state
  - long press enters `抬腕亮屏`
  - state and toast stay aligned with settings page

### G2. Settings Entry Tile

- Action:
  - tap the tile
- Expected:
  - enters settings home

### G3. Keep Screen On Tile

- Action:
  - tap when off
  - tap when on
  - long press
- Expected:
  - off -> enables 5 minutes
  - on -> disables
  - long press enters `持续亮屏`
  - toast text matches actual behavior

## H. Screen-Off Restore Behavior

### H1. Normal Page Restore

- Action:
  - stop on a settings detail page
  - wait for auto screen-off
  - wake again
- Expected:
  - returns to the same page instead of forcing watchface

### H2. Overlay / Prompt Restore

- Action:
  - stop on a confirmation prompt or conflict prompt
  - wait for auto screen-off
  - wake again
- Expected:
  - returns to the same prompt state
  - prompt content and buttons remain visible

## I. Battery And Long Battery Mode

### I1. Battery Settings Entry

- Action:
  - enter `设置`
  - confirm `电池` is visible on the settings home page
  - enter `电池`
- Expected:
  - `电池` is a first-level settings entry, not a hidden path
  - entering it opens the battery status page instead of a placeholder
  - battery page shows current percent, duration estimate, and status text

### I2. Battery Sub-Pages

- Action:
  - from `电池`, open `说明`
  - return to `电池`
  - open `长续航模式`
  - return to `电池`
  - open `续航优化`
  - return to `电池`
- Expected:
  - all three entries are reachable from the battery page
  - back arrow, edge-back, and crown press return correctly
  - `续航优化` page keeps its local toggle interaction without breaking navigation

### I3. Low Battery Notification Baseline

- Action:
  - from a normal watchface path, press `B`
  - open the notifications surface if needed
- Expected:
  - low battery notification still appears
  - battery notification styling remains readable
  - battery page and watchface battery text do not become stale after the injection

### I4. Enter Long Battery From Settings

- Action:
  - go to `设置 -> 电池 -> 长续航模式`
  - enable the main switch
- Expected:
  - enabling the switch enters the long-battery watchface
  - the long-battery watchface only shows date / week, time, battery, and steps model value
  - home-ring swipe, notifications pull, and quick-settings pull do not open shell surfaces in this mode

### I5. Long Battery Watchface Interaction Contract

- Action:
  - on the long-battery watchface, single tap the screen
  - return to the long-battery watchface and try drag / flick / swipe gestures
  - press crown from the long-battery watchface
- Expected:
  - only a confirmed single tap enters the long-battery exit page
  - drag / flick / swipe do not accidentally enter the exit page
  - crown press does not jump back to the normal home watchface path

### I6. Long Battery Exit Direction And Wake Restore

- Action:
  - from the long-battery exit page, rotate `E`
  - rotate `Q`
  - let the device auto screen-off on the exit page
  - wake again with the currently allowed wake path
- Expected:
  - `E` increases exit progress
  - `Q` decreases exit progress
  - progress is clamped and does not wrap
  - after screen-off and wake, the app returns to the long-battery watchface instead of restoring the exit page

### I7. Exit Long Battery

- Action:
  - from the long-battery exit page, rotate `E` until the progress completes
  - re-enter long-battery mode
  - trigger charging or external power with the simulator battery stream if available
- Expected:
  - completed crown progress exits long-battery mode and returns to the normal home watchface path
  - charging or external power also exits long-battery mode
  - after exit, notifications pull, quick-settings pull, and home-ring navigation work again

### I8. Enter Long Battery From Quick Settings

- Action:
  - from the normal watchface path, open `Quick Settings`
  - tap the `长续航模式` tile while it is off
  - cancel once
  - tap it again and confirm
- Expected:
  - opening from quick settings first shows the confirmation prompt
  - cancel keeps the app in the normal mode
  - confirm enters the same long-battery watchface path as the settings entry
  - the quick-settings tile visual state stays aligned with the current power mode

## J. Steps / Activity Service

### J1. Simulator Step Sample Injection

- Action:
  - enter long-battery mode
  - note the current steps value on the long-battery watchface
  - press `S` once
  - return to or stay on the long-battery watchface
  - press `S` again
- Expected:
  - each `S` press increases the displayed steps by `100`
  - the updated value comes from the shared model path, not from a page-local button
  - long-battery watchface rendering updates without breaking the current battery / time display

### J2. Existing Battery Paths Stay Stable

- Action:
  - after using `S`, press `B`
  - open the long-battery exit page
  - rotate `E / Q`
- Expected:
  - low-battery notification baseline still works
  - long-battery exit interaction is unchanged
  - adding the steps service does not break the v0.5 battery / power-mode path

### J3. Steps App Shared Model Consumer

- Action:
  - stay in normal mode
  - open `Steps`
  - note the current step number in the overview and the large steps card
  - press `S`
  - confirm both visible step numbers update
  - press `S` again
- Expected:
  - `StepsAppPage` no longer shows a page-local fixed steps value
  - the overview step metric and the large steps card stay in sync
  - each `S` press increases both values by `100`
  - when the steps value reaches four digits, it stays on one line instead of wrapping
  - steps / kcal / active cards follow the current simulator width instead of relying on legacy fixed sizing
  - vertical scroll, crown scroll, and `数据说明` navigation still work

## Exit Criteria

The current simulator baseline can be considered stable enough for a new round when:

- all sections above pass without obvious regression
- all long-battery entry / exit paths pass without page-jump regressions
- no display-policy state saves to the wrong page
- no wake / screen-off path jumps to an unexpected page
- no quick-settings control drifts away from settings-page state
- no screen-off style selection breaks swipe or hit-testing
