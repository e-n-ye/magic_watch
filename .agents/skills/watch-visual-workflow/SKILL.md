---
name: watch-visual-workflow
description: Generate and analyze Magic Watch visual assets with the project-scoped Right Code image and chat APIs. Use when Codex needs to create watchface artwork, home shortcut page visuals, quick-settings mood boards, prompt packs, or when Codex needs to analyze reference screenshots/images and turn them into structured UI guidance for this project.
---

# Watch Visual Workflow

Use this skill when Magic Watch needs image-assisted design work, not just layout code.

Keep the repo as the long-term memory:

- store reusable prompt patterns in this skill
- store generated output outside git in `.agents/generated/`
- store final decisions and selected directions back into `docs/`

## Workflow

1. Read the current product intent first:
   - `docs/home_interaction_model.md`
   - `docs/v0_scope.md`
   - `docs/v0_shell_baseline.md`
2. Decide whether the task is:
   - `generate`: create new bitmap references for watchface / shortcut pages
   - `analyze`: read screenshots or inspiration images into structured UI notes
   - `refine`: use generated or reference images to tighten prompt language before the next round
3. Keep the output tied to one surface at a time:
   - watchface
   - one home shortcut page
   - quick settings
   - launcher
4. Save generated images and JSON responses under:
   - `.agents/generated/<topic>/`
5. Summarize the chosen direction back into project docs instead of leaving the decision only in chat.

## Generate Images

Use:

- `scripts/right_code_generate_image.py`

Default config file:

- `.agents/config/right_code.env`

Typical command:

```powershell
C:\Users\13984\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe `
  .agents\skills\watch-visual-workflow\scripts\right_code_generate_image.py `
  --prompt-file .agents\skills\watch-visual-workflow\assets\prompts\home-shortcut-weather.txt `
  --topic home-shortcut-weather `
  --size 1024x1024
```

Use `--reference` multiple times when a prompt needs inspiration images.

## Analyze Reference Images

Use:

- `scripts/right_code_chat.py`

This is for turning screenshots or product photos into structured design notes.

Typical command:

```powershell
C:\Users\13984\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe `
  .agents\skills\watch-visual-workflow\scripts\right_code_chat.py `
  --prompt-file .agents\skills\watch-visual-workflow\assets\prompts\analyze-home-ring.txt `
  --image D:\MY_Desk\watch\image_temp\XiaoMiWatch\3d37ab38beff89b84dc45a3c1edbab3c.jpg
```

## Prompt Rules

Write prompts around surface intent, not vague aesthetics.

Always include:

- target surface name
- target display geometry for the current prototype
- visible hierarchy
- interaction mood
- information density
- lighting / material / palette constraints
- what must stay readable on a small round watch display

For the current Magic Watch simulator and the Xiaomi reference set, prefer:

- rounded-square display composition
- black outer breathing room
- inset card stage inside the safe area

Do not force a round-display composition when the current reference task is clearly based on a rounded-square watch shell.

Avoid asking for:

- full app screenshots with dense unreadable text
- desktop-style layouts
- random decorative gradients unrelated to the watch surface

## Project-Specific Output Standard

For every generation round, capture:

1. prompt used
2. reference images used
3. generated image URL or saved file path
4. what is reusable for Magic Watch
5. what should be rejected

If the result is promising, convert it into:

- card hierarchy notes
- palette notes
- background / foreground layering notes
- data density notes

## References

Read as needed:

- `references/right-code-api.md`
- `references/prompt-patterns.md`
