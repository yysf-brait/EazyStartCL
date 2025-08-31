# EasyStart (EZS) 🚀

![Language](https://img.shields.io/badge/Language-C23-blue.svg)
![Build System](https://img.shields.io/badge/Build-CMake-orange.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20(WSL)-lightgrey.svg)

**高质量的现代C语言入门辅助库。它提供了各种实用的功能，旨在为计算机科学学习提供一个轻松的开始 (An Easy Start)！**

---

## 🤔 EZS是什么？

`EasyStart` (简称 EZS) 是一个专为正在学习C语言的计算机科学学生设计的辅助库。我们深知C语言入门阶段的痛点：恼人的中文乱码、难以理解的`printf`/`scanf`、复杂的环境配置、从基础语法到数据结构之间的巨大鸿沟、过于单薄的标准库功能......

EZS旨在**扫清这些与C语言核心知识无关的障碍**，让学生能从第一天起就专注于真正重要的概念，如变量、控制流、指针和内存，并以一种平滑、专业的方式过渡到更高级的课程和真实的软件开发。

EZS也可以为你自己的C语言项目提供一个坚实的基础，帮助你快速搭建起一个高效、现代的C语言开发环境，提供若干实用工具，提升你的开发效率。

## 💡 核心哲学：做“脚手架”，而非“温室”

与其他一些教学库试图完全隐藏C语言复杂性不同，EZS遵循一套清晰的设计哲学：

*   **辅助而非替代**: EZS帮你解决初期烦人的环境问题（如UTF-8），提供比标准库更安全的工具（如`ezs_input`），但它从不隐藏C语言的本质。`EZS_PRINT`会把指针的地址打出来，让你直面内存。
*   **拥抱标准工具链**: 我们不提供“一键式”的黑盒环境。EZS被设计为与 **CLion + CMake + GCC** 这一现代、专业的开发环境无缝协作，让你学习到的技能可以直接迁移到未来的工作和项目中。
*   **代码即是教材**: EZS的每一行代码都以最高质量标准编写，并附有详尽的注释。我们鼓励你打开它的源代码，学习一个真实的、高质量的、跨平台的C语言项目是什么样的。

## ✨ 功能特性

EZS提供了一个精心挑选的工具集，以应对C语言学习曲线中最陡峭的部分：

*   **1. 零配置UTF-8环境**: 只需`#include "EazyStart/ezs.h"`，即可在Windows上自动解决控制台的中文输入输出乱码问题。
*   **2. 全能变量诊断宏 (`EZS_PRINT`)**: 一行代码，清晰打印出变量的类型、地址、大小和值，帮助你建立变量与内存之间的直观联系。
*   **3. 安全健壮的输入函数 (`ezs_input_*`)**: 完全替代`scanf`，从根本上杜绝缓冲区污染、类型不匹配等“玄学”问题，并提供友好的错误提示。
*   **4. 无缝集成专业数据结构库 (`STC`)**: 直接使用业界顶级的C语言泛型容器库（动态数组、哈希表、集合等），无需关心复杂的构建配置，让你在数据结构课程中如虎添翼。
*   **5. 高质量随机数生成器 (`ezs_random_*`)**: 提供一个现代、无偏、易于使用的随机数工具，远胜于C标准库的`rand()`。
*   **6. 跨平台高精度时钟 (`ezs_clock`)**: 为性能测试等高级功能提供坚实的、可移植的时间测量基础。
*   **7. 极简代码性能基准测试 (`ezs_benchmark`)**: 只需`start`/`end`两行代码，即可对你的算法或函数进行精确的性能分析，并在程序结束时自动生成精美报告。

## 🚀 快速开始

在你的C语言项目中使用EZS极其简单。

### 1. 目录结构

我们推荐如下的项目结构：

```
YourProject/
├── EazyStart/        <-- 将整个EZS项目文件夹复制到这里
│   ├── include/
│   ├── src/
│   ├── third_party/
│   ├── CMakeLists.txt
│   └── ...
├── CMakeLists.txt    <-- 你的主项目CMake配置文件
└── main.c            <-- 你的源代码
```

### 2. 配置CMake

在你的主项目 `CMakeLists.txt` 中，大部分由CLion自动生成，只需添加 **最后两行** 即可完成所有配置：

```cmake
cmake_minimum_required(VERSION 3.23)
project(YourProject C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(YourProject main.c) # CLion会自动维护你的源文件列表

# --- 只需添加这两行 ---
add_subdirectory(EazyStart)
target_link_libraries(YourProject PRIVATE EazyStart)
```

### 3. 编写代码

在你的 `main.c` 中，包含EZS的头文件并开始使用！

```c
#include "EazyStart/ezs.h" // 引入EZS核心功能
#include <stdio.h>

int main(void) {
    puts("你好，EZS！"); // 中文将正确显示

    int my_var = 123;
    EZS_PRINT(my_var); // 轻松查看变量信息

    int age = ezs_input_int_with_prompt("请输入您的年龄: ");
    puts("你的年龄被记录下来了！在下面打印出来:");
    EZS_PRINT(age);

    return 0;
}
```

现在，在CLion中点击“Build”和“Run”，一切都将如你所愿地工作！

## 🛠️ 功能展示

<details>
<summary><b>点击展开: EZS_PRINT 变量诊断宏</b></summary>

```c
#include "EazyStart/ezs.h"

int main() {
    int student_id = 202401;
    double pass_rate = 98.5;
    char grade = 'A';
    char* char_ptr = &grade;
    auto null_ptr = nullptr;

    EZS_PRINT(student_id);
    EZS_PRINT(pass_rate);
    EZS_PRINT(char_ptr);
    EZS_PRINT(null_ptr);
}
```

**输出:**
```
[EZS ...\main.c:9] -> [student_id at 0x...]:int(4 bytes) = 202401(0x316a1)
[EZS ...\main.c:10] -> [pass_rate at 0x...]:double(8 bytes) = 98.500000(0x1.8a00000000000p+6)
[EZS ...\main.c:11] -> [char_ptr at 0x...]:char*(8 bytes) = 0x... -> ['A'(0x41)]
[EZS ...\main.c:12] -> [null_ptr at 0x...]:nullptr_t*(8 bytes) = nullptr
```

</details>

<details>
<summary><b>点击展开: STC 数据结构库集成</b></summary>

```c
#include <stc/cstr.h>      // 直接使用STC库

#define T SSet, cstr, (c_keypro)
#include <stc/sortedset.h> // 使用STC的排序集合

int main(void)
{
    SSet second = c_make(SSet, {"red", "green", "blue"});
    SSet third = {}, fourth = {}, fifth = {};

    for (c_items(i, const char*, {"orange", "pink", "yellow"}))
        SSet_emplace(&third, *i.ref);

    SSet_emplace(&fourth, "potatoes");
    SSet_emplace(&fourth, "milk");
    SSet_emplace(&fourth, "flour");

    // Copy all to fifth:
    fifth = SSet_clone(second);
    c_foreach(i, SSet, third) SSet_emplace(&fifth, cstr_str(i.ref));
    c_foreach(i, SSet, fourth) SSet_emplace(&fifth, cstr_str(i.ref));
    c_foreach(i, SSet, fifth) printf("- %s\n", cstr_str(i.ref));
    c_drop(SSet, &second, &third, &fourth, &fifth);
}
```
**输出:**
```
- blue
- flour
- green
- milk
- orange
- pink
- potatoes
- red
- yellow
```
</details>

<details>
<summary><b>点击展开: Benchmark 性能基准测试</b></summary>

```c
#include "EazyStart/ezs.h"
#include "EazyStart/time/benchmark.h"

int main() {
    // 任务会执行5次，Benchmark会自动统计
    for (int i = 0; i < 5; ++i) {
        ezs_benchmark_start("Simple Summation");
        volatile long long sum = 0;
        for (int j = 0; j < 1000 * 1000; ++j) {
            sum += j;
        }
        ezs_benchmark_end("Simple Summation");
    }
    // 报告将在程序退出时自动打印
}
```
**程序退出时自动打印的报告:**
```
┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                       Benchmark Result Table                                                       │
├─────────────────────┬────────────┬──────────────────────┬──────────────────────┬──────────────────────┬──────────────────┬─────────┤
│Benchmark Name       │      Count │                 Mean │                  Min │                  Max │          Std Dev │      RSD│
├─────────────────────┼────────────┼──────────────────────┼──────────────────────┼──────────────────────┼──────────────────┼─────────┤
│Simple Summation     │          5 │           748us953ns │           587us704ns │           896us514ns │        0.000123s │   16.36%│
└─────────────────────┴────────────┴──────────────────────┴──────────────────────┴──────────────────────┴──────────────────┴─────────┘
```
</details>

## 🖥️ 推荐环境

为了获得最佳体验并学习到最符合行业标准的实践，我们强烈推荐以下开发环境：

*   **操作系统**: Windows 10/11 with WSL2, 或者原生 Linux / macOS。
*   **编译器**: **GCC** (通过 MinGW-w64 on Windows 或 build-essential on Linux)。EZS使用了C23标准的一些特性。
*   **构建系统**: **CMake** (版本 3.23+)。
*   **IDE**: **JetBrains CLion**。它与CMake的无缝集成为本项目提供了最流畅的开发体验。

## 🤝 贡献

欢迎任何形式的贡献！无论是发现BUG、提出新功能建议还是提交代码，请通过 Issue 或 PR 与我们联系。

## 📜 许可证

本项目采用 [MIT License](./LICENSE) 开源。