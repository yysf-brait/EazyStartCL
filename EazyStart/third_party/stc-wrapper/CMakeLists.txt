project(stc-wrapper C)

set(STC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../STC)

set(STC_SOURCES
        ${STC_SOURCE_DIR}/src/cregex.c
        ${STC_SOURCE_DIR}/src/cspan.c
        ${STC_SOURCE_DIR}/src/cstr_core.c
        ${STC_SOURCE_DIR}/src/cstr_io.c
        ${STC_SOURCE_DIR}/src/cstr_utf8.c
        ${STC_SOURCE_DIR}/src/csview.c
        ${STC_SOURCE_DIR}/src/fmt.c
        ${STC_SOURCE_DIR}/src/random.c
        ${STC_SOURCE_DIR}/src/stc_core.c
)

add_library(stc STATIC ${STC_SOURCES})

target_include_directories(stc PUBLIC
        $<BUILD_INTERFACE:${STC_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(stc PRIVATE m)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(stc PRIVATE
            -Wno-missing-field-initializers
            -Wno-double-promotion
            -Wno-stringop-overflow
            -Wno-clobbered
    )
endif ()
