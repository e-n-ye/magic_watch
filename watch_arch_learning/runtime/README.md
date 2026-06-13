# Runtime

本目录只负责提供稳定的 PC LVGL + SDL 运行环境：

- 初始化 LVGL。
- 创建 SDL 窗口和鼠标输入。
- 驱动 `lv_timer_handler()`。

它不知道 Heart、Steps、页面导航或任何 Magic Watch 产品语义。课程期间除非运行环境本身有问题，否则不随 Stage 演化。

顶层 `lv_conf.h` 使用独立最小配置，只启用 Stage 00 需要的 SDL 窗口、鼠标和基础控件，避免把 FS、ThorVG、示例或 demo 带入实验场。
