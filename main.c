/**
 * @file main.c
 * @brief EasyStart (EZS) 项目全功能展示
 * @version 1.0
 *
 * 本文件旨在全面、清晰地演示EZS库的所有核心功能。
 * 它既是一个功能测试文件，也是一份写给最终用户的“快速上手指南”。
 *
 * ======================= EZS 核心功能一览 =======================
 * 1. 自动控制台字符集设置: (零配置) 引入头文件即可解决Windows中文乱码。
 * 2. EZS_PRINT: (诊断工具) 全能的变量信息打印宏，为初学者洞察变量本质。
 * 3. ezs_input_...: (安全基石) 安全、健壮的交互式输入函数，杜绝scanf陷阱。
 * 4. STC库无缝集成: (工程实践) 直接使用高质量第三方数据结构库，无需关心构建细节。
 * 5. ezs_random_...: (现代工具) 高质量、易用的现代伪随机数生成器。
 * 6. ezs_clock: (底层支撑) 跨平台的高精度时钟API (作为benchmark模块的基石)。
 * 7. ezs_benchmark: (性能度量) 极其易用的代码性能基准测试工具。
 * ================================================================
 *
 * 如何编译运行:
 * 1. 确保此 main.c 与 EazyStart 子目录位于同一项目根目录下。
 * 2. 使用配置好的CMake + CLion (或等效环境) 构建项目。
 * 3. 运行生成的可执行文件，并跟随提示进行交互。
 */

// 只需要包含一个头文件，即可启用EasyStart的大部分功能。
#include "EazyStart/ezs.h"

// 特定功能模块的头文件可以按需引入。
#include "EazyStart/time/benchmark.h"

// STC库的头文件也可以直接引入，EZS的构建系统已为你处理好一切。
#include <stc/cstr.h>
#define T SSet, cstr, (c_keypro)
#include <stc/sortedset.h>

#include <stdio.h>
#include <inttypes.h> // 用于 PRIx64 宏


// --- 函数声明 ---
void demo_print(void);

void demo_input(void);

void demo_random(void);

void demo_benchmark(void);

void demo_stc(void);

int main(void) {
    // --- (功能 #1) 自动设置控制台为UTF-8 ---
    // 注意：你不需要为处理中文字符做任何额外操作！
    // 仅仅因为我们包含了 "EazyStart/ezs.h"，EZS库就会在main函数执行前，
    // 自动将Windows控制台的输入输出设置为UTF-8编码，从而完美解决中文乱码问题。
    // 下面的所有中文提示和输出能够正确显示，都得益于这个“隐形”的功臣。
    puts("=============== EasyStart (EZS) 项目全功能演示 ===============");
    puts("您好！欢迎使用EZS辅助库。下面的所有中文都应能正确显示。");
    puts("==============================================================");

    // --- 功能演示区 ---
    demo_print();
    demo_random();
    demo_benchmark();
    demo_stc();
    demo_input(); // 输入部分有可能会提前终止程序，故放在最后

    puts("\n=============== 所有功能演示完毕，感谢使用！ ===============");
    puts("请注意，Benchmark的统计报告将在程序退出时自动打印。");

    return 0;
}

/**
 * @brief 演示EZS_PRINT变量诊断宏
 */
void demo_print(void) {
    puts("\n--- [演示 #2] EZS_PRINT 变量诊断宏 ---");
    puts("EZS_PRINT可以一行代码清晰地打印出变量的类型、地址、大小和值。");
    puts("支持所有基础类型，以及指向它们的指针。");

    int student_id = 202401;
    double pass_rate = 98.5;
    char grade = 'A';
    char non_printable_char = '\t'; // 一个制表符，是不可打印字符
    bool is_finished = true;
    char *char_ptr = &grade; // 指向grade的指针
    void *void_ptr = &non_printable_char; // void指针
    auto null_ptr = nullptr;

    EZS_PRINT(student_id);
    EZS_PRINT(pass_rate);
    EZS_PRINT(grade);
    EZS_PRINT(non_printable_char); // 注意它如何智能地处理不可打印字符
    EZS_PRINT(is_finished);
    EZS_PRINT(char_ptr); // 注意它如何处理指针：同时显示地址和指向的内容
    EZS_PRINT(void_ptr); // void指针不会解引用
    EZS_PRINT(null_ptr); // 正确地处理C23的nullptr
}

/**
 * @brief 演示高质量随机数生成器
 */
void demo_random(void) {
    puts("\n--- [演示 #5] ezs_random_* 高质量随机数生成器 ---");
    puts("EZS的随机数生成器质量高且使用简单，第一次调用时会自动初始化。");

    // 演示生成 [1, 6] 范围内的整数，就像掷骰子
    const int dice_roll = ezs_random_int_inclusive(1, 6);
    printf("随机掷骰子 (1-6): %d\n", dice_roll);

    // 演示生成 [0.0, 100.0) 范围内的浮点数，可用于模拟分数
    const double random_score = ezs_random_double(0.0, 100.0);
    printf("随机生成一个分数 (0.0-100.0): %f\n", random_score);

    // 演示生成一个随机的布尔值，就像抛硬币
    const bool coin_toss = ezs_random_bool();
    printf("随机抛硬币: %s\n", coin_toss ? "正面" : "反面");

    // 你可以获取当前使用的种子，方便复现随机序列
    const uint64_t current_seed = ezs_random_get_current_seed();
    printf("本次随机序列使用的种子是: 0x%016" PRIx64 "\n", current_seed);
}

/**
 * @brief 演示代码性能基准测试工具
 */
void demo_benchmark(void) {
    puts("\n--- [演示 #7] ezs_benchmark 代码性能基准测试 ---");
    puts("EZS Benchmark极其易用，只需start/end即可度量代码性能。");
    puts("它会自动统计调用次数、平均/最小/最大耗时、标准差等，并在程序结束时打印报告。");

    // 演示1：测试一个简单的循环任务
    puts("正在执行一个简单的求和任务5次...");
    for (int i = 0; i < 5; ++i) {
        volatile long long sum = 0; // 使用volatile防止编译器优化掉循环
        ezs_benchmark_start("Simple Summation");
        for (int j = 0; j < 1000 * 1000; ++j) {
            sum += j;
        }
        ezs_benchmark_end("Simple Summation");
    }

    // 演示2：测试一个高频、短耗时的任务
    puts("正在执行一百万次随机布尔值生成...");
    ezs_benchmark_start("1M Random Bools");
    volatile bool temp = false; // 使用volatile防止编译器优化掉调用
    for (int i = 0; i < 1000 * 1000; ++i) {
        temp = ezs_random_bool();
    }
    ezs_benchmark_end("1M Random Bools");
    printf("最后生成的随机布尔值是: %s\n", temp ? "true" : "false");
    puts("Benchmark数据已记录。");
}

/**
 * @brief 演示STC库的无缝集成
 */
void demo_stc(void) {
    puts("\n--- [演示 #4] STC (Standard C Containers) 库无缝集成 ---");
    puts("EZS已为你处理好STC库的编译和链接，你只需#include并使用即可！");
    puts("下面是一个使用STC的排序集合(SSet)的例子：");

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

    printf("\n合并后的排序集合 'fifth' 包含:\n");
    c_foreach(i, SSet, fifth) printf("- %s\n", cstr_str(i.ref));

    c_drop(SSet, &second, &third, &fourth, &fifth);
    puts("\nSTC演示完毕，内存已自动清理。");
}

/**
 * @brief 演示安全、健壮的交互式输入函数
 */
void demo_input(void) {
    puts("\n--- [演示 #3] ezs_input_* 安全输入函数 ---");
    puts("EZS的输入函数安全可靠，能自动处理错误输入并提示用户重新输入。");
    puts("它从根本上解决了scanf带来的缓冲区污染等问题。");
    puts("详细输入规则请检阅 EazyStart/include/EazyStart/io/input.h 中的注释。");

    // 演示输入一个整数
    int user_age = ezs_input_int(); // 将使用默认提示语
    printf("你输入了年龄，让我们用EZS_PRINT来确认一下：\n");
    EZS_PRINT(user_age);

    // 演示输入一个布尔值
    // 尝试输入 Y, N, T, F, 1, 0，甚至是错误的内容看看效果
    bool need_exit = ezs_input_bool_with_prompt("你希望就此结束演示吗? (Y/N): ");
    printf("你的选择是：\n");
    EZS_PRINT(need_exit);

    if (need_exit) {
        puts("演示提前结束。");
        // 注意：由于我们在这里提前退出，你将看到Benchmark报告。
        // 如果想看到所有演示，请在上一步选择 'N'。
        exit(0);
    }
}
