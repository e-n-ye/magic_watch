# Weather Icon Workflow

## Intent

When a home-ring surface already has a clear structural direction, we should stop faking icons with temporary code shapes and start treating key icons as visual assets with their own generation workflow.

For the current Magic Watch v0 shell, the first concrete asset target is:

- Weather page top-right partly-cloudy icon

## Why This Matters

- The current page structure is already simple enough that the weather icon is becoming a visible quality bottleneck.
- A generated icon gives us a cleaner visual reference than stacking circles and rounded boxes in code.
- This also turns "use `gpt-image-2` for concrete small assets" into a repeatable repo workflow instead of a chat-only idea.

## Current Prompt Entry

Prompt file:

- `.agents/skills/watch-visual-workflow/assets/prompts/weather-icon-cloud-sun.txt`

## Recommended Command

```powershell
C:\Users\13984\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe `
  .agents\skills\watch-visual-workflow\scripts\right_code_generate_image.py `
  --prompt-file .agents\skills\watch-visual-workflow\assets\prompts\weather-icon-cloud-sun.txt `
  --topic weather-icon-cloud-sun `
  --response-format b64_json `
  --size 1024x1024
```

## Output Convention

Generated output should live under:

- `.agents/generated/weather-icon-cloud-sun/`

Current expected stable outputs:

- `latest.json`
- `latest.png`

Review each round for:

1. silhouette readability at small size
2. wearable-OS feel instead of emoji feel
3. whether the icon still reads clearly inside a compact weather hero card

## Next Decision After Generation

After a good round exists, choose one of three paths:

1. keep it as a visual reference only
2. convert it into a simulator bitmap asset
3. simplify it into a code-native geometric icon language for v0

## 2026-05-14 First Run Note

First successful local output now exists under:

- `.agents/generated/weather-icon-cloud-sun/latest.png`
- `.agents/generated/weather-icon-cloud-sun/latest.json`

Current judgment:

- the generation workflow is now live, not just scaffolded
- this icon should first be used in the Web visual sandbox as a reference asset
- the next review should focus on small-size silhouette readability after downscaling
- do not rush to integrate it into LVGL runtime until the page-level visual spec is stable
