#include "EazyStart/io/input.h"
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUFFER_SIZE ((size_t)(1024))

// 清除输入缓冲区，直到遇到换行符或EOF
static void clear_input_buffer() {
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 读取一行输入到缓冲区，直到换行符或EOF，第一个换行符会被替换为字符串结束符'\0'。
static bool read_line(char *buffer, const char *prompt) {
    fputs(prompt, stdout);
    fflush(stdout);
    if (nullptr == fgets(buffer, INPUT_BUFFER_SIZE, stdin)) {
        if (ferror(stdin)) {
            fprintf(stderr, "[EZS][FATAL] "
                    "A critical I/O error occurred while reading from standard input. "
                    "The program cannot continue.\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "[EZS][CRITICAL] "
                "Input stream closed (EOF). "
                "The program cannot continue to read input.\n");
        exit(EXIT_SUCCESS);
    }
    char *input_end = strchr(buffer, '\n');
    if (nullptr == input_end) {
        fprintf(stderr, "[EZS][WARN] "
                "Input is too long. "
                "The limit is %zu characters with an additional newline character.\n",
                INPUT_BUFFER_SIZE - 2);
        clear_input_buffer();
        return false;
    }
    *input_end = '\0';
    return true;
}

// 返回字符串中第一个非空白字符的指针
// 如果字符串仅包含空白字符或为空字符串，则返回nullptr
static const char *get_first_non_whitespace_char(const char *str) {
    while ('\0' != *str && isspace((unsigned char) *str)) {
        str += 1;
    }
    return '\0' == *str ? nullptr : str;
}

/*---------------------------EZS_INPUT的常规类型函数定义部分---------------------------*/
// 常规类型的输入函数模板
#define DEFINE_INPUT_FUNC(TYPE, SUFFIX, CONV_FUNC, RANGE_CHECK, ...) \
    [[nodiscard]] TYPE ezs_input_##SUFFIX##_with_prompt(const char *prompt) { \
        while (true) { \
            char buffer[INPUT_BUFFER_SIZE]; \
            if (!read_line(buffer, prompt)) { \
                continue; \
            } \
            errno = 0; \
            char *interpretation_end = buffer; \
            auto const result = CONV_FUNC(buffer, &interpretation_end __VA_OPT__(, __VA_ARGS__)); \
            if (interpretation_end == buffer) { \
                fprintf(stderr, "[EZS][WARN] " \
                        "Input is empty or does not have the expected form.\n"); \
                continue; \
            } \
            if (errno == ERANGE || RANGE_CHECK) { \
                fprintf(stderr, "[EZS][WARN] " \
                        "Input out of range for "#TYPE".\n"); \
                continue; \
            } \
            const char* extra_chars_start = get_first_non_whitespace_char(interpretation_end); \
            if (nullptr != extra_chars_start) { \
                fprintf(stderr, "[EZS][WARN] " \
                        "Extra characters [%s] found after the last character interpreted.\n", extra_chars_start); \
                continue; \
            } \
            return (TYPE) result; \
        } \
    } \
    [[nodiscard]] TYPE ezs_input_##SUFFIX(void) { \
        return ezs_input_##SUFFIX##_with_prompt("[EZS] Please enter a ["#TYPE"] value: "); \
    }
// 批量生成输入函数
I_EZS_INPUT_TYPES_LIST(DEFINE_INPUT_FUNC)

/*---------------------------EZS_INPUT的特殊类型函数定义部分---------------------------*/
[[nodiscard]] char ezs_input_char_with_prompt(const char *prompt) {
    while (true) {
        char buffer[INPUT_BUFFER_SIZE];
        if (!read_line(buffer, prompt)) {
            continue;
        }

        // 如果仅输入了'\n'，read_line会将其转换为'\0'
        // 如果buffer[0]是'\0'，则需要将其重新视为'\n'
        if (buffer[0] == '\0') {
            return '\n';
        }
        if (buffer[1] != '\0') {
            fprintf(stderr, "[EZS][WARN] "
                    "Extra characters [%s] found after the last character interpreted.\n", buffer + 1);
            continue;
        }

        return buffer[0];
    }
}

[[nodiscard]] char ezs_input_char(void) {
    return ezs_input_char_with_prompt("[EZS] Please enter a [char] value(press Enter for '\\n'): ");
}

[[nodiscard]] bool ezs_input_bool_with_prompt(const char *prompt) {
    while (true) {
        char buffer[INPUT_BUFFER_SIZE];
        if (!read_line(buffer, prompt)) {
            continue;
        }

        const char *start = get_first_non_whitespace_char(buffer);
        if (nullptr == start) {
            fprintf(stderr, "[EZS][WARN] "
                    "Input is empty.\n");
            continue;
        }

        const char *extra_chars_start = get_first_non_whitespace_char(start + 1);
        if (nullptr != extra_chars_start) {
            fprintf(stderr, "[EZS][WARN] "
                    "Extra characters [%s] found after the last character interpreted.\n", extra_chars_start);
            continue;
        }

        switch (toupper((unsigned char) *start)) {
            case 'Y':
            case 'T':
            case '1':
                return true;
            case 'N':
            case 'F':
            case '0':
                return false;
            default:
                fprintf(
                    stderr, "[EZS][WARN] "
                    "Unrecognized input. "
                    "Please use 'Y'/'T'/'1' for true, or 'N'/'F'/'0' for false.\n");
        }
    }
}

[[nodiscard]] bool ezs_input_bool(void) {
    return ezs_input_bool_with_prompt("[EZS] Please enter a [bool] value (Y/N, T/F, 1/0): ");
}
