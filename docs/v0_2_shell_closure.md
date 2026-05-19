# Magic Watch v0.2 Shell Closure

Date: 2026-05-19

## Why This Closure Exists

The project is no longer in the "startup only" phase.
The simulator has already grown a coherent v0 shell with a usable interaction skeleton, display policy, settings detail flow, quick settings linkage, and a real screen-off path.

At this point, the highest-value work is not adding more business pages.
The highest-value work is freezing a stable, explainable, regressable baseline before the code surface grows further.

This document is that baseline note for the current v0.2 shell.

## Current Stage Judgment

Current stage:

- still a simulator-first shell prototype
- no longer only a "baseline startup scaffold"
- already in the late half of the v0 shell prototype stage
- display policy and screen-off behavior are now part of the main path, not a side experiment

That means future work should be judged by:

- whether it makes the shell easier to explain
- whether it reduces regression risk
- whether it preserves room for later hardware migration

instead of only by whether it adds more visible pages.

## Current Main Visible Path

The current v0.2 visible shell should be understood as:

1. Watchface
2. Home ring shortcut surfaces
3. Messages shell
4. Quick Settings shell
5. Launcher list
6. Settings home and display-related detail pages
7. Power menu
8. Screen-off / wake path

These surfaces now form one connected shell story.

## Current Display And Screen-Off Baseline

The display-related main path has already grown into a real subsystem prototype.

### Settings Home

`显示与亮度` currently includes these visible entries:

- 屏幕亮度
- 息屏时间
- 抬腕亮屏
- 单击亮屏
- 遮盖息屏
- 息屏显示
- 默认息屏样式
- 持续亮屏

The page order matters because it now reflects the current product understanding:

- active wake behaviors first
- then screen-off behavior
- then AOD fallback style
- then temporary keep-screen-on behavior

### Policies Already Landed

`DisplayPolicyModel` currently owns at least these user-visible policy dimensions:

- crown press wake
- notification wake
- raise to wake mode and scheduled window
- tap to wake
- cover to sleep
- screen-off display mode and scheduled window
- default screen-off style
- auto screen-off timeout
- temporary keep-screen-on duration
- brightness mode and manual brightness level

This is enough to treat display policy as a real v0 shell concern.

### Behaviors Already Closed

The following closed loops already exist in code:

- auto brightness and manual brightness
- manual brightness vertical slider page
- screen-off timeout selection
- raise-to-wake off / all-day / scheduled
- raise-to-wake scheduled start and end time editing
- scheduled time editing uses explicit confirm-save semantics
- tap to wake toggle
- cover to sleep toggle
- simulator `SimCoverSleep`
- keep-screen-on duration selection
- battery confirmation before enabling non-off keep-screen-on
- keep-screen-on suppresses auto screen-off while active
- screen-off display off / smart / scheduled
- battery confirmation before enabling smart or scheduled screen-off display
- screen-off display scheduled start and end time editing
- default screen-off style selection with two current fallback styles
- conflict prompt between raise-to-wake and screen-off display
- restoring the pre-screen-off page after wake
- restoring overlay / modal-like pages after wake
- real screen-off rendering instead of pure black placeholder

### Quick Settings Alignment

Quick Settings is already aligned with the current display-policy semantics for these three controls:

- 翻腕亮屏:
  - tap toggles
  - long press enters detail page
- 设置:
  - tap enters settings home
- 持续亮屏:
  - when off, tap enables 5 minutes
  - when on, tap disables
  - long press enters detail page

Quick Settings top toast feedback has also been landed.

### Current Screen-Off Rendering Baseline

The screen-off page is no longer only a near-black simulator state.
It now has a real display branch:

- when `息屏显示 = 关闭`, screen-off stays visually off
- when `息屏显示 = 智能开启` or an active `定时开启` window applies, the screen-off page renders the chosen fallback style

Current fallback styles:

- `时分指针`
- `日期数字`

Important current boundary:

- there is not yet a true "watchface-specific matching screen-off style" system
- current behavior still uses the chosen default fallback style
- this is intentional for now because the project does not yet have multiple real watchface families

## Current Simulator Input Contract

The current shell should be tested with this simulator input contract:

- `Enter` or `Space`: crown press
- `Q / E`: crown rotate
- `R`: simulator raise to wake
- `F`: simulator raise dismiss
- `C`: simulator cover to sleep
- touch drag and flick:
  - home ring horizontal movement
  - launcher list scroll
  - settings list scroll
  - shell sheet open / close drag
- left-edge right-swipe:
  - back navigation

## What Is Stable Enough To Freeze

The following are stable enough to treat as the current v0.2 baseline:

- shell navigation skeleton
- display policy model ownership
- settings display detail flow
- quick-settings linkage for display behaviors
- screen-off rendering fallback behavior
- screen-off -> wake -> restore previous page path
- simulator input contract for wake / cover / crown / shell drag
- one full paired manual regression pass has been executed against this shell baseline

## Regression Status

The current v0.2 shell is no longer only "documented in theory".
It has now gone through one real manual regression run and the issues found in that run were closed in follow-up small rounds.

Regression run artifact:

- `docs/simulator_manual_regression_run_2026_05_19.md`

Issues found and closed during that run included:

- home-ring `Q / E` multi-page overshoot
- `Messages -> return -> Quick Settings` backdrop offset residue
- manual brightness slider accidental value changes during left-edge back
- raise-to-wake scheduled-card selection-dot layout inconsistency

## What Is Intentionally Not In This Closure

This closure does not claim the following are done:

- payments, weather, health, or other real business services
- real hardware power management
- true low-power display pipeline
- watchface-specific paired AOD / screen-off style system
- editable default screen-off style internals
- final visual polish for every shell page

These are later-stage topics.
They should not be mixed into the current closure.

## Current Known Debt

The shell is now coherent enough to freeze, but a few debts should be kept visible:

- some historical documents still describe earlier unfinished rounds and should be treated as superseded by this closure
- display-policy decisions are still distributed between `AppStateMachine`, settings pages, and screen-off rendering
- there is still room to later extract a lighter `DisplayPolicyCoordinator` once the behavior is stable enough
- the project still relies on simulator semantics for wake and screen-off rather than hardware-backed power states
- the simulator `Q / E` crown behavior is still only an approximation of a real rotary encoder, even though the current shell baseline is usable

## Recommended Next Step

The most suitable next step after this closure is:

`Build and maintain a simulator manual regression matrix for the current shell baseline.`

Reason:

- the shell is now rich enough to regress silently
- future changes in quick settings, wake policy, or page restoration can easily break one another
- a stable matrix now is more valuable than more placeholder feature pages

For that matrix, see:

- `docs/simulator_manual_regression_matrix.md`
