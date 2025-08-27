#pragma once
#include <stdint.h>

/* * EazyStart的随机数生成器
 * 以64位整数作为初始种子
 * 使用splitmix64生成4个64位整数作为中间状态
 * 使用xoshiro256**算法生成随机数
 *
 * 注意！本随机数生成器不具备密码学安全性 请勿用于加密或安全相关的场景
 *
 * 种子为惰性加载 会在第一次调用随机数生成函数时自动初始化
 * 也可以使用ezs_random_init函数手动初始化种子
 * 也可以使用ezs_random_init_with_seed函数手动设置种子
 * 除非再次调用上述函数，否则种子不会被改变
 * 使用ezs_random_is_initialized函数可以检查随机数生成器是否已初始化
 * 使用ezs_random_get_current_seed函数可以获取当前的种子值 如果没有初始化则返回0
 *
 * 调用约定：ezs_random_{TYPE}
 * 字符与整型随机数生成函数返回一个范围在[min, max)之间的整数
 * 浮点随机数生成函数返回一个范围在[min, max)之间的浮点数
 * 布尔随机数生成函数不需要参数
 * min >= max为异常情况
 * 在NDEBUG模式下，上述异常情况不会被断言检查
 *
 * 调用约定：ezs_random_{TYPE}_inclusive
 * 字符与整型随机数生成函数返回一个范围在[min, max]之间的整数
 * 浮点随机数生成函数返回一个范围在[min, max]之间的浮点数
 * 注意！浮点随机数生成函数`*_inclusive`在均匀性上存在瑕疵 这是不可避免的且并非实现上的错误
 * min > max为异常情况
 * 在NDEBUG模式下，上述异常情况不会被断言检查
 *
 */

/*
 * ------------------- 开发者注意：关于整数生成的实现细节 -------------------
 *
 * 在 `DEFINE_RANDOM_INTEGER_FUNC` 宏中
 * `const uint64_t range = ((uint64_t) max) - ((uint64_t) min);`
 * 该行的意图比较晦涩
 * 它会依赖于一个实现定义行为：无法在新类型中表示的无符号数转型为有符号数的结果
 * 本项目中其结果必须为：位模式不变，按有符号数重新解释，即二进制补码表示
 *
 * 问题背景：
 * 当 min 和 max 分别为int64类型的最小值和最大值时
 * 直接计算 `max - min` 会导致有符号整数溢出
 * 这是C语言标准中的“未定义行为”（Undefined Behavior）。
 *
 * 我们的解决方案：
 * 1. 我们将 min 和 max 转换为 uint64_t 进行减法。
 * 这利用了无符号整数的回绕特性（这是明确定义的行为）
 * `range` 的计算结果会精确地等于 `UINT64_MAX`
 * 2. 在 `if (UINT64_MAX == range)` 分支中直接返回 `(TYPE) random_next()`。
 *
 * 关键决策点——依赖“实现定义行为”：
 *
 * 将一个 `uint64_t` 的值转换为 `int64_t`，当该值超出 `INT64_MAX` 时，
 * C标准规定其结果是“实现定义行为”（Implementation-Defined Behavior）。
 *
 * 我们选择依赖此行为，是基于以下考量：
 *
 * 它极其可靠：在所有我们关心的现代平台上整数都使用二进制补码表示。
 * 在这些平台上，此转换的行为是固定且可预测的。
 *
 * 这规避了更大的风险：有符号整数溢出引发的“未定义行为”。
 * 我们用一个良性的、可预测的“实现定义行为”
 * 替换了一个灾难性的、不可预测的“未定义行为”。
 *
 * 它兼具性能与简洁性：此方法是所有可行方案中最高效、最简洁的。
 * 任何为了追求理论上100%可移植性而引入的替代方案（例如复杂的算术或联合体类型双关）
 * 都会增加代码的复杂性，而带来的实际收益几乎为零。
 */

/*---------------------------EZS_RANDOM的类型列表宏---------------------------*/

// 整数类型列表宏
#define I_EZS_RANDOM_INTEGER_TYPES_LIST(X) \
    /* 字符类型 */ \
    X(char, char) \
    X(signed char, signed_char) \
    X(unsigned char, unsigned_char) \
    /* 整型类型 */ \
    X(short, short) \
    X(unsigned short, unsigned_short) \
    X(int, int) \
    X(unsigned int, unsigned_int) \
    X(long, long) \
    X(unsigned long, unsigned_long) \
    X(long long, long_long) \
    X(unsigned long long, unsigned_long_long)
// 浮点类型列表宏
#define I_EZS_RANDOM_FLOAT_TYPES_LIST(X) \
    X(float, float) \
    X(double, double) \
    X(long double, long_double)

#define RANDOM_TYPES_LIST(X) \
    I_EZS_RANDOM_INTEGER_TYPES_LIST(X) \
    I_EZS_RANDOM_FLOAT_TYPES_LIST(X)

/*---------------------------EZS_RANDOM的函数声明部分---------------------------*/

// 随机数生成函数声明
#define DECLARE_RANDOM_FUNC(TYPE, SUFFIX) \
[[nodiscard]] TYPE ezs_random_##SUFFIX(TYPE min, TYPE max); \
[[nodiscard]] TYPE ezs_random_##SUFFIX##_inclusive(TYPE min, TYPE max);
RANDOM_TYPES_LIST(DECLARE_RANDOM_FUNC)


// 布尔类型的随机数生成函数声明
[[nodiscard]] bool ezs_random_bool(void);

// 主动初始化随机数生成器
void ezs_random_init(void);

// 使用指定种子主动初始化随机数生成器
void ezs_random_init_with_seed(uint64_t new_seed);

// 检查随机数生成器是否已初始化
[[nodiscard]] bool ezs_random_is_initialized(void);

// 获取当前的随机数种子
[[nodiscard]] uint64_t ezs_random_get_current_seed(void);

/*---------------------------清理局部宏---------------------------*/

#undef RANDOM_TYPES_LIST
#undef DECLARE_RANDOM_FUNC
