# Prompt Patterns

## 1. Watchface

Use when generating a center watchface concept:

```text
Generate a smartwatch watchface concept for Magic Watch.
Surface: watchface center page.
Display geometry: follow the current prototype target. If the current reference set is Xiaomi-like, use a rounded-square safe area.
Style: modern, calm, premium, small-screen readable.
Must show: hour, minute, date, battery.
Keep hierarchy strong at 240x240 scale.
Avoid dense paragraphs, avoid fake phone UI, avoid desktop composition.
```

## 2. Home Shortcut Page

Use when generating one page in the home ring:

```text
Generate a smartwatch quick surface concept for Magic Watch.
Surface: home shortcut page for {topic}.
Display geometry: follow the current prototype target. Use rounded-square safe margins when working from the Xiaomi reference set.
Primary focus card: {focus}.
Need 3 to 4 compact metrics or quick actions.
Information density: medium.
Visual language: consistent with a premium wearable OS, not a phone screenshot.
Keep the design readable inside the target display safe area with strong edge breathing room.
```

## 3. Quick Settings

```text
Generate a smartwatch quick settings panel concept.
Display geometry: choose the current prototype target instead of assuming round hardware.
Need 4 primary toggles with clear on/off hierarchy.
Interaction tone: fast, tactile, low-cognitive-load.
Keep labels short and legible.
```

## 4. Analyze Screenshot

Use for chat image understanding:

```text
Please analyze this smartwatch screenshot for Magic Watch.
Return:
1. layout hierarchy
2. information grouping
3. likely interaction goal
4. what should be simplified for our v0 simulator
5. what is visual polish versus structural necessity
```
