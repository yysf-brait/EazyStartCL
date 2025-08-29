#pragma once

#include <time.h>

/*---------------------------EZS_CLOCK 获取时间函数---------------------------*/

// 获取高精度时间
// src出参，用于指示时间来源，置空表示不关心时间来源
// src_size出参，表示src的大小
// 返回值：成功返回true，失败返回false
bool ezs_clock_get_performance_counter(struct timespec *ts, char *src, size_t src_size);

// 获取系统时间
// src出参，用于指示时间来源，置空表示不关心时间来源
// src_size出参，表示src的大小
// 返回值：成功返回true，失败返回false
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
struct timespec ezs_clock_timespec_sub(struct timespec ts1, struct timespec ts2);

// struct timespec operator-=
// 计算ts1 - ts2的结果，结果存入ts1
// ts1不为nullptr由用户保证
// 参数的合法性由用户保证 即
// ts1和ts2的tv_sec均 >= 0
// ts1和ts2的tv_nsec均在[0, 1_000_000_000)范围内
void ezs_clock_timespec_sub_eq(struct timespec *ts1, struct timespec ts2) __attribute__((nonnull(1)));

// struct timespec operator/
// 计算ts1 / divisor的结果，返回结果
// 参数的合法性由用户保证 即
// ts1和ts2的tv_sec均 >= 0
// ts1和ts2的tv_nsec均在[0, 1_000_000_000)范围内
// divisor > 0
struct timespec ezs_clock_timespec_div(struct timespec ts1, long divisor);

// struct timespec operator<=>
// 比较ts1和ts2的大小，返回-1表示ts1 < ts2，0表示ts1 == ts2，1表示ts1 > ts2
signed char ezs_clock_timespec_compare(struct timespec ts1, struct timespec ts2);

// 将timespec转换为秒，返回double类型的秒数
double ezs_clock_timespec_to_seconds(struct timespec ts);

// 将timespec转换为天、小时、分钟、秒、毫秒、微秒、纳秒的组合
// 参数的合法性由用户保证 即
// ts1和ts2的tv_sec均 >= 0
// ts1和ts2的tv_nsec均在[0, 1_000_000_000)范围内
// days: 0 <= days <= LONG_MAX
// hours: 0 <= hours < 24
// minutes: 0 <= minutes < 60
// seconds: 0 <= seconds < 60
// milliseconds: 0 <= milliseconds < 1000
// microseconds: 0 <= microseconds < 1000
// nanoseconds: 0 <= nanoseconds < 1000
// 置空的指针表示不关心该单位，并且比它更小的单位也不关心，即不会写入这些单位的值
void ezs_clock_timespec_to_suited_units(struct timespec ts,
                                        unsigned long *days, signed char *hours,
                                        signed char *minutes, signed char *seconds,
                                        signed short *milliseconds, signed short *microseconds,
                                        signed short *nanoseconds);
