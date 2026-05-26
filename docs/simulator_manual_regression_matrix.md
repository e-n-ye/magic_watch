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
- `N`: inject one message notification
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

## K. Notification Service

### K1. Simulator Notification Sample Injection

- Action:
  - stay on a normal watchface path
  - press `N`
  - open `Notifications`
- Expected:
  - one new message notification appears in the list
  - this round still uses simulator-only injection, but the path is now `NotificationSample -> NotificationService -> DataCenter`
  - the notification list is refreshed from the shared model instead of page-local mock rows

### K2. Existing Notification Surfaces Stay Stable

- Action:
  - after pressing `N`, observe toast behavior
  - let the notification wake preview appear if current policy allows it
  - dismiss or close it through the existing path
- Expected:
  - toast still appears
  - wake preview current behavior is not broken by the service refactor
  - `Notifications` list, wake preview, and toast all read the same newly injected notification

### K3. Existing Battery And Long-Battery Paths Stay Stable

- Action:
  - after using `N`, press `B`
  - enter long-battery mode
  - return to the notifications surface if needed
- Expected:
  - low-battery notification baseline still works
  - long-battery path is not affected by the notification service refactor
  - adding the notification service does not break the existing battery / power-mode / steps closures
## L. Notification Preview And Shell Linkage

### L1. Screen-Off Notification Wake Preview

- Action:
  - let the watch enter `ScreenOff`
  - press `N`
- Expected:
  - if notification wake is enabled, the app wakes into `NotificationWakePreview`
  - the preview shows the same newest notification content that later appears in the notifications list

### L2. Preview To Notifications List

- Action:
  - while `NotificationWakePreview` is visible, tap the preview card
  - re-enter the preview path and press crown
- Expected:
  - both actions enter the full `Notifications` list
  - entering the list ends the temporary wake-preview-only session and turns into normal notifications browsing

### L3. Notifications List Open During New Arrival

- Action:
  - open `Notifications`
  - press `N`
- Expected:
  - the list refreshes with the new notification
  - no extra wake preview is opened
  - no extra toast is layered on top of the already open notifications list

## M. Notification Read State And Detail Entry

### M1. New Notifications Start Unread

- Action:
  - stay on a normal watchface path
  - press `N`
  - open `Notifications`
- Expected:
  - the newest notification appears in the list as unread
  - entering the list alone does not mark the notification as read

### M2. Wake Preview To List Still Unread

- Action:
  - let the watch enter `ScreenOff`
  - press `N`
  - enter `Notifications` from `NotificationWakePreview`
- Expected:
  - the notification is still unread after moving from preview into the notifications list
  - wake preview is not treated as formal reading

### M3. Tap List Card To Enter Detail And Mark Read

- Action:
  - in `Notifications`, tap a notification card
  - confirm detail mode appears
  - use the top-left back button to return to the list
- Expected:
  - tapping the card opens detail content inside `NotificationsPage`
  - the notification becomes read when detail opens
  - returning to the list shows a muted read state for that same notification

### M4. Existing Baselines Stay Stable

- Action:
  - after completing M1-M3, press `B`
  - optionally re-enter and exit long-battery mode
- Expected:
  - low-battery notification baseline still works
  - notification read/detail changes do not break the long-battery path

## N. Notification Clear Semantics

### N1. Clear Requires Confirmation

- Action:
  - open `Notifications`
  - tap `清空`
- Expected:
  - a confirmation overlay appears
  - the list is not cleared immediately

### N2. Cancel Keeps Existing Notifications

- Action:
  - while the confirmation overlay is visible, tap `取消`
- Expected:
  - the overlay closes
  - the notification list remains unchanged

### N3. Confirm Clears The List

- Action:
  - tap `清空` again
  - tap `确认`
- Expected:
  - all current notifications are cleared
  - the page returns to the empty-state view

### N4. Overlay Blocks Underlying List Motion

- Action:
  - show the clear confirmation overlay
  - try crown rotation or list drag
- Expected:
  - the underlying notifications list does not continue scrolling
  - the confirmation layer stays visually stable

### N5. Existing Baselines Stay Stable

- Action:
  - after clearing, press `N`
  - then press `B`
- Expected:
  - new notifications can still be injected normally
  - low-battery notification baseline still works

## O. Notification Swipe Delete

### O1. Card Follows Right Swipe

- Action:
  - in `Notifications` list view, press and drag one notification to the right
- Expected:
  - the card moves with the finger instead of deleting immediately

### O2. Short Swipe Springs Back

- Action:
  - drag a notification card to the right, but do not pass the dismiss threshold
  - release the touch
- Expected:
  - the card springs back into place
  - the notification is not deleted

### O3. Long Swipe Deletes Only That Card

- Action:
  - drag a notification card to the right past the dismiss threshold
  - release the touch
- Expected:
  - only that one notification is deleted
  - the rest of the list remains intact

### O4. Tap Detail And Vertical Scroll Still Work

- Action:
  - tap a notification card without swiping
  - return from detail
  - vertically scroll the notifications list
- Expected:
  - tapping still enters detail
  - vertical list scrolling still behaves normally
  - the new swipe action does not steal ordinary list browsing

### O5. Existing Notification Baselines Stay Stable

- Action:
  - test `清空`
  - press `N`
  - press `B`
- Expected:
  - clear-all confirmation still works
  - new notifications can still be injected
  - low-battery notification baseline still works

## P. Notification Stage Closure Bundle

### P1. Full Notification Closure Run

- Action:
  - on a normal watchface path, press `N`
  - verify toast behavior
  - open `Notifications`
  - verify the newest notification starts unread
  - tap it to enter detail, then return and verify it becomes read
  - test `清空` cancel / confirm
  - inject more notifications and test single-card right-swipe delete
  - from detail view, close the notifications shell with the bottom handle
  - return to watchface, then open notifications again
- Expected:
  - the whole notification chain works as one coherent system closure
  - reopening notifications after closing detail returns to the list view, not the stale detail view

### P2. Wake Preview And Existing Baselines

- Action:
  - let the watch enter `ScreenOff`
  - press `N`
  - move from wake preview into `Notifications`
  - press `B`
  - optionally re-enter and exit long-battery mode
- Expected:
  - wake preview path still works
  - low-battery notification baseline still works
  - long-battery path is not broken by the notification stage work

### P3. Long-Battery Mode Suppresses Notifications

- Action:
  - enter long-battery mode
  - let the watch enter `ScreenOff`
  - press `N`
  - try normal notifications pull / open behavior if applicable
- Expected:
  - no notification wake preview appears
  - no notifications shell is opened
  - the watch remains in the long-battery path
  - long-battery mode keeps only time / steps / NFC semantics

## Q. Health Monitoring Settings Shared Model

### Q1. Battery Optimization Toggles Still Work

- Action:
  - open `设置 -> 电池 -> 续航优化`
  - toggle each of the 5 switches once
- Expected:
  - each switch still responds immediately
  - page visuals behave the same as before this round

### Q2. Toggle State Persists Across Re-entry

- Action:
  - in `续航优化`, change one or more toggles
  - navigate back to `电池`
  - enter `续航优化` again
- Expected:
  - the changed toggles keep their latest state
  - the page no longer behaves like a one-page local state island

### Q3. Existing Battery / Notification Baselines Stay Stable

- Action:
  - after changing optimization toggles, press `B`
  - if convenient, also enter and exit long-battery mode once
- Expected:
  - low-battery notification baseline still works
  - long-battery mode path is not broken by this settings-model change

## R. Sleep Home And Settings Shell

### R1. Sleep Is No Longer A Placeholder

- Action:
  - enter `AppSleep` from Launcher or the home ring card
- Expected:
  - the page is no longer a placeholder
  - the page shows:
    - 昨晚睡眠记录占位卡
    - 近 7 天睡眠占位卡
    - `设置`
    - `说明`

### R2. Sleep Settings Path Is Reachable

- Action:
  - from `AppSleep`, open `设置`
  - enter `睡眠高精度监测`
  - return
  - enter `睡眠呼吸质量监测`
- Expected:
  - both detail pages are reachable
  - both pages show a switch and a short explanation body

### R3. Sleep Settings Reuse The Shared Model

- Action:
  - change one or both sleep settings from `AppSleep`
  - return to `设置 -> 电池 -> 续航优化`
- Expected:
  - corresponding switches remain in sync
  - `Sleep` settings and `BatteryOptimizationPage` are editing the same shared model

### R4. Sleep Info Page Shell Is Reachable

- Action:
  - from `AppSleep`, open `说明`
- Expected:
  - the info page opens
  - title shows `睡眠说明`
  - the page contains real explanatory body text instead of placeholder copy
  - touch scroll and crown scroll can browse the full body

## S. Blood Oxygen Home And Settings Shell

### S1. Blood Oxygen Is No Longer A Placeholder

- Action:
  - enter `AppBloodOxygen` from Launcher or the health shortcut surface
- Expected:
  - the page is no longer a placeholder
  - the page shows:
    - 今日血氧饱和度线图占位
    - 当前百分比占位
    - 上次更新时间占位
    - `开始测量`
    - `设置`
    - `说明`

### S2. Blood Oxygen Settings Path Is Reachable

- Action:
  - from `AppBloodOxygen`, open `设置`
  - turn `全天血氧监测` off
  - turn `全天血氧监测` on
  - open `低血氧提醒`
- Expected:
  - `血氧设置` is reachable
  - `全天血氧监测` can be switched
  - `低血氧提醒` entry is hidden when `全天血氧监测` is off
  - `低血氧提醒` entry is reachable and shows current selection text when `全天血氧监测` is on

### S3. Low Blood Oxygen Reminder Can Be Selected

- Action:
  - in `低血氧提醒`, select `不提醒 / 90% / 85% / 80%`
  - return to `血氧设置`
  - turn `全天血氧监测` off, then on again
- Expected:
  - each option can be selected
  - returning to `血氧设置` updates the status text immediately
  - re-enabling `全天血氧监测` restores the `低血氧提醒` row with the previous selection

### S4. Blood Oxygen Settings Reuse The Shared Model

- Action:
  - change `全天血氧监测` from `AppBloodOxygen`
  - return to `设置 -> 电池 -> 续航优化`
- Expected:
  - `全天血氧监测` remains in sync across both pages
  - `AppBloodOxygen` and `BatteryOptimizationPage` are editing the same shared model

### S5. Blood Oxygen Info Body Is Reachable

- Action:
  - from `AppBloodOxygen`, open `说明`
  - vertically drag / flick through the info page
  - press `Q / E`
- Expected:
  - the info page opens
  - title shows `血氧说明`
  - the page contains real explanatory body text instead of placeholder copy
  - the body includes reference values, accuracy factors, wearing guidance, and the non-medical-device note
  - touch scroll and crown scroll can browse the full body

## T. Heart Rate Home And Resting 30 Days

### T1. Heart Rate Is No Longer A Placeholder

- Action:
  - enter `AppHeartRate` from Launcher or the health shortcut surface
- Expected:
  - the page is no longer a placeholder
  - title shows `心率`
  - the first visible state is `正在测量 / 请保持静止`
  - this round still uses a fixed mock measurement result, not a real heart-rate algorithm

### T2. Entry Measurement Ends In Result And Wear Prompt

- Action:
  - stay on `AppHeartRate` until the mock measurement completes
  - when the wear prompt appears, tap `知道了`
- Expected:
  - the prompt shows `1.5cm-2cm` and `请正确佩戴手表后重试`
  - tapping `知道了` dismisses the prompt
  - the result area shows `未佩戴`, `76 次/分`, and `76次/分(1分钟前)`
  - the result and measuring states occupy the same main page position

### T3. Heart Rate Main Entries

- Action:
  - after dismissing the wear prompt, scroll the Heart Rate page by touch and by `Q / E`
- Expected:
  - entries show:
    - `近30天静息`
    - `心率设置`
    - `心率说明`
  - `近30天静息`, `心率设置`, and `心率说明` open real pages

### T4. Resting 30 Days Page Is Reachable

- Action:
  - from `AppHeartRate`, open `近30天静息`
  - vertically drag / flick through the page
  - press `Q / E`
  - tap the top-left back button
- Expected:
  - the page title shows `近30天静息`
  - the chart card shows a mock 30-day resting heart-rate line
  - visible labels include `4/26`, `5/10`, `5/25`, `-- 次/分`, and `今日静息`
  - touch scroll and crown scroll keep working
  - the back button returns to `AppHeartRate`

### T5. Heart Rate Settings Page Is Reachable

- Action:
  - from `AppHeartRate`, open `心率设置`
  - scroll the page by touch and by `Q / E`
- Expected:
  - the page title shows `心率设置`
  - visible rows include:
    - `全天心率监测`
    - `心脏健康监测`
    - `高心率提醒`
    - `低心率提醒`
  - default status includes `智能监测`, `开启`, `关闭`, `关闭`

### T6. All-Day Heart Rate Monitoring Can Be Selected

- Action:
  - from `心率设置`, open `全天心率监测`
  - select `关闭 / 智能监测 / 1分钟一次 / 10分钟一次 / 30分钟一次`
  - return to `心率设置`
- Expected:
  - the selected option is highlighted
  - returning to `心率设置` updates the `全天心率监测` status text immediately

### T7. Heart Health Monitoring Reuses Shared Model

- Action:
  - from `心率设置`, open `心脏健康监测`
  - toggle `心脏健康监测`
  - return to `心率设置`
  - compare with `设置 -> 电池 -> 续航优化`
- Expected:
  - the switch visual updates immediately
  - `心率设置` and `续航优化` show the same `心脏健康监测` state

### T8. High And Low Heart Rate Reminders Can Be Selected

- Action:
  - from `心率设置`, open `高心率提醒`
  - select `关闭 / 100次/分 / 110次/分 / 120次/分 / 130次/分 / 140次/分 / 150次/分`
  - return to `心率设置`
  - open `低心率提醒`
  - select `关闭 / 40次/分 / 45次/分 / 50次/分`
  - return to `心率设置`
- Expected:
  - each selected option is highlighted in its option page
  - returning to `心率设置` updates `高心率提醒` and `低心率提醒` status text immediately
  - this round only stores thresholds and does not trigger alert notifications

### T9. Heart Rate Info Body Is Reachable

- Action:
  - from `AppHeartRate`, open `心率说明`
  - vertically drag / flick through the page
  - press `Q / E`
  - tap the top-left back button
- Expected:
  - the page title shows `心率说明`
  - visible body text includes:
    - `心率`
    - `55-80次/分`
    - `30-240次/分`
    - `测量说明`
    - `1.5cm-2cm`
    - `本品非医疗器械`
  - touch scroll and crown scroll keep working
  - the back button returns to `AppHeartRate`

## Exit Criteria

The current simulator baseline can be considered stable enough for a new round when:

- all sections above pass without obvious regression
- all long-battery entry / exit paths pass without page-jump regressions
- no display-policy state saves to the wrong page
- no wake / screen-off path jumps to an unexpected page
- no quick-settings control drifts away from settings-page state
- no screen-off style selection breaks swipe or hit-testing
