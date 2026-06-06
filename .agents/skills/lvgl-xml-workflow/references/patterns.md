# LVGL XML Patterns Reference

Use this reference when editing Magic Watch LVGL XML. It records local evidence from the bundled LVGL Editor docs, examples, and tutorials.

## Source Map

- Syntax overview: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\docs\syntax\overview.mdx`
- Styles: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\docs\syntax\styles.mdx`
- Images: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\docs\syntax\images.mdx`
- Components: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\docs\syntax\components.mdx`
- Events: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\docs\syntax\events.mdx`
- Small component tutorial: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\tutorials\2_new_component`
- Layout tutorial: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\tutorials\5_layouts`
- Assets tutorial: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\tutorials\3_assets`
- Comprehensive component examples: `D:\MY_Desk\watch\exam\lvgl_xml\lvgl_editor-master\examples\components`

## Component Ownership

Evidence:

- `tutorials\2_new_component\screens\screen_components.xml` instantiates `section`, `button_normal`, and `button_warning`; it does not duplicate their internal labels or styles.
- `tutorials\2_new_component\components\buttons\button_normal.xml` owns the button label, base style, pressed style, and `remove_style_all`.
- `examples\components\cards\move_goal\move_goal.xml` owns the full card content and uses nested `row`, `column`, and `stat_col` components.

Rules:

- Build a component around an internal visual contract: size, radius, padding, icon/text placement, scroll behavior, and child widgets.
- Let the screen instantiate components and provide per-instance values such as text, image names, subjects, IDs, and simple style overrides.
- If a screen needs to know too much about a component's internal icon or label geometry, the component API is incomplete.

## Layout Components

Evidence:

- `tutorials\5_layouts\components\row\row.xml` wraps `layout="flex"` and `flex_flow="row"`.
- `tutorials\5_layouts\components\column\column.xml` wraps `layout="flex"` and `flex_flow="column"`.
- `tutorials\5_layouts\screens\screen_layouts.xml` composes `row` and `column` instead of hand-authoring every child relationship.

Rules:

- Prefer small container components (`row`, `column`, domain-specific grid/stage components) over repeated screen-level layout fragments.
- Use absolute `x`/`y` only for page-level placement or intentionally floating elements. Keep repeated internal positions inside components.
- Use `ignore_layout="true"` only when a child intentionally escapes the active layout.

## Styles And Dynamic Values

Evidence:

- `docs\syntax\styles.mdx` says API properties cannot be used inside `<styles><style .../></styles>` because style sheets are initialized once.
- The same doc allows API properties on local style attributes such as `<view style_bg_color="$bg_color">`.
- `tutorials\2_new_component\components\buttons\button_warning.xml` extends `button_normal` and sets `style_bg_color="#yellow"` on the component instance. Its generated C applies the override after `button_normal_create(...)`.

Rules:

- Good: `<lv_label text="$label_text"/>`
- Good: `<lv_image src="$icon_src"/>`
- Good: `<view style_bg_color="$bg_color">` when the generated order is known or no later `remove_style_all` clears it.
- Good: `<health_shortcut_card style_bg_color="0xbf4c4c" />` if the component root exposes a styled object.
- Bad: `<style name="style_card" bg_color="$card_bg"/>`
- Risky: root local style attributes combined with a child `<remove_style_all />` unless generated C confirms the local style is applied after style removal.

Preferred pattern for per-instance root colors:

```xml
<component>
  <styles>
    <style name="style_base" bg_opa="100%" bg_color="#card_dark" />
  </styles>
  <view extends="lv_button">
    <remove_style_all />
    <style name="style_base" />
  </view>
</component>

<health_shortcut_card style_bg_color="0xbf4c4c" />
```

## Images

Evidence:

- `docs\syntax\images.mdx` says Editor images must be registered in `globals.xml` under `<images>`.
- `src_path` is relative to the project root containing `project.xml`.
- `tutorials\3_assets\globals.xml` registers both `<data>` and `<file>` images.
- `tutorials\3_assets\screens\screen_assets.xml` uses `<lv_image src="flower_data" />` and `<lv_image src="flower_file" />`.
- `examples\components\basic\icon_button\icon_button.xml` exposes `<prop name="icon" type="image" default="icon_plus" />` and forwards it as `<lv_image src="$icon" align="center">`.

Rules:

- Confirm the file exists under the LVGL XML project root, then register it in `globals.xml`.
- Prefer simple direct image proof before using it inside a complex card: `<lv_image src="health_heart" />`.
- For component APIs, use `type="image"` and pass names, not file paths.
- If the icon does not render, verify registration, file extension, case-sensitive name matching, and generated code before changing layout.

## Events

Evidence:

- `docs\syntax\events.mdx` says `<event_cb callback="...">` assumes a C function with the exact LVGL callback signature exists.
- The developer must implement that function. Missing callbacks can break or spam Preview.
- `examples\components\cards\move_goal\move_goal.xml` uses `subject_increment_event` for pure XML interaction.

Rules:

- In preview-only XML cards, avoid custom C callbacks until the callback is implemented and visible to the Editor preview build.
- Use subjects for simple state changes when the task is still inside XML.
- For later app integration, keep stable IDs or subjects in the component API, then wire callbacks in the integration card.

## Magic Watch Health Card Rebuild Checklist

Use this before continuing `LVGL-XML-Q1-2`:

- Define `health_shortcut_card` so it owns card size, radius, padding, icon placement, metric placement, and `scrollable="false"`.
- Let the screen provide `icon_src`, `metric_text`, `card_id`, and direct style overrides such as `style_bg_color`.
- Do not require the screen to pass per-card `icon_x`, `icon_y`, or internal label coordinates unless the user is intentionally tuning an experimental layout.
- Prove image rendering with a minimal `lv_image` or component preview before judging card layout.
- Prove manual color adjustment with `style_bg_color` on a component instance or a small variant component before exposing a custom `card_bg` prop.
- Keep the interaction boundary as `card_id` or a subject until `Q1-4`; do not bind custom C callbacks during the preview-only card.
