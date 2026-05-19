# Magic Watch v0 Shell Baseline

Date: 2026-05-14

This document is the clean re-entry point for the current simulator baseline.
If a future session only has time to read one status file, read this one first.

## Why This Baseline Exists

We now have the first simulator build that is coherent enough to freeze as a baseline:

- the home ring can be exercised
- crown simulation is working
- top / bottom shell entry is aligned with the intended watch model
- launcher is now a flowing list instead of a leftover grid
- old experimental app content is still kept in code, but it is no longer the main visible path

The goal of this baseline is not product completeness.
The goal is to preserve the first explainable, testable, top-down interaction shell.

## Current Visible Entry Set

The current v0 visible path should be understood as:

1. Watchface
2. Four home shortcut surfaces
3. Messages shell
4. Quick Settings shell
5. Launcher list
6. Settings list and its placeholder detail pages
7. Power menu
8. Screen-off / wake path

These are the surfaces that define the current product shell.

## Hidden But Retained Legacy Content

The following content is intentionally still present in code, but should not be treated as the current v0 main path:

- GPS
- Recorder
- Audio Player
- Video Player
- Infrared
- Pedometer
- LoRa
- Timing tools home
- Games home
- 2048
- Calculator

They are kept as retained assets or future placeholders, not as the current interaction story.

## Behaviors Verified So Far

The following behaviors have already been verified in the simulator:

- `Enter` can be used as crown press
- `Q / E` can now be used for crown rotation
- horizontal swipe on the home ring is easier to trigger than before
- watchface top-down entry opens Messages
- watchface bottom-up entry opens Quick Settings
- Launcher list supports touch drag and inertial scrolling
- Settings list supports touch drag and inertial scrolling

## Current Boundaries

This is still a shell prototype.

That means:

- the four shortcut surfaces are still placeholder surfaces, not real feature pages
- Messages is still mock data, not phone sync
- Quick Settings is still a shell, not real device configuration
- display wake policy is not yet modeled as a dedicated module
- launcher visual design is still functional, not final

## Recommended Next Step After This Baseline

After this baseline commit, the most suitable next task is:

`Build a reusable HomeShortcutPage template and unify the visual / structural language of the watchface-centered home ring.`

This is a better next step than adding more business features because:

- the interaction shell now exists
- the next missing piece is surface quality and structural consistency
- the home ring still uses generic placeholders, which blocks meaningful product refinement

## Status Note

This document freezes the first coherent shell baseline from 2026-05-14.

The project has since moved forward.
For the current late-shell-stage closure focused on display policy, settings, quick settings, and screen-off behavior, read:

- `docs/v0_2_shell_closure.md`
- `docs/simulator_manual_regression_matrix.md`

## Git Meaning Of This Baseline

This baseline commit should be treated as:

- the first stable simulator interaction checkpoint
- the point after which future changes can be compared incrementally
- the end of the "from scattered simulator remnants to coherent v0 shell" stage
