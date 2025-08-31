#if defined(_WIN32) && (defined(__GNUC__) || defined(__clang__))
#include <stdio.h>
#include <Windows.h>

// 本函数在任何时候都不应被手动调用
// 它仅在程序启动时自动调用一次，用于设置控制台的字符集为UTF-8
// 需要加入 EazyStart/CMakeLists.txt -> EZS_FORCE_LINK_SYMBOLS
__attribute__ ((constructor))
void i_ezs_init_charset(void) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    puts("-------------------------[EZS CHARSET] AUTOMATICALLY SET THE CONSOLE TO UTF-8-------------------------");
}
#else

void i_ezs_init_charset(void) {
    /* On non-Windows platforms or with non-GCC compilers, we do nothing. */
    /* Terminals on these systems (e.g., Linux) typically default to UTF-8. */
}

#endif