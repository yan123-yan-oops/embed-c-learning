# 嵌入式 C 学习环境

配合 6 个月「导航抗干扰嵌入式」学习计划使用。

## 📂 目录结构

```
embed-c-learning/
├── .vscode/                    # VS Code / Trae CN 配置
│   ├── c_cpp_properties.json   # C 语言智能提示配置
│   ├── tasks.json              # 编译/运行任务（Ctrl+Shift+B）
│   └── extensions.json         # 推荐插件清单
│
├── 01-basics/                  # C 基础
│   ├── hello_embed.c           # 基本结构、数据类型、volatile/static
│   └── ...（你添加的练习）
│
├── 02-pointers/                # 指针
│   └── pointers_intro.c        # 指针、数组、函数指针
│
├── 03-bitwise/                 # 位操作
│   └── bit_ops.c               # GPIO 置位/清位/翻转、寄存器位域
│
├── 04-memory/                  # 内存管理
│   └── memory_layout.c         # 栈/堆/全局区/常量区 + 内存池
│
└── README.md                   # 本文件
```

## ⚙️ 环境搭建步骤

### 第 1 步：安装 GCC 编译器（三选一）

**推荐方案 A — MinGW-w64（最常用）**
1. 下载：https://github.com/niXman/mingw-builds-binaries/releases
2. 选择最新版 `x86_64-14.2.0-release-win32-seh-ucrt-rt_v12-rev0.7z`
3. 解压到 `C:\mingw64\`
4. 将 `C:\mingw64\bin` 添加到系统 PATH 环境变量
5. 验证：打开终端运行 `gcc --version`

**方案 B — TDM-GCC（一键安装）**
1. 下载：https://jmeubank.github.io/tdm-gcc/
2. 运行安装程序，一路 Next
3. 自动添加 PATH

**方案 C — MSYS2（最完整）**
1. 下载：https://www.msys2.org/
2. 安装后打开 MSYS2 UCRT64 终端
3. 运行：`pacman -S mingw-w64-ucrt-x86_64-gcc`
4. 将 `C:\msys64\ucrt64\bin` 添加到 PATH

### 第 2 步：在 VS Code / Trae CN 中打开本文件夹

```
文件 → 打开文件夹 → 选择 embed-c-learning
```

### 第 3 步：安装推荐的扩展

按 `Ctrl+Shift+X`，搜索安装：
- **C/C++** (ms-vscode.cpptools) — 必装
- **C/C++ Extension Pack** (ms-vscode.cpptools-extension-pack)

### 第 4 步：编译运行

- 打开任意 `.c` 文件
- 按 `Ctrl+Shift+B` → 选择 "build current file"
- 或在终端手动编译：`gcc -Wall -std=c11 hello.c -o hello.exe && hello.exe`

## 📚 学习路线（配合你的 6 月计划）

| 阶段 | 对应目录 | 内容 |
|------|----------|------|
| 第 1 周 | 01-basics | 数据类型、volatile、static、printf调试 |
| 第 2 周 | 02-pointers | 指针、数组、字符串、函数指针 |
| 第 3 周 | 03-bitwise | 位运算、寄存器操作宏、位域 |
| 第 4 周 | 04-memory | 内存布局、堆栈、内存池、malloc陷阱 |
| 第 5-6 周 | 自行扩展 | 结构体、链表、队列、环形缓冲区 |

## 🔧 VS Code 快捷键

| 快捷键 | 功能 |
|--------|------|
| `Ctrl+Shift+B` | 编译当前文件 |
| `Ctrl+F5` | 运行（不调试） |
| `F5` | 调试（需要配置 launch.json） |
| `F9` | 设置断点 |
| `F10` | 单步跳过 |
| `F11` | 单步进入 |
