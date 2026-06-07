# MagicWatch AI 协作规则

> 状态：Current Draft  
> 更新日期：2026-06-07  
> 路由类型：工作流规则  
> 适用场景：AI 执行代码或文档任务前的 Scope Lock、执行中防越界、收尾审查

---

## 1. 一句话规则

AI 每次执行 MagicWatch 任务，都必须先锁边界，再小卡推进，最后按 checklist 审查。

MagicWatch 的目标不是让 AI 写更多代码，而是让 AI 在清晰边界内帮助用户训练系统设计、性能分析和工程审查能力。

---

## 2. 每次任务必须声明 Scope Lock

任何实现任务开始前，必须写清：

```text
Goal:
- 本卡要完成什么

Allowed files:
- 本卡允许修改的文件

Forbidden files:
- 本卡明确禁止修改的文件

Forbidden changes:
- 本卡明确禁止的行为

Outputs:
- 本卡结束后必须交付什么

Verification:
- 本卡要运行哪些命令或人工验证
```

如果无法把 `Allowed files` 控制在少量文件内，优先拆卡。

---

## 3. 默认禁止事项

除非当前卡片明确允许，否则默认禁止：

- 顺手重构。
- 改目录结构。
- 引入 `malloc/free`。
- 引入 C++ 到 `watch_core`。
- 引入 STL、`new/delete`、`std::function`。
- 改 public interface。
- 手改生成文件。
- 升级 LVGL。
- 修改已验收 DMA 逻辑。
- 修改无关平台端口。
- 把 PC 结果写成 F411 真机结果。
- 把构建通过写成手动 UI 验收通过。

---

## 4. 分层边界

### Core

Core 负责系统语义：

- 输入意图。
- 事件队列。
- 页面状态。
- 电源状态。
- 模型快照。
- Coordinator。

Core 禁止：

- include `lvgl.h`。
- include STM32 HAL、ESP-IDF、SDL、CubeMX 头文件。
- 持有 LVGL 对象指针。
- 调用 screen create / load API。
- 访问平台 driver。
- 使用堆和 STL。

### UI

UI 负责 LVGL 对象、生成 UI、View Adapter 和 callback 适配。

UI 禁止：

- 直接访问 HAL。
- 直接读传感器。
- 直接决定电源策略。
- 在 callback 中切业务页面。
- 把 LVGL Subject 当业务状态源。

### Platform

Platform 负责 display、input、tick、power apply、sensor adapter、vibration。

Platform 禁止：

- 拥有页面业务状态。
- 直接切页面。
- include 页面私有头。
- 解释通知、健康、设置等业务语义。

---

## 5. 生成文件规则

`ui/lvgl_pro` 中的生成 C 文件可以入库，但禁止手改。

允许：

- diff 生成文件。
- 登记生成文件。
- 重新生成后提交。
- 在手写 adapter 中包装生成对象。

禁止：

- 为了跑通 F411 手动 patch 生成文件。
- 把业务逻辑写进生成文件。
- 在生成文件里加入平台分支。
- 把生成文件当作长期手写 UI 层。

如果生成 C 在 F411 上需要大量手改才能跑，必须停止并回到兼容性探针结论。

---

## 6. 性能任务规则

性能任务必须先观测，再优化。

允许观测：

- FPS。
- flush calls/s。
- pixels/s。
- last flush ms。
- dirty area。
- RGB565 转换耗时。
- SPI / DMA 传输耗时。
- `lv_timer_handler` 调用频率。

禁止：

- 凭体感宣称性能提升。
- 只看单个 FPS 数字。
- 把 debug probe 负载当产品 UI 基线。
- 继续优化已经判定不是主瓶颈的局部链路。

性能结论必须说明：

- 场景。
- 指标。
- 对比口径。
- 未验证项。
- 是否有花屏、卡死、撕裂、输入迟滞。

---

## 7. 人工验收 checklist

AI 完成后，必须逐项检查：

- [ ] 是否声明 `Allowed files / Forbidden files / Forbidden changes`？
- [ ] 是否修改了 forbidden files？
- [ ] 是否引入 `malloc/free/new/delete/STL`？
- [ ] `watch_core` 是否 include 了 LVGL / HAL / SDL / CubeMX？
- [ ] UI 层是否直接访问 HAL 或平台 driver？
- [ ] platform port 是否拥有页面跳转或业务状态？
- [ ] 是否手改生成文件？
- [ ] 是否改变已验收 DMA、颜色、字体、PC 四卡行为？
- [ ] 是否把未编译、未烧录、未人工观察写成“通过”？
- [ ] 是否有指标、风险、未验证项记录？

若任一项命中，必须说明原因。若命中项破坏本卡边界，停止并回滚或重新拆卡。

---

## 8. 收尾报告模板

每次任务收尾时，报告应包含：

```text
完成：
- 本轮实际完成了什么

修改文件：
- 文件列表

验证：
- 已运行的命令
- 已执行的人工验证

未验证：
- 编译、烧录、真机、手动 UI 等未执行项

风险：
- 仍需关注的问题

Doc Impact:
- none / small / required
```

不要把“建议下一步”写成“已经完成”。

---

## 9. 停止条件

出现以下情况必须停止并讨论：

- 需要大面积修改生成文件。
- 需要升级 LVGL 才能继续。
- 需要打开 runtime XML parser 才能继续。
- F411 必须依赖 PNG / FS 文件路径才可显示。
- `watch_core` 开始依赖 UI 或 HAL。
- Platform 开始保存业务状态。
- 单张卡超过 3 个核心模块。
- 性能指标不可复现。
- 用户真机现象与文档假设冲突。

停止不是失败，而是防止项目被局部问题拖偏。

---

## 10. 文档同步规则

只在事实成立后更新文档。

允许：

- 记录已验证指标。
- 记录明确失败原因。
- 记录停止条件触发。
- 记录人工验收结论。

禁止：

- 把计划写成完成。
- 把 PC 现象写成真机现象。
- 把未执行的测试写成通过。
- 因为文档更顺眼而改无关历史内容。

中文文档修改后，必须对本轮实际改动的中文文件做乱码哨兵检查。
