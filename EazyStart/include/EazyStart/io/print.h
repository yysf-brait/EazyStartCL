#pragma once

#include <complex.h>
#include <stddef.h>

/*---------------------------EZS_PRINT的类型列表宏---------------------------*/
/* 基本类型 */
#define I_EZS_PRINT_TYPES_LIST_BASE(X) \
    /* 字符类型 */ \
    /* 字符指针在EZS_PRINT中被视为指向单个字符的指针 */ \
    /* 输出字符串应当使用puts等函数 */ \
    X(char, char, "%s(%#hhx)", (isprint((unsigned char)var) ? (char[]){'\'', var, '\'', '\0'} : "<np>"), var) \
    /* (un)signed char类型应被视为数值类型 */ \
    X(signed char, signed_char, "%hhd(%#hhx)", var, (unsigned char)var) \
    X(unsigned char, unsigned_char, "%hhu(%#hhx)", var, var) \
    /* 整型类型 */ \
    X(short, short, "%hd(%#hx)", var, (unsigned short)var) \
    X(unsigned short, unsigned_short, "%hu(%#hx)", var, var) \
    X(int, int, "%d(%#x)", var, (unsigned int)var) \
    X(unsigned int, unsigned_int, "%u(%#x)", var, var) \
    X(long, long, "%ld(%#lx)", var, (unsigned long)var) \
    X(unsigned long, unsigned_long, "%lu(%#lx)", var, var) \
    X(long long, long_long, "%lld(%#llx)", var, (unsigned long long)var) \
    X(unsigned long long, unsigned_long_long, "%llu(%#llx)", var, var) \
    /* 浮点类型 */ \
    X(float, float, "%f(%#a)", var, var) \
    X(double, double, "%f(%#a)", var, var) \
    X(long double, long_double, "%Lf(%#La)", var, var) \
    /* 布尔类型 */ \
    X(bool, bool, "%s", (var ? "true" : "false")) \
    /* nullptr_t类型 */ \
    X(nullptr_t, nullptr_t, "nullptr")
/* 复数类型 */
#if !defined(__STDC_NO_COMPLEX__)
#define I_EZS_PRINT_TYPES_LIST_COMPLEX(X) \
    X(float complex, float_complex, "%f + %fi", crealf(var), cimagf(var)) \
    X(double complex, double_complex, "%f + %fi", creal(var), cimag(var)) \
    X(long double complex, long_double_complex, "%Lf + %Lfi", creall(var), cimagl(var))
#else
#define I_EZS_PRINT_TYPES_LIST_COMPLEX(X)
#endif

/* 虚数类型 */
#if defined(_Imaginary_I)
#define I_EZS_PRINT_TYPES_LIST_IMAGINARY(X) \
    X(float imaginary, float_imaginary, "%fi", cimagf(var)) \
    X(double imaginary, double_imaginary, "%fi", cimag(var)) \
    X(long double imaginary, long_double_imaginary, "%Lfi", cimagl(var))
#else
#define I_EZS_PRINT_TYPES_LIST_IMAGINARY(X)
#endif

// 类型列表宏合并
#define I_EZS_PRINT_TYPES_LIST(X) \
    I_EZS_PRINT_TYPES_LIST_BASE(X) \
    I_EZS_PRINT_TYPES_LIST_COMPLEX(X) \
    I_EZS_PRINT_TYPES_LIST_IMAGINARY(X)

/*---------------------------EZS_PRINT的函数声明部分---------------------------*/
// 常规类型的打印函数声明
#define DECLARE_PRINT_FUNC(TYPE, FUNCTION_SUFFIX, ...) \
    void i_ezs_print_##FUNCTION_SUFFIX(const char *file, const int line, const char *name, const TYPE *ptr); \
    void i_ezs_print_pointer_##FUNCTION_SUFFIX(const char *file, const int line, const char *name, TYPE* const *ptr); \
    void i_ezs_print_const_pointer_##FUNCTION_SUFFIX(const char *file, const int line, const char *name, const TYPE* const *ptr);
I_EZS_PRINT_TYPES_LIST(DECLARE_PRINT_FUNC)

// 特例类型的打印函数声明
// void*类型的打印函数声明
void i_ezs_print_pointer_void(const char *file, int line, const char *name, void *const *ptr);

// const void*类型的打印函数声明
void i_ezs_print_const_pointer_void(const char *file, int line, const char *name, const void *const *ptr);

// 不支持的类型打印函数声明
void i_ezs_print_unsupported(const char *file, int line, const char *name, const void *ptr);


/*---------------------------EZS_PRINT---------------------------*/
// 根据类型，转发至不同的实现
#define I_EZS_PRINT_GENERATE_GENERIC_ENTRIES(TYPE, SUFFIX, FORMAT, ...) \
    TYPE: i_ezs_print_##SUFFIX, \
    TYPE*: i_ezs_print_pointer_##SUFFIX, \
    const TYPE*: i_ezs_print_const_pointer_##SUFFIX,
#define I_EZS_PRINT_GENERATE_GENERIC_BLOCK(file, line, name, ptr) (_Generic((*ptr), \
    I_EZS_PRINT_TYPES_LIST(I_EZS_PRINT_GENERATE_GENERIC_ENTRIES) \
    /* 特例类型处理 */ \
    void*: i_ezs_print_pointer_void, const void*: i_ezs_print_const_pointer_void, \
    default: i_ezs_print_unsupported \
))(file, line, (name), ptr)

// 如果你发现在此处编译报错，说明你传入的变量不是左值，或者无法获取其地址。
// 你可以尝试将其提升为一个变量，而不是直接传入一个表达式。
#define I_EZS_PRINT_MUST_BE_LVALUE_AND_GET_ADDR(var) (&(var))
// EZS_PRINT: 一个用于打印变量的全自动函数[仅支持常见类型的左值]
#define EZS_PRINT(var) \
do { \
    I_EZS_PRINT_GENERATE_GENERIC_BLOCK(__FILE__, __LINE__, (#var), I_EZS_PRINT_MUST_BE_LVALUE_AND_GET_ADDR(var)); \
} while (false)

/*---------------------------清理局部宏---------------------------*/
#undef DECLARE_PRINT_FUNC
