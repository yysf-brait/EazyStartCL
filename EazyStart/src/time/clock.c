#include "EazyStart/time/clock.h"

#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#define _POSIX_C_SOURCE 200809L // NOLINT(*-reserved-identifier)
#include <unistd.h>
#undef _POSIX_C_SOURCE
#endif
#include <time.h>

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
    ts->tv_nsec = (long) ((counter.QuadPart % frequency.QuadPart) * 1000000000 / frequency.QuadPart);
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
