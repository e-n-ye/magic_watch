# F411 XML 兼容性探针

本文档记录 `V0.3-F411-XML-PROBE` 的只读探针结果。目标不是让 F411 直接跑通 XML 页面，而是在进入 `V0.4` 之前，先判断生成 C、资源路径和构建依赖是否落在 F411 可接受范围内。

## 当前结论状态

- `V0.3-A`：已完成
- `V0.3-B`：已完成
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

## V0.3-B LVGL 9.6 到 8.2 不兼容点评估

### 对照基线

- F411 当前 LVGL 版本：`8.2.0`
  - 证据：`try/my_watch_f411_v2.1/user/third_party/lvgl/lvgl/lvgl.h`
- F411 当前关键配置：
  - `LV_USE_BTN = 0`
  - `LV_USE_IMG = 0`
  - `LV_USE_LABEL = 1`
  - `LV_USE_MENU = 0`
  - `LV_USE_FS_STDIO = 0`
  - `LV_USE_FS_POSIX = 0`
  - `LV_USE_FS_WIN32 = 0`
  - `LV_USE_FS_FATFS = 0`
  - `LV_USE_PNG = 0`
  - 证据：`try/my_watch_f411_v2.1/user/third_party/lvgl/lv_conf.h`

### 兼容分类

#### 同名存在，可视为低风险兼容

- 类型/事件：
  - `lv_event_t`
  - `lv_event_get_user_data(...)`
- 工具：
  - `lv_snprintf(...)`
  - `lv_pct(...)`
  - `lv_color_hex(...)`
- 对象基础：
  - `lv_obj_create(...)`
  - `lv_obj_get_child(...)`
  - `lv_obj_set_x(...)`
  - `lv_obj_set_y(...)`
  - `lv_obj_set_width(...)`
  - `lv_obj_set_height(...)`
  - `lv_obj_set_align(...)`
  - `lv_obj_add_style(...)`
  - `lv_obj_remove_style_all(...)`
  - `lv_obj_set_style_bg_color(...)`
  - `lv_obj_set_style_text_color(...)`
  - `lv_obj_set_style_radius(...)`
- 样式：
  - `lv_style_init(...)`
  - `lv_style_set_width(...)`
  - `lv_style_set_height(...)`
  - `lv_style_set_pad_all(...)`
  - `lv_style_set_radius(...)`
  - `lv_style_set_bg_opa(...)`
  - `lv_style_set_bg_color(...)`
  - `lv_style_set_text_color(...)`
- 文本：
  - `lv_label_create(...)`
  - `lv_label_set_text(...)`
- 枚举/宏：
  - `LV_OBJ_FLAG_SCROLLABLE`
  - `LV_ALIGN_BOTTOM_LEFT`

#### 可通过小 shim 或重命名过渡，但不是直接兼容

- `lv_button_create(...)`
  - F411 8.2 中对应旧名是 `lv_btn_create(...)`
  - 额外风险：当前配置 `LV_USE_BTN = 0`，即使改成旧名也不会直接可用
- `lv_image_create(...)`
  - F411 8.2 中对应旧名是 `lv_img_create(...)`
  - 额外风险：当前配置 `LV_USE_IMG = 0`
- `lv_image_set_src(...)`
  - F411 8.2 中对应旧名是 `lv_img_set_src(...)`
  - 额外风险：当前配置 `LV_USE_IMG = 0`
- `lv_obj_set_flag(obj, flag, false)`
  - F411 8.2 没有同名三参入口，但有 `lv_obj_add_flag(...)` / `lv_obj_clear_flag(...)`
  - 说明：语义可拆解，不是根本阻塞

#### 当前 F411 8.2 中未发现直接对应入口，属于高风险不兼容

- `LV_USE_XML`
- `#include "lv_xml/lv_xml.h"`
- `lv_xml_register_image(...)`
- `lv_xml_create(...)`
- `lv_obj_set_name_static(...)`
- `lv_strdup(...)`
- `LV_EDITOR_PREVIEW`

### 当前判断

- 这不是“只有少量 9.x 命名差异”的情况。
- 主要缺口分三层：
  - XML 运行时体系缺失：F411 当前 8.2 代码树中没有 `lv_xml/*` 入口。
  - 控件命名和配置双重不匹配：生成代码用 `lv_button_*` / `lv_image_*`，而 F411 当前既是旧名 `lv_btn_*` / `lv_img_*`，又把 `BTN` / `IMG` 组件关掉了。
  - 少量辅助 API 缺失：如 `lv_obj_set_name_static(...)`、`lv_strdup(...)`，理论上可单独补薄 shim，但它们不是当前最大问题。

### 处理建议

- `同名存在` 项：
  - 可直接视为后续可复用基础，不构成主要阻塞。
- `可 shim / 重命名` 项：
  - 仅在确认整体路线仍值得继续后再评估，不建议现在为单个 API 提前补兼容宏。
- `高风险不兼容` 项：
  - 不建议靠手改生成 C 硬接到 F411 8.2。
  - 若后续还想走 XML 生成路线，更合理的候选是：
    - 重生成为更接近 LVGL 8.2 / 无 XML 运行时依赖的产物
    - 或把 F411 目标改成纯 C 数组资源 + 手写轻量页面装配

### V0.3-B 小结

- 当前证据支持把 `XML 运行时依赖` 记为一级风险。
- 当前证据支持把 `9.6 生成代码直接接 8.2` 记为不可直接假设成立。
- 还不能在本卡直接下“必须止损”最终结论，因为 `V0.3-C` 还需要把图片、字体、Flash/RAM 成本一起纳入判断。
