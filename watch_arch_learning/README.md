# Watch Architecture Learning Lab

这是 Magic Watch 的独立 PC 架构冲突实验场。

这里不复制 Magic Watch 的 Core、Adapter 或产品实现。每个 Stage 只在真实需求压力出现后，判断应当继续保持简单，还是承担新抽象的成本。

当前只存在：

- `runtime/`：稳定的 LVGL 9.6 + SDL 启动和主循环。
- `stage_00_direct/`：两张卡、详情区域和 Back 的直接调用实验。
- `stage_01_console_direct/`：已冻结的无 LVGL 终端直接调用基线。
- `stage_02_context_menu/`：已冻结的上下文相关菜单导航实验。
- `stage_03_multi_input_direct/`：正在验证第二种输入表示的直接翻译方案。
- `stage_04_command_language_pressure/`：已冻结的命令字符串压力实验。

Stage 01 与 Stage 02 已完成并冻结。Stage 02 的规划与复盘见：

- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage02-q15.md`

Stage 03 已冻结，复盘见：

- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage03-q16.md`

Stage 04 已完成第一版并冻结，复盘见：

- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage04-q17.md`

Stage 05 当前只完成规划，不创建代码目录：

- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage05-q18.md`

## 构建

```powershell
cmake --preset mingw-debug
cmake --build --preset mingw-debug
```

运行：

```powershell
watch_arch_learning/build/mingw-debug/out/watch_arch_stage_00_direct.exe
```

Stage 01 使用独立的 Console-only preset，不查找 SDL，也不构建 LVGL：

```powershell
cmake --preset console-debug
cmake --build --preset console-debug
watch_arch_learning/build/console-debug/out/watch_arch_stage_01_console_direct.exe
```

Stage 02 使用同一 Console-only preset：

```powershell
cmake --build --preset console-debug --target watch_arch_stage_02_context_menu
watch_arch_learning/build/console-debug/out/watch_arch_stage_02_context_menu.exe
```

Stage 03 也使用 Console-only preset：

```powershell
cmake --build --preset console-debug --target watch_arch_stage_03_multi_input_direct
watch_arch_learning/build/console-debug/out/watch_arch_stage_03_multi_input_direct.exe
```

Stage 04 同样使用 Console-only preset：

```powershell
cmake --build --preset console-debug --target watch_arch_stage_04_command_language_pressure
watch_arch_learning/build/console-debug/out/watch_arch_stage_04_command_language_pressure.exe
```
