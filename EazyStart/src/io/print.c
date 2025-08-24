#include "EazyStart/io/print.h"
#include <ctype.h>
#include <stdio.h>

/*---------------------------EZS_PRINT的常规类型函数定义部分---------------------------*/
// 非指针类型的打印函数模板
#define DEFINE_PRINT_FUNC_FOR_TYPE(TYPE, FUNCTION_SUFFIX, FORMAT_SPECIFIER, ...) \
    inline void i_ezs_print_##FUNCTION_SUFFIX(const char *file, const int line, \
                                              const char *name, const TYPE *ptr) { \
        const TYPE var = *ptr; \
        printf("[EZS %s:%d] -> [%s at 0x%p]:"#TYPE"(%zu bytes) = "FORMAT_SPECIFIER"\n", \
            file, line, name, ptr, sizeof(var) __VA_OPT__(, __VA_ARGS__)); \
    }

// 指针类型打印打印函数模板
#define DEFINE_PRINT_FUNC_FOR_POINTER_IMPL(file, line, name, ptr, TYPE, FORMAT_SPECIFIER, ...) \
    if (nullptr == *(ptr)) { \
        printf("[EZS %s:%d] -> [%s at 0x%p]:"#TYPE"*(%zu bytes) = nullptr\n", \
            file, line, name, ptr, sizeof(*ptr)); \
    } else { \
        [[maybe_unused]] const TYPE var = **ptr; \
        printf("[EZS %s:%d] -> [%s at 0x%p]:"#TYPE"*(%zu bytes) = 0x%p -> ["FORMAT_SPECIFIER"]\n", \
            file, line, name, ptr, sizeof(*ptr), *ptr __VA_OPT__(, __VA_ARGS__)); \
    }
// non-const指针类型打印函数
#define DEFINE_PRINT_FUNC_FOR_POINTER(TYPE, FUNCTION_SUFFIX, FORMAT_SPECIFIER, ...) \
    inline void i_ezs_print_pointer_##FUNCTION_SUFFIX(const char *file, const int line, \
                                                      const char *name, TYPE* const *ptr) { \
        DEFINE_PRINT_FUNC_FOR_POINTER_IMPL(file, line, name, ptr, TYPE, FORMAT_SPECIFIER, __VA_ARGS__); \
    }
// const指针类型打印函数
#define DEFINE_PRINT_FUNC_FOR_CONST_POINTER(TYPE, FUNCTION_SUFFIX, FORMAT_SPECIFIER, ...) \
    inline void i_ezs_print_const_pointer_##FUNCTION_SUFFIX(const char *file, const int line, \
                                                            const char *name, const TYPE* const *ptr) { \
        DEFINE_PRINT_FUNC_FOR_POINTER_IMPL(file, line, name, ptr, TYPE, FORMAT_SPECIFIER, __VA_ARGS__); \
    }

// 以TYPE批量定义打印函数（TYPE类型、TYPE*类型、const TYPE*类型）
#define DEFINE_PRINT_FUNC(TYPE, FUNCTION_SUFFIX, FORMAT_SPECIFIER, ...) \
    DEFINE_PRINT_FUNC_FOR_TYPE(TYPE, FUNCTION_SUFFIX, FORMAT_SPECIFIER, __VA_ARGS__) \
    DEFINE_PRINT_FUNC_FOR_POINTER(TYPE, FUNCTION_SUFFIX, FORMAT_SPECIFIER, __VA_ARGS__) \
    DEFINE_PRINT_FUNC_FOR_CONST_POINTER(TYPE, FUNCTION_SUFFIX, FORMAT_SPECIFIER, __VA_ARGS__)
// 批量生成打印函数
#define GENERATE_PRINT_FUNCTIONS(TYPE, SUFFIX, FORMAT, ...) \
    DEFINE_PRINT_FUNC(TYPE, SUFFIX, FORMAT, __VA_ARGS__)
I_EZS_PRINT_TYPES_LIST(GENERATE_PRINT_FUNCTIONS)

/*---------------------------EZS_PRINT的特殊类型函数定义部分---------------------------*/
// void*类型打印函数
#define DEFINE_PRINT_FUNC_FOR_VOID_POINTER_IMPL(file, line, name, ptr) \
    if (nullptr == *(ptr)) { \
        printf("[EZS %s:%d] -> [%s at 0x%p]:void*(%zu bytes) = nullptr\n", \
            file, line, name, ptr, sizeof(*ptr)); \
    } else { \
        printf("[EZS %s:%d] -> [%s at 0x%p]:void*(%zu bytes) = 0x%p\n", \
            file, line, name, ptr, sizeof(*ptr), *ptr); \
    }
// non-const void*指针类型打印函数
inline void i_ezs_print_pointer_void(const char *file, const int line,
                                     const char *name, void *const *ptr) {
    DEFINE_PRINT_FUNC_FOR_VOID_POINTER_IMPL(file, line, name, ptr);
}

// const void*指针类型打印函数
inline void i_ezs_print_const_pointer_void(const char *file, const int line,
                                           const char *name, const void *const *ptr) {
    DEFINE_PRINT_FUNC_FOR_VOID_POINTER_IMPL(file, line, name, ptr);
}

// 不支持的类型打印函数
inline void i_ezs_print_unsupported(const char *file, const int line,
                                    const char *name, const void *ptr) {
    printf("[EZS %s:%d] -> [%s at 0x%p]:unsupported type\n",
           file, line, name, ptr);
}

/*---------------------------清理局部宏---------------------------*/
#undef DEFINE_PRINT_FUNC_FOR_TYPE
#undef DEFINE_PRINT_FUNC_FOR_POINTER_IMPL
#undef DEFINE_PRINT_FUNC_FOR_POINTER
#undef DEFINE_PRINT_FUNC_FOR_CONST_POINTER
#undef DEFINE_PRINT_FUNC
#undef GENERATE_PRINT_FUNCTIONS
#undef DEFINE_PRINT_FUNC_FOR_VOID_POINTER_IMPL
