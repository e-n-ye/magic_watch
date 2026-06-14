# Watch Architecture Learning Lab

这是 Magic Watch 的独立 PC 架构冲突实验场。

这里不复制 Magic Watch 的 Core、Adapter 或产品实现。每个 Stage 只在真实需求压力出现后，判断应当继续保持简单，还是承担新抽象的成本。

当前只存在：

- `runtime/`：稳定的 LVGL 9.6 + SDL 启动和主循环。
- `stage_00_direct/`：两张卡、详情区域和 Back 的直接调用实验。
- `stage_01_console_direct/`：无 LVGL 的终端直接调用练习骨架，等待用户接通页面状态与命令行为。

Stage 00 已冻结。Stage 01 当前只建立 Console 练习骨架，不提前引入 Intent、Core 或 Renderer。

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
