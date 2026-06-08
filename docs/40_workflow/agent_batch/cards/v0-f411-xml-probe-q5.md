# V0.3 F411 XML 兼容性探针卡片包

本卡片包承接长期路线中的 `V0.3`。目标是在真正把 XML 生成 UI 上 F411 前，先判断 LVGL 版本、生成 C API、资源格式、Flash/RAM 和构建路径是否可接受。

本包只做探针和判断，不手改生成文件，不升级 LVGL，不把 runtime XML parser 或 PNG/FS 当作 F411 主路径。

---

## V0.3-A 列出生成 C 的 LVGL API 清单

- 批次：V0.3-F411-XML-PROBE
- 状态：TODO
- 依赖：`V0.2-C`
- 自检：`git status --short -uall`；`git diff --check`；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`docs: list lvgl pro generated api usage`
- Doc Impact：small

### 问题定位

PC XML 目标已通过，但它运行在 PC LVGL 9.6.0-dev 环境。F411 当前是 LVGL 8.2.0，不能直接假设生成 C 可编译。

### 实施方案

1. 扫描 `ui/lvgl_pro` 生成 C / H 中调用的 `lv_*` API、对象类型、style 属性和 subject / event 相关入口。
2. 输出 API 清单到兼容性探针文档草稿。
3. 不改生成文件，不改 F411 工程。

### 涉及位置

Allowed files:

- `docs/30_testing/f411_xml_compatibility_probe.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-xml-probe-q5.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `ui/lvgl_pro/**`
- `watch_core/**`
- `sim/lv_port_pc_vscode/**`
- `try/my_watch_f411_v2.1/**`

Forbidden changes:

- 禁止手改生成文件。
- 禁止升级 LVGL。
- 禁止把生成 C 登记进 F411 工程。

### 风险

- 只靠文本扫描可能漏掉宏展开或间接调用；本卡输出是探针清单，不是最终编译证明。

### 验收标准

- 文档列出生成 C 直接使用的主要 LVGL API 和对象类型。
- 清单能服务 V0.3-B 的版本兼容判断。

### 执行记录

- 待执行。

---

## V0.3-B 评估 LVGL 9.6 到 8.2 的不兼容点

- 批次：V0.3-F411-XML-PROBE
- 状态：TODO
- 依赖：`V0.3-A`
- 自检：`git status --short -uall`；`git diff --check`；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`docs: compare lvgl xml api compatibility`
- Doc Impact：small

### 问题定位

F411 接 XML 前必须知道缺口属于少量 shim、资源替换，还是 LVGL 版本差异过大。

### 实施方案

1. 对照 F411 `try/my_watch_f411_v2.1/user/third_party/lvgl` 中的 LVGL 8.2 头文件，确认 V0.3-A 清单是否存在。
2. 标记高风险项，例如 LVGL 9 新 API、subject、event 名称变化、对象类型差异。
3. 给出每个不兼容点的处理建议：可 shim、需替换资源、需重生成、应止损。

### 涉及位置

Allowed files:

- `docs/30_testing/f411_xml_compatibility_probe.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-xml-probe-q5.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `ui/lvgl_pro/**`
- `try/my_watch_f411_v2.1/user/third_party/lvgl/**`
- `watch_core/**`

Forbidden changes:

- 禁止修改 LVGL 8.2 源码或配置。
- 禁止新增兼容宏。
- 禁止把“不确定”写成“可编译”。

### 风险

- 兼容性探针可能得出“当前生成方案不适合 F411”的结论；这属于有效结果，不应为了继续而绕过停止条件。

### 验收标准

- 文档明确列出兼容、可 shim、不兼容和未确认项。
- 能判断是否需要停止并重评 UI 生成路线。

### 执行记录

- 待执行。

---

## V0.3-C 评估图片、字体、Flash/RAM 资源成本

- 批次：V0.3-F411-XML-PROBE
- 状态：TODO
- 依赖：`V0.3-B`
- 自检：`git status --short -uall`；`git diff --check`；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`docs: assess f411 xml resource budget`
- Doc Impact：small

### 问题定位

PC 目标可以使用文件路径资源，但 F411 当前 `LV_USE_FS_*` 和 `LV_USE_PNG` 不是主路径。若健康四卡依赖 PNG/FS，直接上板会把问题伪装成 LVGL 兼容问题。

### 实施方案

1. 统计 `ui/lvgl_pro` 使用的图片、字体和生成 C 资源入口。
2. 检查 F411 当前 LVGL 配置中的 PNG、FS、字体和图片支持状态。
3. 估算 Flash/RAM 增量风险，明确优先 C 数组或轻量 LVGL 对象资源。
4. 不生成新资源，不改 F411 工程登记。

### 涉及位置

Allowed files:

- `docs/30_testing/f411_xml_compatibility_probe.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-xml-probe-q5.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `ui/lvgl_pro/**`
- `try/my_watch_f411_v2.1/user/third_party/lvgl/lv_conf.h`
- `try/my_watch_f411_v2.1/MDK-ARM/**`
- `try/my_watch_f411_v2.1/README.md`

Forbidden changes:

- 禁止开启 PNG / FS。
- 禁止把文件路径资源作为 F411 主路径。
- 禁止改字体配置或 MDK 工程。

### 风险

- 资源成本可能比 API 兼容更早成为停止条件；文档必须把这类风险单独列出。

### 验收标准

- 文档列出图片、字体、Flash/RAM 和 FS/PNG 风险。
- 能判断 F411 XML 上板是否需要先做资源替换。

### 执行记录

- 待执行。

---

## V0.3-D 输出继续 / 止损结论

- 批次：V0.3-F411-XML-PROBE
- 状态：TODO
- 依赖：`V0.3-C`
- 自检：`git status --short -uall`；`git diff --check`；本轮实际改动中文文档乱码哨兵检查
- 建议提交信息：`docs: conclude f411 xml compatibility probe`
- Doc Impact：required

### 问题定位

V0.3 的验收不是跑通画面，而是在写代码前判断是否值得进入 `V0.4`。如果需要 runtime XML、PNG/FS 或大面积手改生成代码，就应停止并重评 UI 生成路线。

### 实施方案

1. 完成 `docs/30_testing/f411_xml_compatibility_probe.md`。
2. 给出结论：可继续、需小 shim 后继续、需资源替换后继续、停止并重评。
3. 若可继续，只规划 `V0.4` 卡片，不在本卡实现上板。
4. 若停止，记录触发的停止条件和替代路线候选。

### 涉及位置

Allowed files:

- `docs/30_testing/f411_xml_compatibility_probe.md`
- `docs/00_current/magicwatch_long_term_roadmap.md`
- `docs/40_workflow/agent_batch/cards/v0-f411-xml-probe-q5.md`
- `docs/40_workflow/agent_batch/agent-queue.md`
- `docs/40_workflow/agent_batch/agent-progress.md`

Read-only files:

- `ui/lvgl_pro/**`
- `watch_core/**`
- `try/my_watch_f411_v2.1/**`
- `sim/lv_port_pc_vscode/**`

Forbidden changes:

- 禁止实现 F411 上板。
- 禁止手改生成文件。
- 禁止把未编译结论写成已编译通过。
- 禁止绕过停止条件继续生成 V0.4 代码卡。

### 风险

- 如果结论是停止，短期看会慢一点，但能避免把 F411 资源和 LVGL 版本问题拖到实现阶段才爆出来。

### 验收标准

- 文档明确继续或止损结论。
- 若继续，下一步只生成 `V0.4` 卡片并声明输入条件。
- 若止损，队列不得继续进入 F411 XML 上板实现。

### 执行记录

- 待执行。
