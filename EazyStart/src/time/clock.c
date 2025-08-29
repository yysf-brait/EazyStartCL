#include "EazyStart/time/clock.h"

#include <assert.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#define _POSIX_C_SOURCE 200809L // NOLINT(*-reserved-identifier)
#include <unistd.h>
#undef _POSIX_C_SOURCE
#endif
#include <time.h>

#define NANOS_PER_SEC 1000000000L

/*---------------------------EZS_CLOCK 获取时间函数---------------------------*/

bool ezs_clock_get_performance_counter(struct timespec *ts, char *src, const size_t src_size) {
#if defined(_WIN32) || defined(_WIN64)
    if (nullptr != src && src_size > 0) {
        snprintf(src, src_size, "\'QueryPerformanceCounter\'");
    }

    static LARGE_INTEGER frequency = {0};
    if (0 == frequency.QuadPart) {
        if (!QueryPerformanceFrequency(&frequency)) {
            return false;
        }
    }
    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter)) {
        return false;
    }

    ts->tv_sec = counter.QuadPart / frequency.QuadPart;
    ts->tv_nsec = (long) ((counter.QuadPart % frequency.QuadPart) * NANOS_PER_SEC / frequency.QuadPart);
    return true;

#elif defined(__unix__) || defined(__APPLE__) && defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0
    if (nullptr != src && src_size > 0) {
        snprintf(src, src_size, "\'clock_gettime(CLOCK_MONOTONIC)\'");
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

struct timespec ezs_clock_timespec_div(const struct timespec ts1, const long divisor) {
    assert(divisor > 0);
    struct timespec result = {
        .tv_sec = ts1.tv_sec / divisor,
        .tv_nsec = (ts1.tv_sec % divisor * NANOS_PER_SEC + ts1.tv_nsec) / divisor
    };
    if (result.tv_nsec >= NANOS_PER_SEC) {
        result.tv_sec += 1;
        result.tv_nsec -= NANOS_PER_SEC;
    }
    return result;
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

void ezs_clock_timespec_to_suited_units(const struct timespec ts,
                                        unsigned long *days, signed char *hours,
                                        signed char *minutes, signed char *seconds,
                                        signed short *milliseconds, signed short *microseconds,
                                        signed short *nanoseconds) {
    assert(ts.tv_sec >= 0 && "ts.tv_sec must be non-negative");
    assert(ts.tv_nsec >= 0 && ts.tv_nsec < NANOS_PER_SEC && "ts.tv_nsec must be in [0, 1_000_000_000)");
    unsigned long total_seconds = (unsigned long) ts.tv_sec;
    unsigned long total_nanoseconds = (unsigned long) ts.tv_nsec;

    if (nullptr == days) return;
    *days = total_seconds / (unsigned long) (24 * 60 * 60);
    total_seconds %= (unsigned long) (24 * 60 * 60);

    if (nullptr == hours) return;
    *hours = (signed char) (total_seconds / (unsigned long) (60 * 60));
    total_seconds %= (unsigned long) (60 * 60);

    if (nullptr == minutes) return;
    *minutes = (signed char) (total_seconds / (unsigned long) 60);
    total_seconds %= 60;

    if (nullptr == seconds) return;
    *seconds = (signed char) total_seconds;

    if (nullptr == milliseconds) return;
    *milliseconds = (signed short) (total_nanoseconds / (unsigned long) 1000000);
    total_nanoseconds %= (unsigned long) 1000000;

    if (nullptr == microseconds) return;
    *microseconds = (signed short) (total_nanoseconds / (unsigned long) 1000);
    total_nanoseconds %= (unsigned long) 1000;

    if (nullptr == nanoseconds) return;
    *nanoseconds = (signed short) total_nanoseconds;
}

/*---------------------------清理局部宏---------------------------*/

#undef NANOS_PER_SEC
