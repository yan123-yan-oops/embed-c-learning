# 导航抗干扰嵌入式系统 — 学习工程

> 🎯 **目标岗位**：中森通信 · 嵌入式开发（导航抗干扰产品实施）
>
> **岗位职责**：导航抗干扰产品的软件构建/设计/实现、项目实施、调试测试、文档编写
> **任职要求**：嵌入式 RTOS 开发 + 熟练 C/C++ + MATLAB 仿真分析 + 信号与系统/DSP/通信原理

---

## 📌 这个工程是什么

围绕**导航抗干扰嵌入式岗位**的技能要求，按 6 个月路线分阶段建设的能力仓库。
每个目录都是一块"积木"——从 C 语法到 RTOS 多任务，再到 MATLAB 抗干扰仿真，
最终拼成一份可以直接摆在面试官面前的作品集。

```
北斗卫星 ─→ RF前端 ─→ ADC ─→ FPGA/ARM (C 采集) ─→ PC/MATLAB (算法验证)
                              ↑ 嵌入式轨              ↑ 算法轨
                              本仓库覆盖              配套仓库：matlab-anti-jamming
```

---

## 📂 目录结构

```
embed-c-learning/
├── 01-basics/                          # C 基础 + 数据类型
│   ├── data_types_and_twos_complement.c  # 数据类型/整型提升/补码/%d vs %u
│   ├── volatile_static.c                 # volatile/static 三大场景
│   └── EXERCISES.md                      ← 本章习题
│
├── 02-pointers/                        # 指针
│   ├── pointers_intro.c                  # 指针、数组、函数指针
│   ├── pointer_comprehensive.c           # 指针综合应用
│   └── EXERCISES.md                      ← 本章习题
│
├── 03-bitwise/                         # 位操作
│   ├── bit_ops.c                         # GPIO 置位/清位/翻转、寄存器位域
│   └── EXERCISES.md                      ← 本章习题
│
├── 04-memory/                          # 内存管理
│   ├── memory_layout.c                   # 栈/堆/全局区/常量区 + 内存池
│   └── EXERCISES.md                      ← 本章习题
│
├── 05-struct-enum/                     # 结构体与枚举
│   ├── struct_enum.c                     # struct/union/packed/位域
│   └── EXERCISES.md                      ← 本章习题
│
├── 06-strings/                         # 字符串处理
│   ├── string_parse.c                    # NMEA 字符串解析
│   └── EXERCISES.md                      ← 本章习题
│
├── 07-ringbuffer/                      # 环形缓冲区
│   ├── ring_buffer.c                     # ISR 安全的无锁环形队列
│   └── EXERCISES.md                      ← 本章习题
│
├── 08-nmea-parser/                     # NMEA 0183 解析器（模块化工程）
│   ├── main.c
│   ├── nmea_parser.c
│   ├── nmea_parser.h
│   └── EXERCISES.md                      ← 本章习题
│
├── 09-pool-allocator/                  # 静态内存池
│   ├── memory_pool.c                     # 零 malloc 的内存管理
│   └── EXERCISES.md                      ← 本章习题
│
├── 10-state-machine/                   # 状态机
│   ├── state_machine.c                   # 有限状态机框架
│   └── EXERCISES.md                      ← 本章习题
│
├── .vscode/                            # VS Code 配置（智能提示/编译任务）
└── README.md                           # 本文件
```

---

## 🛠️ 环境搭建

### 第 1 步：安装 GCC 编译器（三选一）

| 方案 | 适合 | 下载 |
|------|------|------|
| **MinGW-w64**（推荐） | 最常用 | https://github.com/niXman/mingw-builds-binaries/releases |
| TDM-GCC | 一键安装 | https://jmeubank.github.io/tdm-gcc/ |
| MSYS2 | 最完整 | https://www.msys2.org/ |

装好后将 `bin` 目录加入系统 PATH，终端运行 `gcc --version` 验证。

### 第 2 步：VS Code 打开本文件夹

```
文件 → 打开文件夹 → 选择 embed-c-learning
```

### 第 3 步：安装推荐扩展

按 `Ctrl+Shift+X`，搜索安装：
- **C/C++** (ms-vscode.cpptools) — 必装
- **C/C++ Extension Pack** — 推荐

### 第 4 步：编译运行

- 打开任意 `.c` 文件
- 按 `Ctrl+Shift+B` → 选择 "build current file"
- 或终端手动：`gcc -Wall -std=c11 hello.c -o hello.exe && hello.exe`

---

## 🗺️ 学习路线（6 个月 → 中森通信）

### 阶段总览

| 阶段 | 时间 | C/嵌入式轨 | MATLAB/算法轨 | 交付物 |
|------|------|-----------|--------------|--------|
| **1** | 第 1 月 | C 筑基（01-10） | MATLAB 入门 + 信号基础 | NMEA 解析器 + 内存池 |
| **2** | 第 2-3 月 | ARM 外设 + FreeRTOS | GNSS 信号生成 + 干扰建模 | 北斗多任务采集工程 |
| **3** | 第 4 月 | 项目整合调试 | 抗干扰算法仿真 | [matlab-anti-jamming](https://github.com/yan123-yan-oops/matlab-anti-jamming) |
| **4** | 第 5-6 月 | 简历 + 面试 | 作品集整理 | 面试题库 + 作品集 |

> **为什么 C 和 MATLAB 要并行？** 嵌入式采集数据（C）+ 上位机验证算法（MATLAB）= 完整闭环。
> 中森通信 JD 明确要求"熟练使用 MATLAB 等工具对数据进行仿真分析"，
> 同时要求"嵌入式 RTOS 开发经验 + 熟练 C/C++"——两条腿走路。

### 阶段 1：C 筑基 + MATLAB 入门（第 1 月）

| 周 | C / 嵌入式（~70%） | MATLAB / 信号（~30%） |
|----|-------------------|----------------------|
| W1 | 01-basics 数据类型 / volatile / static | 安装 MATLAB → 矩阵运算、plot 画图 |
| W2 | 02-pointers 指针 / 函数指针 · 04-memory 内存布局 | 生成正弦波、读取 .bin 文件 |
| W3 | 03-bitwise 位操作 / 寄存器宏 · 05-struct-enum | 频谱分析 fft / 加噪声 |
| W4 | 06-strings NMEA 解析 · 07-ringbuffer 环形缓冲区 · 09-pool-allocator | 用 MATLAB 验证 NMEA 解析 |
| W5 🚀 | **08-nmea-parser** 完整实现 · 10-state-machine 状态机 | — |

**阶段 1 交付物**（简历可直接写）：
- ✅ 串口 NMEA 0183 解析器（模块化 .h/.c 分离，CRC 校验，可移植 STM32）
- ✅ 静态内存池管理器（固定块池 + 可变块池，零 malloc）

### 阶段 2：ARM 外设 + FreeRTOS（第 2-3 月）

> 对应 JD："具备嵌入式 RTOS 系统开发经验"

**第 2 月：ARM 底层外设**

| 周 | 内容 | 练习 |
|----|------|------|
| W6 | STM32F4/F7 或 GD32 环境 | 点灯、GPIO |
| W7 | UART 串口（中断 + DMA） | 接收 GPS 模块数据 |
| W8 | 定时器 TIM / PWM / 输入捕获 | 产生 PWM、测频率 |
| W9 | SPI / I2C 外设 | 读取 IMU 传感器 |

**第 3 月：FreeRTOS + 北斗采集框架**

| 周 | 内容 | 练习 |
|----|------|------|
| W10 | 任务 / 队列 / 信号量 | 3 任务：采集+解析+输出 |
| W11 | 中断管理 / 软件定时器 | UART 中断 + 数据队列 |
| W12 | **北斗底层框架整合** | 搭建导航采集工程 |

**北斗采集工程结构**：
```
beidou_collector/
├── cmd/         命令解析
├── gnss/        北斗/GPS 协议（复用阶段1 nmea_parser）
├── driver/      UART / SPI / TIM 驱动
├── os/          FreeRTOS 配置
├── task/        采集 / 解析 / 输出任务
├── util/        环形缓冲区 / 内存池（复用阶段1）
└── doc/         概要设计文档
```

**阶段 2 MATLAB 轨**（对应 JD："MATLAB 仿真分析"）：
- 生成 GPS C/A 码（Gold 序列）→ 理解 FPGA 配码
- 生成北斗 B1I 信号 → 理解 ADC 采样数据样貌
- 多普勒效应仿真 → 理解 PLL 跟踪环
- 干信比 JSR 扫描 → 为阶段 3 抗干扰打基础

**硬件推荐**：

| 方案 | 价格 | 适合 |
|------|------|------|
| STM32F407VGT6 开发板 | ~¥80 | 入门，资料最多 |
| STM32F767 开发板 | ~¥150 | 带 DSP 指令，算法移植 |
| GD32F450 | ~¥50 | 国产替代 |
| 正点原子/野火 GPS 模块 | ~¥60 | 北斗/GPS 双模 NMEA |

### 阶段 3：抗干扰算法 + MATLAB 仿真（第 4 月）

> 对应 JD："导航抗干扰产品软件构建/设计/实现" + "信号与系统/DSP/通信原理"
> 配套仓库：[matlab-anti-jamming](https://github.com/yan123-yan-oops/matlab-anti-jamming)

**抗干扰技能树**：

```
难度    算法               简历价值
⭐      Notch 陷波器         ⭐⭐⭐      ← 一个月拿下
⭐⭐     LMS 自适应滤波       ⭐⭐⭐⭐
⭐⭐⭐    功率倒置 (PI)        ⭐⭐⭐⭐⭐  ← 面试核心
⭐⭐⭐    空频自适应 (SFAP)    ⭐⭐⭐⭐⭐
⭐⭐⭐⭐   RAIM 完好性监测      ⭐⭐⭐⭐
```

| 周 | 算法 | MATLAB 练习 | 对应 C/嵌入式 |
|----|------|------------|-------------|
| W13 | 信号基础 | B1I + C/A 码 + 相关峰 | ADC 采样原理 |
| W14 | Notch 滤波器 | IIR 陷波，频谱对比 | ARM DSP 库实现 |
| W15 | LMS 自适应 | 干扰对消，学习曲线 | 定点 vs 浮点 |
| W16 | 功率倒置 🎯 | 2 阵元 + 方向图 | FPGA 接口 |
| W17 | SFAP + 综合 🎯 | 频域空域联合 | 实时性分析 |
| W18 | **完整仿真工程** | 信号源→干扰→抗干扰→捕获→定位 | 全链路文档 |

**阶段 3 交付物**：
- ✅ 北斗干扰抑制 MATLAB 完整仿真工程（信号生成 + 干扰模型 + 3 种算法 + 频谱报告 + C/N0 曲线）

### 阶段 4：项目整合 + 面试冲刺（第 5-6 月）

**最终作品集结构**：
```
navigator-portfolio/
├── 01-c-demo/           阶段 1：C 模块化 Demo
├── 02-beidou-rtos/      阶段 2：FreeRTOS 北斗采集
├── 03-anti-jamming/     阶段 3：MATLAB 抗干扰仿真
├── 04-interview/        阶段 4：面试题库 + 简历
└── README.md            项目自述（面试官第一眼看这个）
```

---

## 📝 简历项目写法（STAR 框架）

### 项目一：北斗导航数据采集系统

```
FreeRTOS + STM32F4 多任务采集系统
- 搭建 UART DMA 中断框架，采集北斗/GPS NMEA 数据
- 设计环形缓冲区和静态内存池，零 malloc，ISR 安全
- 3 个 RTOS 任务：数据采集 → 协议解析 → 日志输出
- 裸机到 RTOS 迁移，任务间用 xQueue 通信
```

**STAR 回答示例**：
```
S：北斗导航采集项目中，需从 UART 接收 GPS 模块数据
T：数据流要不丢不断，中断和主循环协同一致
A：设计环形缓冲区，中断只管写、主循环只管读，无锁安全
   上层用静态内存池管理消息块，彻底不用 malloc
R：系统连续运行 72 小时无丢包、无内存泄漏
   迁移到 FreeRTOS，只把 rb_put/rb_get 换成 xQueueSend/xQueueReceive
```

### 项目二：导航抗干扰 MATLAB 仿真

```
GNSS 抗干扰算法仿真平台
- 生成北斗 B1I 扩频信号，建立 CW/宽带/脉冲干扰模型
- 实现时域陷波 + LMS 自适应滤波 + 功率倒置 3 种算法
- 完成频谱对比分析，干信比从 40dB 改善至 15dB
- Monte Carlo 蒙特卡洛验证算法统计稳定性
```

---

## 🎯 中森通信面试高频题型

| 类型 | 题目 | 对应模块 |
|------|------|---------|
| **C 基础** | volatile 用途 / const 指针 / static | 01-basics |
| **位操作** | 寄存器置位清位 / 大小端判断 | 03-bitwise |
| **指针** | 函数指针回调 / 指针数组 | 02-pointers |
| **内存** | malloc 陷阱 / 堆栈区别 / 内存对齐 | 04-memory / 09-pool |
| **数据结构** | 手写环形缓冲区 / 链表反转 | 07-ringbuffer |
| **RTOS** | 任务状态 / 优先级反转 / 中断延迟 | 阶段 2 |
| **ARM** | 中断向量表 / DMA / bootloader | 阶段 2 |
| **抗干扰** | LMS vs PI / C/N0 改善 / 阵列方向图 | 阶段 3 |

---

## ⏰ 时间分配建议

### 上班族/学生（每天 ~3h）

```
工作日：
  19:00-20:00   C / ARM 编码（动手为主）
  20:00-20:30   理论 / 文档
  20:30-21:00   MATLAB 练习（穿插进行）

周末：
  上午 3h：集中大块编码（如做 NMEA 解析器）
  下午 2h：MATLAB 仿真
  晚上 1h：写文档 / 整理笔记
```

### 斜杠学习法（对抗遗忘）

```
第 1 天：学新概念（如"指针的指针"）
第 2 天：编码实践（写 10 行用指针的代码）
第 3 天：讲给别人听 / 写笔记
第 7 天：做小练习串起来
```

---

## 📚 推荐资源

### 书籍

| 书名 | 看什么 | 优先级 |
|------|-------|--------|
| 《C Primer Plus》 | C 语言查缺补漏 | ⭐⭐⭐ |
| 《C 和指针》 | 指针专题 | ⭐⭐⭐⭐ |
| 《STM32 嵌入式系统开发实战》 | 外设驱动 | ⭐⭐⭐⭐⭐ |
| 《FreeRTOS 源码与应用》 | RTOS 内部机制 | ⭐⭐⭐⭐⭐ |
| 《GNSS 抗干扰技术》 | 学术原理 | ⭐⭐⭐⭐ |
| 《信号与系统》(奥本海姆) | DSP 基础 | ⭐⭐⭐⭐ |
| 《通信原理》(樊昌信) | 调制/解调/扩频 | ⭐⭐⭐⭐ |

### 网站/视频

| 资源 | 用途 |
|------|------|
| B站：正点原子 STM32 教程 | ARM 外设入门 |
| B站：韦东山 FreeRTOS 教程 | RTOS 实战 |
| MATLAB 官方 Onramp（免费） | MATLAB 零基础 |
| 知网 / IEEE Xplore | 抗干扰论文 |

### 工具链

```
C 编译     : TDM-GCC (已装) → 后期换 ARM GCC
VS Code    : 已配好
MATLAB     : 阶段 3 前装好
STM32 IDE  : STM32CubeIDE (免费，阶段 2 装)
串口工具    : SSCOM / Putty
```

---

## 🔗 相关仓库

- 📡 [matlab-anti-jamming](https://github.com/yan123-yan-oops/matlab-anti-jamming) — 北斗 B1I 抗干扰仿真系统（阶段 3 交付物）

---

## ⚡ VS Code 快捷键

| 快捷键 | 功能 |
|--------|------|
| `Ctrl+Shift+B` | 编译当前文件 |
| `Ctrl+F5` | 运行（不调试） |
| `F5` | 调试 |
| `F9` | 设置断点 |
| `F10` | 单步跳过 |
| `F11` | 单步进入 |

---

> **最后建议**：不要追求"学完再动手"。每个阶段第 1 天就打开 VS Code / MATLAB，敲一行是一行。
> 6 个月后——面试官问的"RTOS 开发经验"和"抗干扰仿真能力"，你都有作品摆在他面前。
>
> 🎯 目标：拿下中森通信嵌入式导航抗干扰岗位！
