# F411 XML 兼容性探针

本文档记录 `V0.3-F411-XML-PROBE` 的只读探针结果。目标不是让 F411 直接跑通 XML 页面，而是在进入 `V0.4` 之前，先判断生成 C、资源路径和构建依赖是否落在 F411 可接受范围内。

## 当前结论状态

- `V0.3-A`：已完成
- `V0.3-B`：未开始
- `V0.3-C`：未开始
- `V0.3-D`：未开始

## V0.3-A 生成 C 的 LVGL API 清单

### 扫描范围

- `ui/lvgl_pro/magic_watch_ui_gen.c`
- `ui/lvgl_pro/magic_watch_ui_gen.h`
- `ui/lvgl_pro/magic_watch_ui.c`
- `ui/lvgl_pro/components/health_shortcut_card_gen.c`
- `ui/lvgl_pro/screens/screen_health_shortcuts_gen.c`
- `ui/lvgl_pro/**/*.h`

### 直接依赖的 LVGL/XML 入口

- XML 相关：
  - `LV_USE_XML`
  - `#include "lv_xml/lv_xml.h"`
  - `lv_xml_register_image(...)`
- 事件相关：
  - `lv_event_t`
  - `lv_event_get_user_data(...)`
- 字符串/工具：
  - `lv_snprintf(...)`
  - `lv_strdup(...)`
  - `lv_pct(...)`
  - `lv_color_hex(...)`

### 直接创建的对象类型

- `lv_obj_create(NULL)`：屏幕根对象、分页点
- `lv_button_create(parent)`：健康卡片按钮容器
- `lv_image_create(parent)`：卡片图标
- `lv_label_create(parent)`：卡片指标文本

### 直接使用的对象操作 API

- 命名和树访问：
  - `lv_obj_set_name_static(...)`
  - `lv_obj_get_child(...)`
- 位置尺寸：
  - `lv_obj_set_x(...)`
  - `lv_obj_set_y(...)`
  - `lv_obj_set_width(...)`
  - `lv_obj_set_height(...)`
  - `lv_obj_set_align(...)`
- 样式和标志：
  - `lv_obj_set_flag(...)`
  - `lv_obj_remove_style_all(...)`
  - `lv_obj_add_style(...)`
  - `lv_obj_set_style_bg_color(...)`
  - `lv_obj_set_style_text_color(...)`
  - `lv_obj_set_style_radius(...)`

### 直接使用的控件 API

- 图片：
  - `lv_image_set_src(...)`
- 文本：
  - `lv_label_set_text(...)`

### 直接使用的 style API

- `lv_style_init(...)`
- `lv_style_set_width(...)`
- `lv_style_set_height(...)`
- `lv_style_set_pad_all(...)`
- `lv_style_set_radius(...)`
- `lv_style_set_bg_opa(...)`
- `lv_style_set_bg_color(...)`
- `lv_style_set_text_color(...)`

### 直接使用的枚举/宏

- `LV_OBJ_FLAG_SCROLLABLE`
- `LV_ALIGN_BOTTOM_LEFT`
- `LV_EDITOR_PREVIEW`

### 当前可见的生成特征

- 生成代码不是纯“无 XML 依赖”的老式静态 C 页面；头文件和初始化流程显式依赖 `LV_USE_XML` 与 `lv_xml/lv_xml.h`。
- 图片资源不是 C 数组，而是通过 `asset_path + "images/*.png"` 组装字符串，再交给 `lv_xml_register_image(...)` 注册。
- `globals.xml` 当前包含 `<subjects>` 区块，但扫描结果里没有看到实际生成的 subject 注册代码；这一点需要在 `V0.3-B` 继续核实，当前只能记为“接口预留存在，实用例未出现”。
- 生成代码使用 `lv_button_*`、`lv_image_*` 这类命名；这对后续 `LVGL 9.6 -> 8.2` 兼容判断是高优先级检查项。

### V0.3-A 小结

- 已拿到可服务 `V0.3-B` 的直接 API 清单。
- 当前已经能确认两类高风险入口：
  - XML 运行时依赖
  - PNG 文件路径图片依赖
- 还不能据此判断 F411 可编译；版本兼容和资源预算要在后续卡片继续验证。
