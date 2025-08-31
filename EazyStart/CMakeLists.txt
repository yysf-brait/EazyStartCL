cmake_minimum_required(VERSION 3.28)
project(EazyStart C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(third_party/stc-wrapper)

set(EZS_SOURCES
        src/tools/charset.c
        src/io/print.c
        src/io/input.c
        src/tools/random.c
        src/time/clock.c
        src/time/benchmark.c
)
add_library(EazyStart ${EZS_SOURCES})
target_link_libraries(EazyStart
        PUBLIC
        stc
        PRIVATE
        m
)
if (WIN32)
    target_link_libraries(EazyStart PRIVATE bcrypt)
endif ()
target_include_directories(EazyStart PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if (NOT DEFINED EZS_ENABLE_SANITIZERS)
    # 如果用户没有通过命令行指定，则根据构建类型设置默认值
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(EZS_ENABLE_SANITIZERS ON CACHE BOOL "Enable Sanitizers (default for Debug)")
    else ()
        set(EZS_ENABLE_SANITIZERS OFF CACHE BOOL "Enable Sanitizers (default for non-Debug)")
    endif ()
endif ()
option(EZS_ENABLE_SANITIZERS "Enable Address and Undefined Behavior Sanitizers" ${EZS_ENABLE_SANITIZERS})


# 条件化Sanitizer选项
set(EZS_SANITIZER_FLAGS "")
if (EZS_ENABLE_SANITIZERS)
    # 仅在UNIX-like系统(如Linux, WSL)且使用GCC/Clang时启用Sanitizers
    if (UNIX AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Sanitizers (ASan, UBSan) enabled for this platform.")
        list(APPEND EZS_SANITIZER_FLAGS
                -fsanitize=address
                -fsanitize=undefined
        )
    else ()
        message(STATUS "Sanitizers (ASan, UBSan) are disabled on this platform (e.g., MinGW on Windows).")
    endif ()
else ()
    message(STATUS "Sanitizers (ASan, UBSan) are disabled.")
endif ()

# 为 EazyStart 库添加编译选项
target_compile_options(EazyStart PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wconversion
        -Wformat=2
        ${EZS_SANITIZER_FLAGS}
)

# 为 EazyStart 库添加链接选项
target_link_options(EazyStart PUBLIC
        ${EZS_SANITIZER_FLAGS}
)

# 此处配置“强制链接”的符号列表，避免其被编译器自动丢弃
set(EZS_FORCE_LINK_SYMBOLS
        i_ezs_init_charset
)
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    foreach (symbol ${EZS_FORCE_LINK_SYMBOLS})
        target_link_options(EazyStart PUBLIC "-Wl,-u,${symbol}")
    endforeach ()
elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
    foreach (symbol ${EZS_FORCE_LINK_SYMBOLS})
        target_link_options(EazyStart PUBLIC "/INCLUDE:${symbol}")
    endforeach ()
endif ()
