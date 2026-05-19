# Simulator Manual Regression Matrix

Date: 2026-05-19

## Purpose

This checklist is the manual regression baseline for the current v0.2 simulator shell.

Use it when:

- closing a shell-related task
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

1. Build with `cmake --build build --target main`
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
  - historical or currently non-shell-facing entries such as `GPS`, `Recorder`, `AudioPlayer`,
    `VideoPlayer`, `Infrared`, `LoRa`, and similar placeholders are not surfaced here
  - touch scroll and crown scroll both keep working

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

## Exit Criteria

The current v0.2 shell baseline can be considered stable enough for a new round when:

- all sections above pass without obvious regression
- no display-policy state saves to the wrong page
- no wake / screen-off path jumps to an unexpected page
- no quick-settings control drifts away from settings-page state
- no screen-off style selection breaks swipe or hit-testing
