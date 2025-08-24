#include <stdio.h>
#include <inttypes.h>
#include "EazyStart/ezs.h"

/**
 * @file main.c
 * @brief EasyStart (EZS) 项目功能展示
 *
 * 本文件旨在演示EZS库的核心功能，包括：
 * 1. EZS_PRINT: 全能的变量信息打印宏。
 * 2. ezs_input_...: 安全、健壮的输入函数。
 * 3. ezs_random_...: 高质量、易于使用的随机数生成器。
 * 4. 自动控制台字符集设置（通过#include "EazyStart/ezs.h" 自动生效）。
 *
 * 如何编译运行:
 * 1. 确保此 main.c 与 EazyStart 子目录位于同一项目根目录下。
 * 2. 使用配置好的CMake + CLion (或等效环境) 构建项目。（Windows需要使用MingW）
 * 3. 运行生成的可执行文件。
 */

// 只需要包含一个头文件"EazyStart/ezs.h"，即可启用EasyStart的所有功能。


int main(void) {
    // --- (功能零) 自动设置控制台为UTF-8 ---
    // 注意：你不需要为处理中文字符做任何额外操作！
    // 仅仅因为我们包含了 "EazyStart/ezs.h"，EZS库就会在main函数执行前，
    // 自动将Windows控制台的输入输出设置为UTF-8编码，从而完美解决中文乱码问题。
    // 下面的所有中文提示和输出能够正确显示，都得益于这个“隐形”的功臣。
    puts("=============== EasyStart (EZS) 项目功能演示 ===============");
    puts("您好！欢迎使用EZS辅助库。下面的所有中文都应能正确显示。");


    // --- 1. EZS_PRINT 变量诊断宏功能展示 ---
    puts("\n--- 1. EZS_PRINT 变量诊断宏功能展示 ---");
    puts("EZS_PRINT可以一行代码清晰地打印出变量的各种信息。");
    puts("支持所有基础类型（如虚数/复数支持，则也包括它们），以及指向它们的指针。");

    int student_id = 202401;
    double pass_rate = 98.5;
    char grade = 'A';
    char non_printable_char = '\t'; // 一个制表符，是不可打印字符
    bool is_finished = true;
    char *char_ptr = &grade; // 指向grade的指针
    void *void_ptr = &non_printable_char; // void指针
    auto null_ptr = nullptr;
    nullptr_t *null_ptr_ptr = &null_ptr; // 指向nullptr的指针

    EZS_PRINT(student_id);
    EZS_PRINT(pass_rate);
    EZS_PRINT(grade);
    EZS_PRINT(non_printable_char); // 注意它如何智能地处理不可打印字符
    EZS_PRINT(is_finished);
    EZS_PRINT(char_ptr); // 注意它如何处理指针：同时显示地址和指向的内容
    EZS_PRINT(void_ptr); // void指针不会解引用
    EZS_PRINT(null_ptr); // 正确地处理C23的nullptr
    EZS_PRINT(null_ptr_ptr); // 指向nullptr的指针


    // --- 2. 输入辅助功能 (ezs_input_...) 展示 ---
    puts("\n--- 2. 输入辅助功能 (ezs_input_...) 展示 ---");
    puts("EZS的输入函数安全可靠，能自动处理错误输入并提示用户重新输入。");
    puts("支持所有基础类型的输入，包括整数、浮点数、字符和布尔值。");
    puts("详细输入规则请检阅 EazyStart/include/EazyStart/input.h 中的注释。");

    // 演示输入一个整数
    int user_age = ezs_input_int(); // 将使用默认提示语
    printf("你输入了年龄，让我们用EZS_PRINT来确认一下：\n");
    EZS_PRINT(user_age);

    // 演示输入一个布尔值
    // 尝试输入 Y, N, T, F, 1, 0，甚至是错误的内容看看效果
    bool need_continue = ezs_input_bool_with_prompt("你希望继续下一个演示吗? (Y/N): ");
    printf("你的选择是：\n");
    EZS_PRINT(need_continue);

    if (!need_continue) {
        puts("演示提前结束。");
        return 0;
    }


    // --- 3. 随机数生成功能 (ezs_random_...) 展示 ---
    puts("\n--- 3. 随机数生成功能 (ezs_random_...) 展示 ---");
    puts("EZS的随机数生成器质量高且使用简单，第一次调用时会自动初始化。");

    // 演示生成 [1, 6] 范围内的整数，就像掷骰子
    const int dice_roll = ezs_random_int(1, 6);
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
    // 更多控制随机数生成器种子的函数请查阅 EazyStart/include/EazyStart/random.h 中的注释。


    puts("\n=============== 所有功能演示完毕，感谢使用！ ===============");

    return 0;
}
