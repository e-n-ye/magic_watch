# UI Lifecycle Contract

日期：2026-05-26

本文档是第 2A-doc 轮产物，只定义 UI 生命周期规则，不实现 helper，不修改代码，不拆 `SettingsPages.cpp` 或 `ShellPages.cpp`。

## Scope Lock

Allowed files:

- `docs/10_architecture/ui_lifecycle.md`

Forbidden files:

- `sim/lv_port_pc_vscode/src/**/*`
- `sim/lv_port_pc_vscode/CMakeLists.txt`
- `ShellPages.cpp/.h`
- `SettingsPages.cpp/.h`
- `AppStateMachine.*`
- `PowerController.*`
- `PageManager.*`
- `InputIntentRouter.*`
- `AppEvents.h`
- `DataCenter.*`
- 其他所有文档

Forbidden changes:

- 禁止修改任何 C++ 代码。
- 禁止新增 `LvglLifecycle.h/.cpp`。
- 禁止新增 `PageBase`。
- 禁止拆 `ShellPages` / `SettingsPages`。
- 禁止批量替换 timer。
- 禁止改变页面行为。
- 禁止让 C++ wrapper 自动拥有所有 LVGL object。
- 禁止引入复杂 ownership 模型。
- 禁止进入第 2A-helper 或第 2B。

## Current LVGL ownership assumptions

当前模拟器页面系统的事实基础：

- `PageManager` 通过 `register_page` 保存页面 factory，通过 `acquire` 懒创建 `PageBase` 实例。
- `PageBase::ensure_created()` 首次调用 `build()`，将返回的 `lv_obj_t*` 缓存在 `root_`。
- `PageManager::load()` 调用 `on_will_disappear()` / `on_will_appear()`，再用 `lv_screen_load_anim()` 切换 root。
- `PageManager` 当前缓存 `PageBase` 实例，不主动销毁页面实例或页面 root。
- 页面内部大量保存 `lv_obj_t*`、`lv_timer_t*`，并使用 `track(data_center_.subscribe(...))` 保存 EventBus subscription。
- 当前已有 `on_will_appear()` / `on_will_disappear()` 模式，但 timer、订阅、临时壳层状态的释放规则尚未统一。

生命周期契约的核心判断：

- LVGL 对象树拥有 LVGL child object 的最终释放权。
- C++ 页面对象默认只保存 LVGL 指针作为弱引用或受控引用。
- 页面拆分前，必须先明确 root、child、timer、subscription、临时壳层和页面状态的责任边界。

## Root object lifecycle

规则：

- 页面 root object 由页面的 `build()` 创建。
- 页面 root object 的创建入口是 `PageBase::ensure_created()`。
- 页面 root object 由 `PageManager` 选择何时显示，但当前 `PageManager` 不主动销毁 root。
- 页面 root object 在当前设计中默认可复用，不应假设每次进入页面都会重新 build。
- `on_will_appear()` 是页面即将显示时的刷新入口，不是 root 创建入口。
- `on_will_disappear()` 是页面即将离开时的暂停/停止入口，不是 root 销毁入口。

约束：

- 页面不得在 `on_will_appear()` 中无条件重复创建 root。
- 页面不得在 `on_will_disappear()` 中直接删除 root，除非未来有明确的 PageManager 销毁协议。
- 拆分页面文件时，不得改变 root 的创建时机。
- 临时壳层页面也必须遵守同一 root 生命周期规则，不能因为是 temporary page 就绕开 PageManager。

## Child object lifecycle

规则：

- 页面 `build()` 创建的 child object 应挂在 root 或 root 的子树下。
- LVGL 父对象删除时，子对象由 LVGL 级联释放。
- C++ 成员保存的 child `lv_obj_t*` 默认是弱引用或受控引用，不表示独占所有权。
- 页面刷新函数可以使用缓存的 child 指针，但必须假设它们可能在 root 删除后失效。

约束：

- 不得在 C++ wrapper 析构函数中盲目对所有 child `lv_obj_t*` 调用 `lv_obj_del()`。
- 不得让一个 child object 同时由 C++ RAII 和 LVGL 父对象树双重释放。
- 不得把 child object 的释放责任分散到多个 helper。
- 未来如果某个 helper 创建 child object，必须明确该 child 挂载到哪个 parent，并由 parent tree 释放。

## Timer lifecycle

规则：

- `lv_timer_t*` 不属于 LVGL object tree，不能依赖 root 删除自动释放。
- 谁创建 timer，谁负责停止/删除 timer。
- timer callback 使用 `this` 作为 user data 时，页面离开或销毁前必须保证 timer 不再回调失效对象。
- 当前页面代码里常见的 `crown_release_timer_`、`preview_close_timer_`、`auto_close_timer_`、`toast_timer_`、`measurement_timer_` 都应按同一原则处理。
- 第 1B 后，`AppStateMachine` 仍拥有其内部 power timer 的 `lv_timer_t*` 对象所有权；`PowerController` 只返回策略 Action。

约束：

- 页面 `on_will_disappear()` 中应停止只在页面可见时需要的 timer。
- 如果 timer 允许跨页面存在，必须在文档或局部注释说明原因和释放责任。
- timer callback 内部必须检查 user data 是否为空。
- timer 删除后必须将对应 `lv_timer_t*` 成员置为 `nullptr`。
- 不得将 `lv_timer_t*` 所有权移动到非 UI/lifecycle Controller，除非已有 timer guard 契约。
- 不得在拆 `ShellPages.cpp` 或 `SettingsPages.cpp` 时复制裸 timer 模式到更多文件。

## EventBus subscription lifecycle

规则：

- 页面订阅 EventBus 应优先通过 `PageBase::track()` 保存 subscription。
- `PageBase` 当前持有 `std::vector<EventBus::Subscription>`，页面实例销毁时 subscription 随页面释放。
- 由于 `PageManager` 当前缓存页面实例，页面 subscription 通常会长期存在，而不是页面离开时自动取消。
- 页面订阅回调必须检查页面是否可见、root 是否存在，或只修改页面内部缓存后等待 `on_will_appear()` 刷新。

约束：

- 拆页时不得把 subscription 变成裸全局订阅。
- 不得在 callback 中假设 root/child object 永远有效。
- 对只在可见期有效的输入订阅，应在 callback 中检查 `lv_screen_active() == root_` 或等价条件。
- 若未来引入 `EventSubscriptionGuard`，它应只管理 subscription 生命周期，不管理 LVGL object 生命周期。

## Temporary shell lifecycle

当前临时壳层包括但不限于：

- Notifications
- QuickSettings
- PowerMenu
- NotificationWakePreview

规则：

- 临时壳层由 `PageManager::show_temporary()` 显示，由 `dismiss_temporary()` 返回栈顶页面。
- 当前 `PageManager` 会缓存 temporary page 对应的 `PageBase` 实例；显示/隐藏不等于创建/销毁。
- 临时壳层 root 默认不是每次打开都新建。
- 临时壳层的短时状态，如 drag progress、preview close timer、toast timer，应在打开/关闭边界明确重置或停止。

约束：

- 临时壳层不能无脑常驻运行 timer。
- 临时壳层可以常驻页面实例，但不应常驻活跃动画、timer 或一次性输入状态。
- 关闭临时壳层时必须考虑 preview 状态、drag 状态、toast 状态是否需要 reset。
- 后续拆 `ShellPages.cpp` 前，必须逐页标注 temporary shell 是否常驻实例、是否有 timer、是否订阅 EventBus、隐藏时是否停止资源。

## Page state ownership

页面状态分三类：

1. 页面内部状态。
2. `DataCenter` 共享模型。
3. `AppStateMachine` / Coordinator 系统状态。

页面内部状态：

- 只影响单页显示、drag、局部 animation、选中项、临时 overlay 的状态应留在页面内部。
- 示例：通知卡片 swipe offset、QuickSettings toast 展示状态、页面局部 crown release 状态。

`DataCenter` 共享模型：

- 多页面共享、服务产生、需要跨页面保持一致的应用模型应放在 `DataCenter`。
- 示例：battery、time、steps、notifications、display policy、power mode、health monitoring settings。
- 页面可以请求修改模型，但不应绕过既有 DataCenter setter/publish 路径。

`AppStateMachine` / Coordinator 系统状态：

- 影响系统导航、页面栈、屏幕状态、临时壳层打开/关闭、跨域冲突的状态应留在 Coordinator 或专用 Controller。
- 示例：screen-off 页面栈快照、当前 temporary shell、PowerAction 执行、HomeRing surface restore。

约束：

- 页面拆分时不得把系统导航状态塞进页面类。
- Controller 不得保存页面内部 `lv_obj_t*` 或页面对象。
- 如果一个状态既影响页面显示又影响系统行为，优先让系统状态产生事件或 Action，页面只消费模型/preview。

## LV_EVENT_DELETE policy

规则：

- 保存 `lv_obj_t*` 的页面或 helper，默认必须把它看成可能失效的弱引用。
- 对长期缓存的关键 root/child 指针，未来应通过 `LV_EVENT_DELETE` 回调置空或标记 invalid。
- `LV_EVENT_DELETE` 回调不应做复杂业务逻辑，只做指针失效、timer 停止或轻量清理。
- 删除事件里不得再次删除同一对象。

当前轮次只定义原则，不要求立刻补齐所有 `LV_EVENT_DELETE`。

后续拆页前最低要求：

- 新增 helper 创建并长期保存 `lv_obj_t*` 时，必须说明是否需要 `LV_EVENT_DELETE`。
- 如果 helper 保存 parent/root 指针，必须在 parent/root 删除时置空。
- 如果页面 root 未来允许销毁，`PageBase::root_` 和页面内部重要 child 指针必须有失效策略。

## Forbidden ownership patterns

禁止模式：

- C++ wrapper 析构函数无差别 `lv_obj_del()` 所有保存的 `lv_obj_t*`。
- LVGL 父对象树和 C++ RAII 同时拥有同一个 child object。
- Controller 直接创建 LVGL object。
- Controller 持有 `lv_obj_t*`、`lv_timer_t*` 或页面对象。
- timer callback 绑定 `this`，但页面隐藏/销毁时不停止 timer。
- EventBus callback 直接操作可能已经删除或不可见的 LVGL object。
- 页面拆分时顺手改变 `PageManager` 的缓存/创建/显示语义。
- 临时壳层关闭后仍保留一次性 drag/preview 状态。
- 将页面 UI 生命周期问题伪装成 `AppStateMachine` 或 Controller 逻辑。

## Candidate minimal helpers for future round

本节只列未来候选，不在本轮实现。

优先候选：

- `LvglDeleteAwarePtr`: 保存 `lv_obj_t*` 弱引用，并在 `LV_EVENT_DELETE` 后标记 invalid 或置空。
- `LvglTimerGuard`: 管理单个 `lv_timer_t*` 的创建后删除和置空，不接管 LVGL object。
- `EventSubscriptionGuard`: 管理非 PageBase 场景下的 EventBus subscription 释放。
- `PageLifecycle` helper: 提供页面可见期 enter/leave 的小工具，不强制继承体系。

非优先候选：

- 完整重写 `PageBase`。
- 强制所有页面继承新的 `PageBase` 子类。
- 一次性替换全仓所有 timer。
- 给所有 LVGL object 套独占 RAII。

未来 helper 设计原则：

- 小而窄，只解决一个生命周期问题。
- 不改变现有页面行为。
- 不强迫页面系统改成完整 OOP 框架。
- 不拥有不该拥有的 LVGL object tree。
- 能逐页接入，不能要求一次性大迁移。

## Acceptance checklist before splitting SettingsPages / ShellPages

拆 `SettingsPages.cpp` 或 `ShellPages.cpp` 前，必须逐项确认：

- 已明确目标页面的 root object 由谁创建、是否复用、谁显示。
- 已明确目标页面是否会 destroy root；若会，谁调用 `lv_obj_del()`。
- 已列出目标页面保存的关键 child `lv_obj_t*`。
- 已列出目标页面所有 `lv_timer_t*`。
- 每个 timer 都明确创建点、停止点、删除点和置空点。
- 每个 timer callback 都检查 user data。
- 已列出目标页面所有 EventBus subscription。
- subscription callback 明确可见性检查或只更新内部缓存。
- 临时壳层页面明确是否常驻实例。
- 临时壳层页面隐藏时明确哪些 drag/preview/toast 状态要 reset。
- 页面状态归属已标注为页面内部、`DataCenter` 或 Coordinator/Controller。
- 不需要为本次拆分引入完整 `PageBase` 重构。
- 不需要让 Controller 操作 LVGL object。
- 构建通过。
- 关键 UI 路径手动回归通过：HomeRing、Launcher、Notifications、QuickSettings、PowerMenu、ScreenOff、LongBattery。

## Recommendation

第 2A-doc 后不建议直接进入第 2B。更稳的下一步是第 2A-helper 小闭环：只选择一个最小 helper，例如 `LvglTimerGuard` 或 `LvglDeleteAwarePtr`，先在极小范围内验证生命周期约束是否能落地。若用户决定暂不写 helper，也可以先用本文档作为拆分前审计清单，但拆 `ShellPages.cpp` 前风险会更高。
