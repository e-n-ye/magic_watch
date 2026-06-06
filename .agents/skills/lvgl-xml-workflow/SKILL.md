---
name: lvgl-xml-workflow
description: Use when working on Magic Watch LVGL Pro Editor XML, especially ui/lvgl_pro screens/components/widgets/globals.xml, image registration, styles, subjects, events, preview errors, generated C, or refactoring XML module boundaries. Trigger before implementing or reviewing LVGL XML pages/cards so Codex learns from the local lvgl_editor examples/tutorials instead of guessing.
---

# LVGL XML Workflow

## Overview

Use this skill to design, repair, or review LVGL XML in `ui/lvgl_pro`. The goal is to write valid, modular LVGL Pro Editor XML from local evidence, with components owning their internal UI and screens only composing instances.

## Evidence First

Before editing XML, inspect the current files and at least one matching local reference:

- Syntax docs: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\docs\syntax\`
- Comprehensive examples: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\examples\`
- Small tutorials: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\tutorials\`

Use `rg` to find a real example before inventing syntax. If a preview is invalid, colors do not apply, or images do not render, stop guessing and compare XML plus generated `_gen.c` from the reference examples.

Load `references/patterns.md` when designing or repairing components, styles, image usage, events, or screen composition.

## Workflow

1. Read the current `project.xml`, `globals.xml`, touched component XML, touched screen XML, and generated C if present.
2. Choose the closest reference pattern from `examples` or `tutorials`; cite the file path in notes or execution records when the task is fragile.
3. Define component ownership before writing XML. The component owns internal structure, spacing, icon placement, text placement, base style, and scroll behavior. The screen owns page-level composition, high-level placement, and per-instance data/style overrides.
4. Keep data-like variation in component API props: text, image names, subjects, IDs.
5. Prefer direct widget/component attributes for standard LVGL properties such as `width`, `height`, `x`, `y`, `align`, `style_bg_color`, and `style_text_color`.
6. Validate in the cheapest order: XML syntax/Editor preview, generated code inspection if available, then downstream simulator/build when the card requires it.

## Hard Rules

- Do not put `$api_prop` values inside `<styles><style .../></styles>`. Style sheets are initialized once and cannot receive per-instance API properties.
- Pass API properties into child widget attributes or local style properties, for example `<lv_image src="$icon_src"/>` or `<view style_bg_color="$bg_color">`.
- If a root object uses `<remove_style_all />`, re-apply all required base styles immediately with `<style name="style_base" />`.
- For per-instance root color overrides, prefer setting `style_bg_color` directly on the component instance or on a variant component that extends the base component. This matches the `button_warning` tutorial pattern.
- Register every image in `globals.xml` under `<images>` before referencing it. `src_path` is relative to the LVGL XML project root that contains `project.xml`.
- Use PNG for Editor image resources unless a reference example proves another route in this project.
- Avoid adding C callbacks in XML during preview-only cards unless the callback is already implemented and visible to the Editor preview runtime. Use `subject_*_event` or defer callback wiring to the integration card when possible.
- Do not mark an XML card DONE from static inspection alone if the user-visible requirement is Editor preview behavior.

## Magic Watch Conventions

- Keep `ui/lvgl_pro/components/*.xml` as reusable pieces with clear APIs.
- Keep `ui/lvgl_pro/screens/*.xml` as page composition. A screen may instantiate four cards, but card internals must not be duplicated in the screen.
- Put shared dimensions, spacing, colors, and image registrations in `ui/lvgl_pro/globals.xml`.
- If a value is meant for a designer/user to tune per instance, prefer a direct attribute visible at the component call site, such as `style_bg_color` or `metric_text`.
- If a value is meant to preserve component geometry across all instances, keep it inside the component or a global token, not repeated on every screen instance.

## Current Known Trap

The first `LVGL-XML-Q1-2` pass produced visible but unreliable XML: card color was not user-adjustable in preview, icon rendering was not trustworthy, and screen/component responsibilities drifted. Treat that implementation as a learning artifact. Rebuild or repair it only after applying this skill's evidence-first workflow.
