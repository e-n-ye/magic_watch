# Watch Architecture Learning Lab

这是 Magic Watch 的独立 PC 架构冲突实验场。

这里不复制 Magic Watch 的 Core、Adapter 或产品实现。每个 Stage 只在真实需求压力出现后，判断应当继续保持简单，还是承担新抽象的成本。

当前只存在：

- `runtime/`：稳定的 LVGL 9.6 + SDL 启动和主循环。
- `stage_00_direct/`：两张卡、详情区域和 Back 的直接调用实验。
- `stage_01_console_direct/`：已冻结的无 LVGL 终端直接调用基线。
- `stage_02_context_menu/`：已冻结的上下文相关菜单导航实验。

Stage 01 与 Stage 02 已完成并冻结。Stage 02 的规划与复盘见：

- `docs/40_workflow/agent_batch/cards/watch-arch-learning-stage02-q15.md`

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
