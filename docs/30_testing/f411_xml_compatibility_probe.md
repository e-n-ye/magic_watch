# F411 XML 兼容性探针

本文档记录 `V0.3-F411-XML-PROBE` 的只读探针结果。目标不是让 F411 直接跑通 XML 页面，而是在进入 `V0.4` 之前，先判断生成 C、资源路径和构建依赖是否落在 F411 可接受范围内。

## 当前结论状态

- `V0.3-A`：已完成
- `V0.3-B`：已完成
- `V0.3-C`：已完成
- `V0.3-D`：已完成

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

## V0.3-C 图片、字体、Flash/RAM 资源成本

### 当前资源入口

- `globals.xml` 当前登记了 12 张图片资源：
  - `health_heart`
  - `health_heart_36`
  - `health_heart_64`
  - `health_spo2`
  - `health_spo2_36`
  - `health_spo2_64`
  - `health_breathe`
  - `health_breathe_36`
  - `health_breathe_64`
  - `health_stress`
  - `health_stress_36`
  - `health_stress_64`
- `globals.xml` 里的 `<fonts>` 区块当前为空，没有看到额外 XML 自定义字体资源。
- `magic_watch_ui_gen.c` 当前通过 `asset_path + "images/*.png"` 组装文件路径，再调用 `lv_xml_register_image(...)` 注册。

### 已验证的图片像素尺寸

通过 PNG 头只读检查，当前 4 组资源的尺寸模式一致：

- 无后缀版本：`256 x 256`
- `_64` 版本：`64 x 64`
- `_36` 版本：`36 x 36`

可据此推导：

- 单组图片总像素数：
  - `256 x 256 = 65536 px`
  - `64 x 64 = 4096 px`
  - `36 x 36 = 1296 px`
  - 合计 `70928 px`
- 4 组图片总像素数：
  - `70928 x 4 = 283712 px`

### 若转成 RGB565 C 数组的 Flash 上界

按 `2 bytes/pixel` 粗估：

- 单张 `256 x 256` 图片：
  - `65536 x 2 = 131072 B`，约 `128 KB`
- 单组 3 张图片：
  - `70928 x 2 = 141856 B`，约 `138.5 KB`
- 当前 12 张图片全部转成 RGB565：
  - `283712 x 2 = 567424 B`，约 `554 KB`

这只是像素数据上界，还没算：

- `lv_img_dsc_t` 元数据
- 对齐/链接开销
- 代码段和其它只读资源

### 与 F411 当前预算的直接冲突

- F411 当前 `LV_MEM_SIZE = 32 KB`
- 当前 `LV_IMG_CACHE_DEF_SIZE = 0`
- 当前 `LV_USE_FS_STDIO/POSIX/WIN32/FATFS = 0`
- 当前 `LV_USE_PNG = 0`
- 当前 `LV_USE_BMP = 0`
- 当前 `LV_USE_SJPG = 0`
- 当前 `LV_USE_GIF = 0`
- 当前 `LV_USE_FREETYPE = 0`
- 当前 `WATCH_LVGL_DRAW_BUF_LINES = 20`
  - 单块 draw buffer：`240 x 20 x 2 = 9600 B`
  - 双 draw buffer + flush byte stream：约 `28800 B`

由此可得：

- 当前工程根本没有“直接从文件系统加载 PNG 并解码显示”的运行时路径。
- 即便先不考虑 XML，仅把一张 `256 x 256` 图标做成原始 RGB565 常量，也已经远大于当前单块 draw buffer，且接近/超过很多 F411 级别场景可接受的单资源体量。
- 若把 12 张图片全部静态转成 RGB565 C 数组，总量约 `554 KB`，这对 F411 目标来说明显偏重，不适合作为“健康四卡第一轮真机探针”的默认资源方案。

### 字体成本判断

- XML 当前没有登记额外字体文件或生成字体资源。
- F411 当前只启用了：
  - `LV_FONT_MONTSERRAT_10 = 1`
  - `LV_FONT_MONTSERRAT_14 = 1`
  - `LV_FONT_MONTSERRAT_20 = 1`
- 当前 `LV_FONT_DEFAULT = &lv_font_montserrat_14`
- `LV_FONT_FMT_TXT_LARGE = 0`
- `LV_USE_FREETYPE = 0`

当前可得结论：

- 这轮 XML 探针的主要资源风险不是“自定义字体太重”，而是图片路径和图片尺寸。
- 若后续继续沿用现有字号体系并避免新增大字库，字体侧暂时不是第一停止条件。
- 但如果后续 XML 方案引入新字体文件、CJK 字库或更大字号，Flash/RAM 风险会显著上升；这一点当前尚未发生，不能提前写成已存在问题。

### 资源路线建议

- 不建议把当前 12 张 PNG 直接原样迁入 F411 真机方案。
- 更合理的优先级是：
  - 优先删除 `256 x 256` 这组对 F411 不友好的资源层级
  - 优先保留更小尺寸的图标版本，或重出真正面向 `240 x 280` 小屏的轻量资源
  - 若必须上板，优先考虑 C 数组形式的小图标，而不是文件路径 + PNG 解码
  - 页面装配优先使用 LVGL 基础对象、色块、文本和小图标，不要把“大图资源兼容问题”伪装成“XML API 兼容问题”

### 未验证项

- 12 张 PNG 的压缩后文件字节数：本轮未直接读取，保留未验证。
- 若启用 PNG/FS 后的真实运行时峰值 RAM：本轮未实测，保留未验证。
- F411 最终可用 Flash 余量与整工程链接结果：本轮未编译验证，保留未验证。

### V0.3-C 小结

- 当前资源侧一级风险不是字体，而是图片路径依赖和 `256 x 256` 资源层级过重。
- 当前证据支持把“先做资源替换/裁剪”视为继续 F411 XML 路线的前置条件之一。
- `V0.3-D` 应把版本兼容和资源预算两类风险合并后，再给出继续或止损结论。

## V0.3-D 继续 / 止损结论

### 结论

- 本轮结论是：停止“当前这套 `LVGL 9.6 runtime XML + 文件路径 PNG 资源` 直接进入 F411 LVGL 8.2”的实现路线。
- 本轮结论不是：放弃 XML 作为 UI 源，也不是否定 PC 侧 `LVGL XML + watch_core` 主线。
- 若后续还要进入 F411 UI 真机闭环，前提应改为：
  - 不依赖 `runtime XML`
  - 不依赖 `PNG/FS` 文件路径资源
  - 不以“大面积手改 9.6 生成 C”作为默认落地方式

### 触发停止条件的证据

- 已确认 F411 当前 LVGL 为 `8.2.0`，而 PC XML 目标使用 `9.6.0-dev` 生成产物。
- 已确认生成代码直接依赖 `LV_USE_XML`、`lv_xml/lv_xml.h`、`lv_xml_register_image(...)`，这些入口在当前 F411 代码树中未发现直接对应实现。
- 已确认生成代码使用 `lv_button_*`、`lv_image_*` 新命名，而 F411 8.2 侧对应旧名为 `lv_btn_*`、`lv_img_*`，且当前配置还把 `BTN` / `IMG` 组件关掉了。
- 已确认当前资源入口是 `asset_path + images/*.png` 的文件路径注册，不是 C 数组资源。
- 已确认 F411 当前 `LV_USE_FS_* = 0`、`LV_USE_PNG/BMP/SJPG/GIF = 0`，文件系统和 PNG 解码都不是现成主路径。
- 已估算单张 `256 x 256` 图像若按 RGB565 原始像素落地约 `128KB`，12 张总量约 `554KB`，对 F411 当前真机探针目标明显偏重。

### 为什么不是“做少量 shim 后继续”

- 风险不是单点 API 缺口，而是 `runtime XML`、对象命名、组件配置、资源路径和资源体量同时不匹配。
- 即使先忽略 XML 运行时，当前 PNG 路径资源和 `256 x 256` 资源层级也已经单独触发资源侧停止条件。
- 若继续把这些问题压到 `V0.4` 实现阶段，再暴露出来时就不再是“探针”，而会变成高耦合返工。

### 推荐替代路线

- 保留 `LVGL XML + watch_core + PC` 作为 UI 设计和语义验证主线。
- F411 侧改为“轻量真机页面装配”候选：
  - 优先复用 `watch_core` 的页面语义和事件链
  - UI 资源优先小图标、色块、文本，避免大 PNG 路径资源
  - 若保留 XML 参与流程，应以“可被 F411 消费的轻量导出产物”为目标，而不是把现有 runtime XML 产物直接搬过去

### 未验证项保留

- 12 张 PNG 的压缩后文件字节数：未直接读取。
- 若强行开启 `PNG/FS` 后的真实峰值 RAM：未实测。
- 整工程最终链接后的 Flash 余量：未编译验证。
- 是否能通过重生成或另一路导出把 XML 收敛成 F411 可接受产物：本轮未做生成器层验证。

### V0.3-D 小结

- `V0.3` 已完成其探针目标，并给出“当前 F411 XML 直接上板路线应止损”的结论。
- 下一步不应直接进入原定义的 `V0.4 F411 XML 真机最小闭环`。
- 更合理的后续是先重评 F411 UI 生成路线，再决定是否新建一组替代 `V0.4` 卡片。
