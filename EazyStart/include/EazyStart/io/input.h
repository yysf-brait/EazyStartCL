#pragma once

#include <limits.h>

/* * 输入规则：
 * 前导的空白字符会被合法地忽略
 * 换行符被视为输入结束符
 * 未经转换的多余字符仅允许空白字符
 * char类型的输入会取第一个字符，且不会忽略任意空白字符
 */

/*---------------------------EZS_INPUT的类型列表宏---------------------------*/
#define I_EZS_INPUT_TYPES_LIST(X) \
    X(signed char, signed_char, strtol, (result < SCHAR_MIN || result > SCHAR_MAX), 10) \
    X(unsigned char, unsigned_char, strtoul, (result > UCHAR_MAX), 10) \
    X(short, short, strtol, (result < SHRT_MIN || result > SHRT_MAX), 10) \
    X(unsigned short, unsigned_short, strtoul, (result > USHRT_MAX), 10) \
    X(int, int, strtol, (result < INT_MIN || result > INT_MAX), 10) \
    X(unsigned int, unsigned_int, strtoul, (result > UINT_MAX), 10) \
    X(long, long, strtol, (false), 10) \
    X(unsigned long, unsigned_long, strtoul, (false), 10) \
    X(long long, long_long, strtoll, (false), 10) \
    X(unsigned long long, unsigned_long_long, strtoull, (false), 10) \
    X(float, float, strtof, (false)) \
    X(double, double, strtod, (false)) \
    X(long double, long_double, strtold, (false))

/*---------------------------EZS_PRINT的函数声明部分---------------------------*/
// 常规类型的输入函数声明
#define DECLARE_INPUT_FUNC(TYPE, SUFFIX, ...) \
    [[nodiscard]] TYPE ezs_input_##SUFFIX##_with_prompt(const char *prompt); \
    [[nodiscard]] TYPE ezs_input_##SUFFIX(void);
I_EZS_INPUT_TYPES_LIST(DECLARE_INPUT_FUNC)

// 特例类型的输入函数声明

// 字符类型的输入函数
// 读取第一个字符，无论它是否是空白字符
// 换行符前不允许有多余字符（无论它们是否是空白字符）
// 直接输入换行符以表示输入'\n'
[[nodiscard]] char ezs_input_char_with_prompt(const char *prompt);

[[nodiscard]] char ezs_input_char(void);

// 布尔类型的输入函数
// 输入Y/T/1表示true，N/F/0表示false（大小写不敏感）
[[nodiscard]] bool ezs_input_bool_with_prompt(const char *prompt);

[[nodiscard]] bool ezs_input_bool(void);

/*---------------------------清理局部宏---------------------------*/
#undef DECLARE_INPUT_FUNC
