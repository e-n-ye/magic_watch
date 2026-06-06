# LVGL XML Q1 PC 手动验收记录

日期：2026-06-06

目标：验证 `magic_watch_xml_sim` 是否形成健康四卡 XML、`watch_core` 事件、Coordinator 页面意图和 UI Adapter 页面切换的 PC 可交互垂直闭环。

## 运行目标

- 可执行文件：`sim/lv_port_pc_vscode/build/out/magic_watch_xml_sim.exe`
- 验收平台：PC LVGL SDL simulator
- 验收范围：健康四卡首页、占位详情页、Back 返回、点击防误触、事件日志链路

## 验收结果

- PC 窗口显示健康四卡：通过。
- 心率、血氧、呼吸、心情四卡单击进入占位详情页：通过。
- 占位详情页显示来源功能名：通过。
- Back 单击返回健康四卡页：通过。
- 终端日志可追踪 `UiEvent -> Coordinator -> PageIntent -> UI Adapter`：通过。
- 健康卡按下后滑动再松开不触发原位置点击：通过。
- Back 按下后滑动再松开不触发返回：修复后通过。

## 修复记录

验收中发现 Back 按下后滑动松开仍触发返回语义。修复方式是在 PC XML UI Adapter 中引入通用 `guarded_click` 注册入口，健康卡 hit target 和 Back 统一使用同一套点击防误触逻辑。

该修复不修改 LVGL 本体，不修改 LVGL Pro 生成文件，不接真实传感器，不影响旧 `magic_watch_sim` 目标。

## 未验证项

- 未验证 F411 真机显示。
- 未验证真实传感器数据。
- 未验证 LCD flush 性能。
- 未验证 SPI DMA 性能和稳定性。
- PC SDL simulator 的交互通过不等同于 F411 真机输入路径通过。
