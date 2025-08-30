#include "EazyStart/time/clock.h"

#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#define _POSIX_C_SOURCE 200809L // NOLINT(*-reserved-identifier)
#include <unistd.h>
#undef _POSIX_C_SOURCE
#endif
#include <time.h>

static constexpr long NANOS_PER_SEC = 1'000'000'000L;

/*---------------------------EZS_CLOCK 获取时间函数---------------------------*/

bool ezs_clock_get_performance_counter(struct timespec *ts, char *src, const size_t src_size) {
#if defined(_WIN32) || defined(_WIN64)
    if (nullptr != src && src_size > 0) {
        snprintf(src, src_size, "\'QueryPerformanceCounter\'");
    }

    static LARGE_INTEGER frequency = {0};
    if (0 == frequency.QuadPart) {
        if (!QueryPerformanceFrequency(&frequency)) {
            fprintf(stderr, "[EZS][ERROR] "
                    "QueryPerformanceFrequency failed, the frequency of query performance counter is unknown. "
                    "High-resolution time cannot be obtained.\n");
            return false;
        }
        if (frequency.QuadPart > INT64_MAX / NANOS_PER_SEC + 1) {
            fprintf(stderr, "[EZS][WARN] "
                    "The frequency of query performance counter is too high (%" PRId64 " Hz). "
                    "There is a risk of overflow when converting to nanoseconds. "
                    "High-resolution time is not reliable.\n", frequency.QuadPart);
            return false;
        }
    }
    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter)) {
        return false;
    }
    // Explicit cast to time_t or typeof(tv_nsec) is necessary for portability,
    // as the underlying type of them can vary across platforms.
    // ReSharper disable once CppRedundantCastExpression
    ts->tv_sec = (time_t) counter.QuadPart / frequency.QuadPart;
    // ReSharper disable once CppRedundantCastExpression
    ts->tv_nsec = (typeof(ts->tv_nsec)) (counter.QuadPart % frequency.QuadPart * NANOS_PER_SEC / frequency.QuadPart);
    return true;

#elif defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0
    if (nullptr != src && src_size > 0) {
        snprintf(src, src_size, "\'clock_gettime(MONOTONIC)\'");
    }
    return 0 == clock_gettime(CLOCK_MONOTONIC, ts);
#else
    if (nullptr != src && src_size > 0) {
        snprintf(src, src_size, "no high-resolution time source");
    }
    return false;
#endif
}

bool ezs_clock_get_time(time_t *t, char *src, const size_t src_size) {
    if (nullptr != src && src_size > 0) {
        snprintf(src, src_size, "\'time\'");
    }
    return (time_t) -1 != time(t);
}

/*---------------------------EZS_CLOCK 时间计算函数---------------------------*/

struct timespec ezs_clock_timespec_add(const struct timespec ts1, const struct timespec ts2) {
    struct timespec result = {
        .tv_sec = ts1.tv_sec + ts2.tv_sec,
        .tv_nsec = ts1.tv_nsec + ts2.tv_nsec
    };
    if (result.tv_nsec >= NANOS_PER_SEC) {
        result.tv_sec += 1;
        result.tv_nsec -= NANOS_PER_SEC;
    }
    return result;
}

void ezs_clock_timespec_add_eq(struct timespec *ts1, const struct timespec ts2) {
    ts1->tv_sec += ts2.tv_sec;
    ts1->tv_nsec += ts2.tv_nsec;
    if (ts1->tv_nsec >= NANOS_PER_SEC) {
        ts1->tv_sec += 1;
        ts1->tv_nsec -= NANOS_PER_SEC;
    }
}

struct timespec ezs_clock_timespec_sub(const struct timespec ts1, const struct timespec ts2) {
    struct timespec result = {
        .tv_sec = ts1.tv_sec - ts2.tv_sec,
        .tv_nsec = ts1.tv_nsec - ts2.tv_nsec
    };
    if (result.tv_nsec < 0) {
        result.tv_sec -= 1;
        result.tv_nsec += NANOS_PER_SEC;
    }
    return result;
}

void ezs_clock_timespec_sub_eq(struct timespec *ts1, const struct timespec ts2) {
    ts1->tv_sec -= ts2.tv_sec;
    ts1->tv_nsec -= ts2.tv_nsec;
    if (ts1->tv_nsec < 0) {
        ts1->tv_sec -= 1;
        ts1->tv_nsec += NANOS_PER_SEC;
    }
}

struct timespec ezs_clock_timespec_div(const struct timespec ts, const uint64_t divisor) {
    assert(divisor != 0 && "Division by zero is not allowed.");
#if defined(__SIZEOF_INT128__) && (LONG_MAX == 9223372036854775807L)
    // time_t为64位且编译器支持128位整数: 使用128位整数，避免溢出风险。
    const __uint128_t total_nanos = (__uint128_t) ts.tv_sec * NANOS_PER_SEC + (__uint128_t) ts.tv_nsec;
    const __uint128_t result_nanos = total_nanos / divisor;
    return (struct timespec){
        // Explicit cast to time_t or typeof(tv_nsec) is necessary for portability,
        // as the underlying type of them can vary across platforms.
        // ReSharper disable once CppRedundantCastExpression
        .tv_sec = (time_t) (result_nanos / NANOS_PER_SEC),
        // ReSharper disable once CppRedundantCastExpression
        .tv_nsec = (typeof(ts.tv_nsec)) (result_nanos % NANOS_PER_SEC)
    };
#else
    // 1. time_t为32位: 此时 unsigned long long 足够大，不会溢出。
    // 2. time_t为64位 && 编译器不支持128位整数: 存在溢出风险。
#if LONG_MAX == 9223372036854775807L
#warning "EZS_CLOCK: On this platform, 'long' is 64-bit but 128-bit integers are not supported. The ezs_clock_timespec_div() function has a potential for integer overflow with very large divisions. "
#endif
    const uint64_t second = (uint64_t) ts.tv_sec;
    const uint64_t nano = (uint64_t) ts.tv_nsec;
    return (struct timespec){
        // Explicit cast to time_t or typeof(tv_nsec) is necessary for portability,
        // as the underlying type of them can vary across platforms.
        // ReSharper disable once CppRedundantCastExpression
        .tv_sec = (time_t) (second / divisor),
        // ReSharper disable once CppRedundantCastExpression
        .tv_nsec = (typeof(ts.tv_nsec)) ((second % divisor * NANOS_PER_SEC + nano) / divisor)
    };
#endif
}

double ezs_clock_timespec_to_seconds(const struct timespec ts) {
    return (double) ts.tv_sec + (double) ts.tv_nsec / NANOS_PER_SEC;
}

signed char ezs_clock_timespec_compare(const struct timespec ts1, const struct timespec ts2) {
    if (ts1.tv_sec < ts2.tv_sec) {
        return -1;
    }
    if (ts1.tv_sec > ts2.tv_sec) {
        return 1;
    }
    if (ts1.tv_nsec < ts2.tv_nsec) {
        return -1;
    }
    if (ts1.tv_nsec > ts2.tv_nsec) {
        return 1;
    }
    return 0;
}

void ezs_clock_timespec_decompose(const struct timespec ts,
                                  uint64_t *restrict days, uint64_t *restrict hours,
                                  uint64_t *restrict minutes, uint64_t *restrict seconds,
                                  uint64_t *restrict milliseconds, uint64_t *restrict microseconds,
                                  uint64_t *restrict nanoseconds) {
    constexpr uint64_t SECONDS_PER_MINUTE = 60;
    constexpr uint64_t SECONDS_PER_HOUR = 60 * SECONDS_PER_MINUTE;
    constexpr uint64_t SECONDS_PER_DAY = 24 * SECONDS_PER_HOUR;

    constexpr uint64_t NANOS_PER_MICROSECOND = 1'000;
    constexpr uint64_t NANOS_PER_MILLISECOND = 1'000 * NANOS_PER_MICROSECOND;
    constexpr uint64_t NANOS_PER_SECOND = 1'000 * NANOS_PER_MILLISECOND;

    uint64_t remaining_seconds = (uint64_t) ts.tv_sec;

    if (nullptr != days) {
        *days = remaining_seconds / SECONDS_PER_DAY;
        remaining_seconds %= SECONDS_PER_DAY;
    }
    if (nullptr != hours) {
        *hours = remaining_seconds / SECONDS_PER_HOUR;
        remaining_seconds %= SECONDS_PER_HOUR;
    }
    if (nullptr != minutes) {
        *minutes = remaining_seconds / SECONDS_PER_MINUTE;
        remaining_seconds %= SECONDS_PER_MINUTE;
    }
    if (nullptr != seconds) {
        *seconds = remaining_seconds;
        remaining_seconds = 0;
    }
    assert(
        remaining_seconds <= UINT64_MAX / NANOS_PER_SECOND &&
        "Potential overflow: duration is too large to be represented in nanoseconds.");
    uint64_t remaining_nanoseconds = (uint64_t) ts.tv_nsec +
                                     remaining_seconds * NANOS_PER_SECOND;

    if (nullptr != milliseconds) {
        *milliseconds = remaining_nanoseconds / NANOS_PER_MILLISECOND;
        remaining_nanoseconds %= NANOS_PER_MILLISECOND;
    }
    if (nullptr != microseconds) {
        *microseconds = remaining_nanoseconds / NANOS_PER_MICROSECOND;
        remaining_nanoseconds %= NANOS_PER_MICROSECOND;
    }
    if (nullptr != nanoseconds) {
        *nanoseconds = remaining_nanoseconds;
    }
}

bool ezs_clock_timespec_to_string(const struct timespec ts, char *buf, const size_t size) {
    if (nullptr == buf || size <= 1) {
        return false;
    }
    if (ezs_clock_timespec_compare(ts, (struct timespec){0, 0}) <= 0) {
        const int n = snprintf(buf, size, "0");
        return n >= 0 && (size_t) n < size;
    }
    uint64_t t[7] = {};
    ezs_clock_timespec_decompose(ts, &t[0], &t[1], &t[2], &t[3], &t[4], &t[5], &t[6]);
    for (size_t i = 0, written = 0; i < 7; i += 1) {
        if (0 == t[i]) {
            continue;
        }
        char const *const units[] = {"d", "h", "m", "s", "ms", "us", "ns"};
        const int n = snprintf(buf + written, size - written,
                               "%"PRIu64"%s", t[i], units[i]);
        if (n < 0 || (size_t) n >= size - written) {
            return false;
        }
        written += (size_t) n;
    }
    return true;
}
