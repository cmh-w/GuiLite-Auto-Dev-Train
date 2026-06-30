# GuiLite 项目分层架构原型设计

## 1. 架构总览

GuiLite 采用**严格单向依赖的四层分层架构**，自底向上依次为：

```
┌──────────────────────────────────────────────────────────────────┐
│                     第4层：自定义拓展控件层                        │
│              (Custom Extended Widgets Layer)                      │
│  用户项目中的自定义控件，继承自第3层原生控件或c_wnd               │
│  文件位置：用户项目代码（非 GuiLite 源码目录）                     │
├──────────────────────────────────────────────────────────────────┤
│                     第3层：原生控件层                              │
│              (Native Widgets Layer)                               │
│  c_button  c_label  c_edit  c_keyboard  c_list_box  c_spin_box   │
│  c_table  c_dialog  c_slide_group  c_wave_ctrl  c_progress_bar   │
│  c_wave_stat  c_wave_buffer                                       │
│  文件位置：src/widgets/  (仅 .h，实现在 src/widgets/widgets.cpp)   │
├──────────────────────────────────────────────────────────────────┤
│                     第2层：多平台适配层                            │
│              (Platform Adapter Layer)                             │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐   │
│  │  api_win.cpp    │  │  api_linux.cpp  │  │  api_unknow.cpp │   │
│  │  Win32 API实现  │  │  POSIX/Linux实现│  │  MCU裸机实现    │   │
│  │ CreateThread    │  │  pthread        │  │  空桩/delay_ms  │   │
│  │ Semaphore/Mutex │  │  sem_t/mutex    │  │  无锁FIFO       │   │
│  │ timeSetEvent    │  │  setitimer      │  │                 │   │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘   │
│  文件位置：src/core/adapter/                                      │
│  编译时通过条件编译选择唯一对应平台                                │
├──────────────────────────────────────────────────────────────────┤
│                     第1层：核心层                                  │
│              (Core Kernel Layer)                                  │
│  ┌────────────┐ ┌────────────┐ ┌────────────┐ ┌──────────────┐   │
│  │  api.h     │ │  wnd.h     │ │  display.h │ │  word.h      │   │
│  │  基础类型  │ │  窗口系统  │ │  渲染引擎  │ │  文字渲染    │   │
│  │  c_rect    │ │  c_wnd     │ │  c_display │ │  c_word      │   │
│  │  c_fifo    │ │  WND_TREE  │ │  c_surface │ │  UTF-8解析   │   │
│  │  颜色宏    │ │  事件系统  │ │  c_layer   │ │  点阵绘制    │   │
│  │  线程/定时 │ │  焦点管理  │ │  Z序管理   │ │              │   │
│  └────────────┘ └────────────┘ └────────────┘ └──────────────┘   │
│  ┌────────────┐ ┌────────────┐ ┌──────────────┐                  │
│  │  image.h   │ │ theme.h    │ │  resource.h  │                  │
│  │  图像渲染  │ │  主题管理  │ │  资源定义    │                  │
│  │  c_image   │ │  c_theme   │ │  BITMAP_INFO │                  │
│  │            │ │  颜色/字体 │ │  LATTICE     │                  │
│  │            │ │  图片配置  │ │              │                  │
│  └────────────┘ └────────────┘ └──────────────┘                  │
│  文件位置：src/core/                                              │
│  依赖：无外部依赖，仅依赖C/C++标准库                              │
├──────────────────────────────────────────────────────────────────┤
│                        编译入口                                  │
│  GuiLite.h — 自动生成的单头文件（通过 GenerateGuiLite.h.sh 拼接）│
│  CMakeLists.txt / GuiLite.vcxproj — 构建系统                     │
│  编译开关：-D GUILITE_ON                                         │
└──────────────────────────────────────────────────────────────────┘
```

---

## 2. 各层详细说明

### 2.1 核心层 (Core Layer)

**职责**：提供 GUI 引擎最基础的抽象，不依赖任何第三方库，仅使用标准 C/C++。

| 文件 | 核心类/类型 | 职责 |
|------|-------------|------|
| `api.h` | `c_rect`, `c_fifo`, 宏定义 | 矩形计算与碰撞检测、线程安全循环队列、ARGB/RGB 颜色宏、坐标对齐标志、断言/日志基础设施 |
| `resource.h` | `BITMAP_INFO`, `LATTICE`, `LATTICE_FONT_INFO` | 位图资源描述、点阵字模数据结构 |
| `wnd.h` | `c_wnd`, `WND_TREE`, `WND_CALLBACK` | 窗口基类（父子层级链表、焦点管理、触摸/导航事件分发、坐标变换、子窗口遍历） |
| `display.h` | `c_display`, `c_surface`, `c_layer` | 物理帧缓冲管理、逻辑表面（Surface）与 Z 序图层、像素绘制/矩形填充/线段绘制 (Bresenham)、图层激活与重叠仲裁、Surface 滑动切换动画 |
| `word.h` | `c_font_operator`, `c_lattice_font_op`, `c_word` | 字库操作抽象接口、点阵字体渲染引擎（UTF-8 解析/抗锯齿/对齐计算）、静态门面类 |
| `image.h` | `c_image_operator`, `c_bitmap_operator`, `c_image` | 图像渲染抽象接口、RGB565 位图绘制（支持透明色/裁切/跨图层穿透）、静态门面类 |
| `theme.h` | `c_theme` | 全局主题管理（字体/图片/颜色表的注册与查询，纯静态类） |
| `core.cpp` | 全局单例初始化 | 注册默认字体/图像操作算子、分配主题资源表 |

**核心层的关键设计模式**：

- **静态门面模式**：`c_word`、`c_image`、`c_theme` 对外提供全静态方法，内部持有可替换的 `operator` 指针，支持运行时更换渲染策略。
- **虚方法模板方法**：`c_wnd::connect` → `pre_create_wnd()` → `on_init_children()`，子类通过覆写实现自定义行为。
- **组件树模式**：窗口通过 `m_parent`/`m_top_child`/`m_next_sibling`/`m_prev_sibling` 构成双向链表树。
- **双缓冲 + Z 序**：`c_surface` 管理 0～2 三层独立帧缓冲，高 Z 序层可配置活动矩形区域，实现弹出菜单/对话框/键盘的透明叠加效果。

---

### 2.2 多平台适配层 (Platform Adapter Layer)

**职责**：封装操作系统差异，向上层提供一致的平台 API。

**被适配的 API（在 `api.h` 中声明）**：

```cpp
void register_debug_function(...);   // 调试与断言
void start_real_timer(...);          // 实时周期定时器（50ms）
void register_timer(...);            // 通用定时器
void create_thread(...);             // 线程创建
void thread_sleep(...);              // 睡眠
unsigned int get_cur_thread_id();    // 线程ID
long get_time_in_second();           // 时间
T_TIME get_time();
int build_bmp(...);                  // BMP截屏
// c_fifo 的 read/write 实现    （线程间通信）
```

**三个适配实现**：

| 文件 | 条件编译宏 | 典型运行环境 | 关键实现细节 |
|------|------------|-------------|-------------|
| `api_win.cpp` | `_WIN32`/`WIN32`/`_WIN64`/`WIN64` | Windows 桌面 | `CreateThread` / `WaitForSingleObject` / `CreateSemaphore` / `CreateMutex` / `timeGetTime` + `Sleep` |
| `api_linux.cpp` | `__linux__` / `__APPLE__` | Linux、macOS、树莓派 | `pthread_create` / `sem_wait` / `pthread_mutex_lock` / `setitimer(SIGALRM)` / `usleep` |
| `api_unknow.cpp` | `__none_os__` 或 未匹配任何已知平台 | MCU/RTOS (FreeRTOS, RT-Thread 等) | 空实现 + 死循环断言 + 用户提供 `delay_ms` / 无锁 FIFO（假设单核关中断保护） |

**关键设计**：
- 编译时通过**预处理器条件编译**选择唯一适配源文件（`CMakeLists.txt` 中 `FILE(GLOB CORE_ADAPTER ...)` 包含所有 `.cpp`，由内部 `#if` 控制编译）。
- `c_fifo` 的构造函数和 read/write 方法**在适配层实现**，核心层只声明。

---

### 2.3 原生控件层 (Native Widgets Layer)

**职责**：基于核心层 `c_wnd` 和渲染 API，提供开箱即用的 GUI 控件。

**控件清单**：

| 控件 | 类名 | 文件 | 继承关系 | 核心功能 |
|------|------|------|----------|---------|
| 按钮 | `c_button` | `button.h` | ← `c_wnd` | 三态绘制 (Normal/Focused/Pushed)、点击回调、触摸/导航事件 |
| 标签 | `c_label` | `label.h` | ← `c_wnd` | 仅显示文本，无交互 |
| 编辑框 | `c_edit` | `edit.h` | ← `c_wnd` | 软键盘输入、文本编辑、缓冲字符串 |
| 键盘 | `c_keyboard` | `keyboard.h` | ← `c_wnd` | 全键盘/数字键盘布局、大小写切换、字符/删除/确认分发 |
| 键盘按钮 | `c_keyboard_button` | `keyboard.h` | ← `c_button` | 显示各按键的字母或特殊功能文字 |
| 列表框 | `c_list_box` | `list_box.h` | ← `c_wnd` | 下拉选择、展开项列表、Z序图层弹出 |
| 微调框 | `c_spin_box` | `spinbox.h` | ← `c_wnd` | 数值增减 (max/min/step)、上下箭头按钮组合 |
| 微调按钮 | `c_spin_button` | `spinbox.h` | ← `c_button` | 触发增减操作的专用按钮 |
| 表格 | `c_table` | `table.h` | ← `c_wnd` | 行列模型、单元格坐标计算与绘制 |
| 对话框 | `c_dialog` | `dialog.h` | ← `c_wnd` | 模态/非模态、Z序图层管理、多 surface 关联 |
| 滑动组 | `c_slide_group` | `slide_group.h` | ← `c_wnd` | 多页面左右滑动手势切换、多 surface 动画 |
| 手势识别 | `c_gesture` | `slide_group.h` | 独立类 | 滑动手势检测 + 页面跟随/切换动画 |
| 波形缓冲区 | `c_wave_buffer` | `wave_buffer.h` | 独立类 | 循环采样缓存、帧数据读取 (min/max/mid)、速度匹配 |
| 波形控件 | `c_wave_ctrl` | `wave_ctrl.h` | ← `c_wnd` | 实时波形滚动显示、填充/扫描两种模式、平滑垂直线段 |
| 进度条 | `c_progress_bar` | `progress_bar.h` | ← `c_wnd` | 0–100% 填充进度条、百分比文字居中 |
| 波形统计 | `c_wave_stat` | `wave_stat.h` | ← `c_wnd` | 内部缓存 512 点、统计 min/max/avg/peak-to-peak、柱状可视化 |

**控件间依赖关系**：

```
c_wnd (core)
  ├── c_button          ← c_label 用不到，但复用 on_paint 风格
  ├── c_label
  ├── c_edit            ──→ 组合 c_keyboard
  ├── c_dialog
  ├── c_list_box        ──→ 组合 c_button (作单个列表项基类)
  ├── c_spin_box        ──→ 组合 c_spin_button (继承 c_button)
  ├── c_table
  ├── c_slide_group     ──→ 组合 c_gesture + 多 c_dialog
  ├── c_wave_ctrl       ──→ 组合 c_wave_buffer (数据结构，非控件)
  ├── c_progress_bar
  └── c_wave_stat

c_button
  └── c_keyboard_button (键盘专用)
  └── c_spin_button     (微调专用)
```

**实现位置**：
- **头文件**：每个控件独立 `.h`，含完整的 `on_paint` 内联实现（受 GuiLite 单头文件策略影响，widget 类的方法是 inline 的）。
- **实现文件**：`widgets.cpp` 存放跨控件共享的复合逻辑（`c_progress_bar` 和 `c_wave_stat` 的方法体 + 键盘布局静态数组 + 全局 `c_dialog::ms_the_dialogs` + `c_edit::s_keyboard` 静态实例）。

---

### 2.4 自定义拓展控件层 (Custom Extended Widgets Layer)

**职责**：用户项目中基于原生控件或核心层派生的业务特定控件。

**典型场景**：

```
// 示例：继承 c_wnd 创建仪表盘控件
class c_gauge : public c_wnd {
  void on_paint() override {
    // 使用 m_surface->draw_line/draw_arc/fill_rect 绘制弧形仪表
    // 使用 c_word::draw_value 显示读数
  }
};

// 示例：组合原生控件创建设置页面
c_wnd* create_settings_page(c_slide_group* parent) {
  auto* page = new c_wnd();  // 容器
  auto* spin = new c_spin_box();
  spin->connect(page, ID_BRIGHTNESS, "亮度", 10, 10, 200, 40);
  // ...
  return page;
}
```

**层间集成方式**：
- 自定义控件通过 `c_wnd::connect()` 挂接到控件树的任意节点。
- 复用核心层的 `c_surface` / `c_display` / `c_word` / `c_image` 全套渲染能力。
- 利用 `c_theme::add_color()` / `add_font()` 定制主题风格。
- 利用 `start_real_timer()` / `register_timer()` 驱动周期性重绘（如波形刷新）。

---

## 3. 编译与构建

### 3.1 构建系统

- **CMake**（主）：`src/CMakeLists.txt`，选项 `-D GUILITE_ON`，`FILE(GLOB WIDGETS_SRC widgets/*.cpp)` 自动收集。
- **Visual Studio**：`src/GuiLite.sln` + `GuiLite.vcxproj`，需手动将新 `.h` 加入 `<ClInclude>`。
- **Keil/IAR**：`src/GuiLite.uvprojx`（STM32 项目）。
- **Shell 脚本**：`src/GenerateGuiLite.h.sh` 拼接各头文件生成根目录 `GuiLite.h`（单头文件部署方式）。

### 3.2 编译开关

```bash
# Desktop (Win/Linux)
cmake -B build -DGUILITE_ON=ON
cmake --build build

# Include in user project
g++ -D GUILITE_ON -I GuiLite main.cpp GuiLite/src/core/core.cpp GuiLite/src/widgets/widgets.cpp
```

---

## 4. 核心数据流

### 4.1 初始化流程

```
用户 main()
  └─→ c_display(phy_fb, W, H, color_bytes, surface_cnt)
  └─→ c_surface(W, H, color_bytes, max_zorder)
  └─→ my_window.connect(display, ID, str, x, y, w, h, child_tree)
        └─→ pre_create_wnd()    [子类覆写]
        └─→ load_child_wnd()    [递归 connect 子窗口]
        └─→ on_init_children()  [子类覆写]
  └─→ my_window.show_window()
        └─→ on_paint()          [子类覆写]
        └─→ child.show_window() [递归]
  └─→ start_real_timer(global_timer_proc)
```

### 4.2 事件处理流

```
触摸事件:
  c_slide_group::on_touch → c_gesture::handle_swipe
    ├─ 手势识别为"滑动" → swipe_surface 动画 → set_active_slide
    └─ 手势识别为"点击" → c_wnd::on_touch
         └─ c_button::on_touch → parent->on_click callback

定时器刷新:
  start_real_timer → 50ms 周期 → global_timer_proc
    └─ c_wave_ctrl::refresh_wave → read_wave_data_by_frame → draw_smooth_vline
```

### 4.3 渲染数据流

```
c_wnd::on_paint()
  └─ m_surface->fill_rect(x0, y0, x1, y1, rgb, z_order)
       ├─ 写入 m_layers[z_order].fb  [图层帧缓冲]
       ├─ Z序比较 / 重叠仲裁
       └─ draw_pixel_low_level → m_display->draw_pixel → *m_phy_fb  [物理帧缓冲]
            └─ flush_screen() → memcpy 到 phy_fb [可选]
```

---

## 5. 架构特征总结

| 特征 | 说明 |
|------|------|
| **最小依赖** | 核心 + 控件层仅依赖 C/C++ 标准库（`<string.h>`, `<stdio.h>`, `<stdlib.h>`） |
| **平台无关** | 平台差异下沉至 Adapter 层三层实现，编译时静态选择 |
| **单头文件部署** | `GuiLite.h` 可独立分发，`#include "GuiLite.h"` 即得全套 API |
| **控件可扩展** | 任意继承 `c_wnd` 覆写 `on_paint` / `on_touch` / `pre_create_wnd` 即可创建新控件 |
| **Z 序图层** | 3 层独立帧缓冲 + 活动矩形仲裁，支持弹出层（键盘/对话框/列表）覆盖显示 |
| **资源占用低** | 适用于 MCU 级设备（16 位色深、无动态内存分配诉求、裸机友好） |
| **组件树架构** | `c_wnd` 双向链表管理父子/兄弟关系，深度不限 |
