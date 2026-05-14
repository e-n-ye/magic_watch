# C++ 最小词典（面向本项目）

这份文档不是 C++ 教材。

它只解决一个问题：

- 让你在阅读 `sim/lv_port_pc_vscode` 当前主线代码时，不被少量 C++ 基础写法卡住

只讲 8 个概念：

1. `namespace`
2. `class`
3. `public/private`
4. `unique_ptr`
5. `virtual / = 0`
6. `using`
7. `const/constexpr`
8. `std::move`

每个概念都按“先翻译成 C 脑回路，再看本项目真实例子”的方式讲。

---

## 1. `namespace`

### 一句话翻译

`namespace` 不是类，不是结构体。  
它更像“名字分组”或者“带前缀的目录”。

### 如果按 C 的感觉理解

你在 C 里为了避免重名，可能会写：

```c
twsim_hal_create_simulator_device();
```

C++ 更喜欢写成：

```cpp
twsim::hal::create_simulator_device();
```

本质是同一个目的：

- 避免名字冲突
- 按模块分组

### 本项目里的真实例子

在 [HAL.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/hal/HAL.cpp:5)：

```cpp
namespace twsim::hal {

std::unique_ptr<Device> create_simulator_device() {
  return platform::simulator::create_device();
}

}  // namespace twsim::hal
```

意思是：

- 这个函数属于 `twsim::hal`
- 所以外面调用时要写：

```cpp
twsim::hal::create_simulator_device()
```

### 你要记住

- `::` 这里表示“到某个名字空间里找名字”
- 不是指针访问
- 不是函数指针

---

## 2. `class`

### 一句话翻译

`class` 可以先理解成：

- “带函数的结构体”

### 如果按 C 的感觉理解

你可以把它粗略想成：

- 一个 `struct`
- 外加一组默认和它绑定的函数

只是 C++ 把“数据”和“操作这些数据的函数”写在一起了。

### 本项目里的真实例子

在 [Application.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Application.h:13)：

```cpp
class Application {
 public:
  explicit Application(std::unique_ptr<hal::Device> device);

  bool start();
  void tick(std::uint32_t elapsed_ms);

 private:
  void register_pages();
  void handle_hal_event(const hal::Event& event);

  std::unique_ptr<hal::Device> device_;
  DataCenter data_center_;
  PageManager page_manager_;
  InputIntentRouter input_router_;
  AppStateMachine state_machine_;
};
```

你可以先把它看成：

- `Application` 这类对象内部有 5 个主要成员变量
- 还有几个专门操作它们的函数

### 你要记住

- `class` 不是“另一个世界的新东西”
- 它先按“结构体 + 相关函数”理解就够了

---

## 3. `public/private`

### 一句话翻译

- `public`：外面允许用
- `private`：只允许类自己内部用

### 如果按 C 的感觉理解

如果是 C，大家通常靠约定：

- 哪些函数给外部用
- 哪些函数只是内部辅助

C++ 可以把这个约定写进语法里。

### 本项目里的真实例子

还是看 [Application.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Application.h:14)：

```cpp
public:
  bool start();
  void tick(std::uint32_t elapsed_ms);

private:
  void register_pages();
  void handle_hal_event(const hal::Event& event);
```

意思是：

- 外部可以调用 `start()` 和 `tick()`
- 外部不该直接调 `register_pages()` 和 `handle_hal_event()`

### 你要记住

它的核心目的不是“炫技”，而是：

- 让模块边界更明确

---

## 4. `std::unique_ptr`

### 一句话翻译

`std::unique_ptr<T>` 可以先理解成：

- “自动释放的独占指针”

### 如果按 C 的感觉理解

先想象 C 写法：

```c
Device* p = malloc(...);
```

然后你要非常小心：

- 谁负责释放
- 什么时候释放
- 会不会重复释放
- 会不会忘记释放

`unique_ptr` 解决的是这个问题。

### “独占”是什么意思

不是说“其他任何指针都不能指向它”，而是说：

- **只有这一个 `unique_ptr` 负责拥有它和释放它**

其他地方仍然可以临时拿：

- 裸指针
- 引用

但不能再来一个新的 `unique_ptr` 也说“这个对象归我管”。

### 本项目里的真实例子 1

在 [HAL.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/hal/HAL.h:115)：

```cpp
std::unique_ptr<Device> create_simulator_device();
```

意思是：

- 返回一个独占拥有 `Device` 对象的智能指针

### 本项目里的真实例子 2

在 [Application.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Application.h:24)：

```cpp
std::unique_ptr<hal::Device> device_;
```

意思是：

- `Application` 里面有个成员 `device_`
- 它独占拥有一个 `hal::Device` 对象

### 尖括号 `<...>` 是什么

这是模板参数。

```cpp
std::unique_ptr<hal::Device>
```

意思是：

- 用 `hal::Device` 这个类型
- 去生成一个“专门管理 `hal::Device` 的 unique_ptr 类型”

你可以先把它想成：

- `unique_ptr(T)` 的 C++ 写法是 `unique_ptr<T>`

---

## 5. `virtual / = 0`

### 一句话翻译

这套写法的目的，是做“接口”和“多态”。

### 如果按 C 的感觉理解

它有点像：

- 一张“接口规范”
- 再加“不同实现填不同函数”

如果硬要类比 C，可以把它类比成：

- 一套函数指针表规范

### 本项目里的真实例子 1：接口

在 [HAL.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/hal/HAL.h:106)：

```cpp
class Device {
 public:
  virtual ~Device() = default;

  virtual bool initialize(const DisplayConfig& config) = 0;
  virtual void set_event_callback(EventCallback callback) = 0;
  virtual void tick(std::uint32_t elapsed_ms) = 0;
};
```

这里的：

```cpp
virtual ... = 0;
```

意思是：

- 这里只规定“必须有这个函数”
- 但这个类自己不提供实现

所以 `Device` 更像“接口基类”。

### 本项目里的真实例子 2：具体实现

在 [SimulatorDevice.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/Platform/Simulator/SimulatorDevice.cpp:47)：

```cpp
class SimulatorDevice final : public hal::Device {
 public:
  bool initialize(const hal::DisplayConfig& config) override {
```

意思是：

- `SimulatorDevice` 继承了 `hal::Device`
- 它真的把 `initialize/tick/...` 这些函数实现出来了

所以：

- `Device`：接口
- `SimulatorDevice`：具体实现

### 为什么这样写

因为上层只想知道：

- “这是一个设备”

不想知道它到底是：

- PC 模拟器设备
- 还是真实手表硬件设备

---

## 6. `using`

### 一句话翻译

`using` 在这个项目里大多数时候就是：

- “起别名”

### 如果按 C 的感觉理解

它很接近 C 里的：

- `typedef`

### 本项目里的真实例子 1

在 [Application.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Application.cpp:11)：

```cpp
using MenuItem = MenuPage::Item;
```

意思是：

- 以后写 `MenuItem`
- 就等价于写 `MenuPage::Item`

### 本项目里的真实例子 2

在 [HAL.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/hal/HAL.h:104)：

```cpp
using EventCallback = std::function<void(const Event&)>;
```

意思是：

- 给这一长串类型起个短名字叫 `EventCallback`

### 你要记住

遇到 `using X = Y;`

先翻译成：

- “给 `Y` 起个别名叫 `X`”

---

## 7. `const / constexpr`

### 一句话翻译

- `const`：这个值不允许被改
- `constexpr`：这个值是编译期常量

### 如果按 C 的感觉理解

`const` 很接近 C 里的只读含义。  
`constexpr` 可以先粗略理解成“更强一点的 `const`”。

### 本项目里的真实例子 1：`const`

在 [Application.h](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/App/Application.h:22)：

```cpp
void handle_hal_event(const hal::Event& event);
```

这里的 `const hal::Event&` 意思是：

- 这是一个事件对象的引用
- 只读，不允许在这个函数里改它

如果按 C 感觉，可以近似理解成：

- “传进来一个只读指针/只读对象视图”

### 本项目里的真实例子 2：`constexpr`

在 [SimulatorDevice.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/Platform/Simulator/SimulatorDevice.cpp:24)：

```cpp
constexpr std::uint32_t kMainButtonLongPressMs = 900U;
```

意思是：

- 主按键长按阈值 900ms
- 这是个编译期固定常量

### 你要记住

- `const` 关注“不能改”
- `constexpr` 关注“编译时就确定”

---

## 8. `std::move`

### 一句话翻译

`std::move(x)` 在这个项目里最重要的意义是：

- “把拥有权交出去”

### 如果按 C 的感觉理解

在 C 里你可能会写：

- 这个指针以后归另一个模块管
- 原来的地方不要再当自己拥有它

`std::move` 就是在表达这件事。

### 本项目里的真实例子

在 [main.cpp](/D:/MY_Desk/watch/magic_watch/sim/lv_port_pc_vscode/src/main.cpp:27)：

```cpp
twsim::app::Application application(std::move(device));
```

这里的 `device` 原本是：

- 一个 `std::unique_ptr<hal::Device>`

`unique_ptr` 不能随便复制，因为它是“独占拥有”。

所以这里不能写：

```cpp
Application application(device);
```

必须写：

```cpp
Application application(std::move(device));
```

意思是：

- 把 `device` 的拥有权转移给 `Application`

之后你就该认为：

- 原来的 `device` 不再拥有那个对象了

---

## 最后只记住这 4 句话

如果你现在不想背完整概念，只先记住这 4 句：

1. `namespace`：就是带前缀的名字分组。
2. `class`：先当成“带函数的结构体”。
3. `unique_ptr`：先当成“自动释放的独占指针”。
4. `virtual / = 0`：先当成“接口规范，具体实现另外填”。

做到这一步，你就已经能读本项目当前主线的大半内容了。
