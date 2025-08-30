#pragma once

#include <stdint.h>
#include <time.h>

/*---------------------------EZS_CLOCK 获取时间函数---------------------------*/

// 获取高精度时间
// src出参，用于指示时间来源，置空表示不关心时间来源
// src_size出参，表示src的大小
// 返回值：获取时间成功返回true，失败返回false
//
// 对src出参的行为 等价于 不检查返回值的一次snprintf调用
// 字符串被截断时将不会得到提醒
// 建议至少给出32bytes的空间
bool ezs_clock_get_performance_counter(struct timespec *ts, char *src, size_t src_size);

// 获取系统时间
// src出参，用于指示时间来源，置空表示不关心时间来源
// src_size出参，表示src的大小
// 返回值：成功返回true，失败返回false
//
// 对src出参的行为 等价于 不检查返回值的一次snprintf调用
// 字符串被截断时将不会得到提醒
// 建议至少给出32bytes的空间
bool ezs_clock_get_time(time_t *t, char *src, size_t src_size);

/*---------------------------EZS_CLOCK 时间计算函数---------------------------*/

// struct timespec operator+
// 计算ts1 + ts2的结果，返回结果
// 参数的合法性由用户保证 即
// ts1和ts2的tv_sec均 >= 0
// ts1和ts2的tv_nsec均在[0, 1_000_000_000)范围内
struct timespec ezs_clock_timespec_add(struct timespec ts1, struct timespec ts2);

// struct timespec operator+=
// 计算ts1 + ts2的结果，结果存入ts1
// ts1不为nullptr由用户保证
// 参数的合法性由用户保证 即
// ts1和ts2的tv_sec均 >= 0
// ts1和ts2的tv_nsec均在[0, 1_000_000_000)范围内
void ezs_clock_timespec_add_eq(struct timespec *ts1, struct timespec ts2) __attribute__((nonnull(1)));

// struct timespec operator-
// 计算ts1 - ts2的结果，返回结果
// 参数的合法性由用户保证 即
// ts1和ts2的tv_sec均 >= 0
// ts1和ts2的tv_nsec均在[0, 1_000_000_000)范围内
// 可能会导致返回值.tv_sec为负数
struct timespec ezs_clock_timespec_sub(struct timespec ts1, struct timespec ts2);

// struct timespec operator-=
// 计算ts1 - ts2的结果，结果存入ts1
// ts1不为nullptr由用户保证
// 参数的合法性由用户保证 即
// ts1和ts2的tv_sec均 >= 0
// ts1和ts2的tv_nsec均在[0, 1_000_000_000)范围内
// 可能会导致ts1.tv_sec为负数
void ezs_clock_timespec_sub_eq(struct timespec *ts1, struct timespec ts2) __attribute__((nonnull(1)));

// struct timespec operator/
// 计算ts1 / divisor的结果，返回结果
// 参数的合法性由用户保证 即
// ts1和ts2的tv_sec均 >= 0
// ts1和ts2的tv_nsec均在[0, 1_000_000_000)范围内
// divisor > 0
struct timespec ezs_clock_timespec_div(struct timespec ts, uint64_t divisor);

// struct timespec operator<=>
// 比较ts1和ts2的大小，返回-1表示ts1 < ts2，0表示ts1 == ts2，1表示ts1 > ts2
signed char ezs_clock_timespec_compare(struct timespec ts1, struct timespec ts2);

// 将timespec转换为秒，返回double类型的秒数
double ezs_clock_timespec_to_seconds(struct timespec ts);

/*---------------------------EZS_CLOCK 时间格式化函数---------------------------*/

// 将struct timespec视为一个时间整体，**分解**为指定单位组合的表示
// 注意：出参的值会依赖于其他参数是否为nullptr
//
// 例如：
// 将{.tv_sec = 100, .tv_nsec = 123456789}分解为days, minutes, milliseconds, nanoseconds
// 得到days = 0, minutes = 1, milliseconds = 40 * 1000 + 123, nanoseconds = 456789
//
// 参数的合法性由用户保证 即
// ts.tv_sec >= 0
// ts.tv_nsec在[0, 1_000_000_000)范围内
//
// 置空的指针表示不关心该单位，该单位的值会下落到更小的单位
// 非空的指针表示该单位的值包括由更大单位下落的值将会被写入该指针指向的变量
//
// 在time_t为64位的系统上，如果ts的时间超过了584年，且秒及更大单位都置空，则会导致溢出
void ezs_clock_timespec_decompose(struct timespec ts,
                                  uint64_t *restrict days, uint64_t *restrict hours,
                                  uint64_t *restrict minutes, uint64_t *restrict seconds,
                                  uint64_t *restrict milliseconds, uint64_t *restrict microseconds,
                                  uint64_t *restrict nanoseconds);

// 将timespec转换为字符串，格式为 "?d?h?m?s?ms?us?ns"
// 注意：只打印非零的单位
// 参数的合法性由用户保证 即
// ts.tv_sec >= 0
// ts.tv_nsec在[0, 1_000_000_000)范围内
// 返回是否成功格式化
bool ezs_clock_timespec_to_string(struct timespec ts, char *buf, size_t size);
