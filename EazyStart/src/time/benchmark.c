#include "EazyStart/time/benchmark.h"
#include "EazyStart/time/clock.h"
#include <inttypes.h>
#include <stc/cstr.h>

typedef struct {
    bool idle;
    uint64_t count;
    struct timespec lastTime;
    struct timespec minDuration;
    struct timespec maxDuration;
    struct timespec sumDuration;
    double correctedSumSquaredDuration; // Corrected Sum of Squares [Welford 方差计算]
} BenchmarkEntry;

static double variance(const double correctedSumSquaredDuration, const uint64_t count) {
    return count > 1 ? correctedSumSquaredDuration / (double) count : 0.0;
}

#define i_keypro cstr
#define i_val BenchmarkEntry
#define i_tag bench
#include <stc/smap.h>

static smap_bench g_benchmarks = {};

void ezs_benchmark_start(const char *name) {
    auto const res = smap_bench_emplace(&g_benchmarks, name, (BenchmarkEntry){0});
    BenchmarkEntry *entry = &res.ref->second;

    // 如果是新创建的条目，进行初始化
    if (res.inserted) {
        entry->idle = true;
        entry->minDuration.tv_sec = INT64_MAX;
        entry->minDuration.tv_nsec = 1000000000L - 1;
    }

    // 状态检查
    else if (!entry->idle) {
        fprintf(stderr, "[EZS BENCHMARK][ERROR] "
                "Benchmark item '%s' was started twice without being ended. "
                "Ignoring this call.\n", name);
        return;
    }
    entry->idle = false;
    // 记录开始时间并更新状态
    if (!ezs_clock_get_performance_counter(&entry->lastTime, nullptr, 0)) {
        fprintf(stderr, "[EZS BENCHMARK][FATAL] "
                "Failed to get high-resolution time. Benchmark cannot proceed.\n");
        exit(EXIT_FAILURE);
    }
}

void ezs_benchmark_end(const char *name) {
    struct timespec endTime = {};
    if (!ezs_clock_get_performance_counter(&endTime, nullptr, 0)) {
        fprintf(stderr, "[EZS BENCHMARK][FATAL] "
                "Failed to get high-resolution time. Benchmark cannot proceed.\n");
        return;
    }

    // 查找条目
    auto const it = smap_bench_find(&g_benchmarks, name);
    if (it.ref == nullptr) {
        fprintf(stderr, "[EZS BENCHMARK][ERROR] "
                "Benchmark item '%s' was ended without being started. "
                "Ignoring this call.\n", name);
        return;
    }

    BenchmarkEntry *entry = &it.ref->second;

    const struct timespec duration = ezs_clock_timespec_sub(endTime, entry->lastTime);

    // 状态检查
    if (entry->idle) {
        fprintf(stderr, "[EZS BENCHMARK][ERROR] "
                "Benchmark item '%s' was ended twice without being started. "
                "Ignoring this call.\n", name);
        return;
    }

    // 更新统计数据
    entry->idle = true;
    entry->count += 1;

    if (entry->count == 1) {
        entry->minDuration = duration;
        entry->maxDuration = duration;
        entry->sumDuration = duration;
        entry->correctedSumSquaredDuration = 0.0;
        return;
    }

    if (ezs_clock_timespec_compare(duration, entry->minDuration) < 0) {
        entry->minDuration = duration;
    }
    else if (ezs_clock_timespec_compare(duration, entry->maxDuration) > 0) {
        entry->maxDuration = duration;
    }

    // 维护 corrected sum of squares [Welford 方差计算]
    // 维护 sumDuration [时间总和]
    const double currentDurationInSeconds = ezs_clock_timespec_to_seconds(duration);
    const struct timespec previousMean = ezs_clock_timespec_div(entry->sumDuration, entry->count - 1);
    const double previousMeanInSeconds = ezs_clock_timespec_to_seconds(previousMean);
    const struct timespec currentSum = ezs_clock_timespec_add(entry->sumDuration, duration);
    entry->sumDuration = currentSum;
    const struct timespec currentMean = ezs_clock_timespec_div(currentSum, entry->count);
    const double currentMeanInSeconds = ezs_clock_timespec_to_seconds(currentMean);
    entry->correctedSumSquaredDuration +=
            (currentDurationInSeconds - previousMeanInSeconds) *
            (currentDurationInSeconds - currentMeanInSeconds);
}
