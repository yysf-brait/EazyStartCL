#include "EazyStart/time/benchmark.h"
#include "EazyStart/time/clock.h"
#include <inttypes.h>
#include <math.h>
#include <stc/cstr.h>

typedef struct {
    bool idle;
    uint64_t count;
    struct timespec lastTime;
    struct timespec minDuration;
    struct timespec maxDuration;
    struct timespec sumDuration;
    long double correctedSumSquaredDuration; // Corrected Sum of Squares [Welford 方差计算]
} BenchmarkEntry;

// 平均值计算
static struct timespec mean_duration(const struct timespec sumDuration, const uint64_t count) {
    return ezs_clock_timespec_div(sumDuration, count);
}

// 样本方差计算
static long double sample_variance(const long double correctedSumSquaredDuration, const uint64_t count) {
    return count > 1 ? correctedSumSquaredDuration / (long double) (count - 1) : 0.0;
}

// 样本标准差计算
static long double sample_standard_deviation(const long double correctedSumSquaredDuration, const uint64_t count) {
    return sqrtl(sample_variance(correctedSumSquaredDuration, count));
}

// 相对标准差计算
static long double relative_standard_deviation(const long double sample_std_dev, const struct timespec meanDuration) {
    const long double meanInSeconds = ezs_clock_timespec_to_seconds(meanDuration);
    return meanInSeconds != 0.0 ? sample_std_dev / meanInSeconds * 100.0 : 0.0;
}

// BenchmarkEntry统计数值计算
// 计算平均值、样本标准差和相对标准差
// 返回false表示计算失败（count为0）
static bool calculate_benchmark_statistics(const BenchmarkEntry *const entry,
                                           struct timespec *meanDuration,
                                           long double *sample_std_dev,
                                           long double *rel_std_dev) {
    if (0 == entry->count) {
        return false;
    }
    *meanDuration = mean_duration(entry->sumDuration, entry->count);
    *sample_std_dev = sample_standard_deviation(entry->correctedSumSquaredDuration, entry->count);
    *rel_std_dev = relative_standard_deviation(*sample_std_dev, *meanDuration);
    return true;
}

#define i_keypro cstr
#define i_val BenchmarkEntry
#define i_tag bench
#include <stc/smap.h>

static smap_bench g_benchmarks = {};
#ifndef EZS_BENCHMARK_NO_AUTO_EXIT
static bool g_is_atexit_registered = false;
#endif

void ezs_benchmark_clear(void) {
    smap_bench_clear(&g_benchmarks);
}

void ezs_benchmark_drop(void) {
    smap_bench_drop(&g_benchmarks);
}

void ezs_benchmark_final_report(void) {
    ezs_benchmark_print_all();
    ezs_benchmark_drop();
}

void ezs_benchmark_start(const char *name) {
#ifndef EZS_BENCHMARK_NO_AUTO_EXIT
    if (!g_is_atexit_registered) {
        if (0 != atexit(ezs_benchmark_final_report)) {
            fprintf(stderr, "[EZS BENCHMARK][FATAL] "
                    "Failed to register atexit handler. Benchmark cannot proceed.\n");
            exit(EXIT_FAILURE);
        }
        g_is_atexit_registered = true;
    }
#endif
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
    } else if (ezs_clock_timespec_compare(duration, entry->maxDuration) > 0) {
        entry->maxDuration = duration;
    }

    // 维护 corrected sum of squares [Welford 方差计算]
    // 维护 sumDuration [时间总和]
    const long double currentDurationInSeconds = ezs_clock_timespec_to_seconds(duration);
    const struct timespec previousMean = ezs_clock_timespec_div(entry->sumDuration, entry->count - 1);
    const long double previousMeanInSeconds = ezs_clock_timespec_to_seconds(previousMean);
    const struct timespec currentSum = ezs_clock_timespec_add(entry->sumDuration, duration);
    entry->sumDuration = currentSum;
    const struct timespec currentMean = ezs_clock_timespec_div(currentSum, entry->count);
    const long double currentMeanInSeconds = ezs_clock_timespec_to_seconds(currentMean);
    entry->correctedSumSquaredDuration +=
            (currentDurationInSeconds - previousMeanInSeconds) *
            (currentDurationInSeconds - currentMeanInSeconds);
}

static void print_benchmark_entry(const char *name, const BenchmarkEntry *entry) {
    char count_buf[32] = "N/A", min_buf[32] = "N/A", max_buf[32] = "N/A",
            mean_buf[32] = "N/A", std_dev_buf[64] = "N/A", rel_std_dev_buf[64] = "N/A";
    struct timespec meanDuration = {};
    long double sample_std_dev = 0.0;
    long double rel_std_dev = 0.0;
    // 如果entry存在且统计数据计算成功，则格式化输出各项统计数据
    if (entry != nullptr &&
        calculate_benchmark_statistics(entry, &meanDuration, &sample_std_dev, &rel_std_dev)) {
        sprintf(count_buf, "%" PRIu64, entry->count);
        if (!ezs_clock_timespec_to_string(entry->minDuration, min_buf, sizeof(min_buf))) {
            snprintf(min_buf, sizeof(min_buf), "Error of conversion");
        }
        if (!ezs_clock_timespec_to_string(entry->maxDuration, max_buf, sizeof(max_buf))) {
            snprintf(max_buf, sizeof(max_buf), "Error of conversion");
        }
        if (!ezs_clock_timespec_to_string(meanDuration, mean_buf, sizeof(mean_buf))) {
            snprintf(mean_buf, sizeof(mean_buf), "Error of conversion");
        }
        if (snprintf(std_dev_buf, sizeof(std_dev_buf), "%Lfs", sample_std_dev) < 0) {
            snprintf(std_dev_buf, sizeof(std_dev_buf), "Error of conversion");
        }
        if (snprintf(rel_std_dev_buf, sizeof(rel_std_dev_buf), "%.2Lf%%", rel_std_dev) < 0) {
            snprintf(rel_std_dev_buf, sizeof(rel_std_dev_buf), "Error of conversion");
        }
    }
    // 输出结果
    printf("│%-20s │ %10s │ %20s │ %20s │ %20s │ %16s │ %8s│\n",
           name, count_buf, mean_buf, min_buf, max_buf, std_dev_buf, rel_std_dev_buf);
}

static void print_benchmark_header(void) {
    printf("\n");
    printf(
        "┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
    printf(
        "│                                                       Benchmark Result Table                                                       │\n");
    printf(
        "├─────────────────────┬────────────┬──────────────────────┬──────────────────────┬──────────────────────┬──────────────────┬─────────┤\n");
    printf("│%-20s │ %10s │ %20s │ %20s │ %20s │ %16s │ %8s│\n",
           "Benchmark Name", "Count", "Mean", "Min", "Max", "Std Dev", "RSD");
    printf(
        "├─────────────────────┼────────────┼──────────────────────┼──────────────────────┼──────────────────────┼──────────────────┼─────────┤\n");
}

static void print_benchmark_footer(void) {
    printf(
        "└─────────────────────┴────────────┴──────────────────────┴──────────────────────┴──────────────────────┴──────────────────┴─────────┘\n\n");
}

void ezs_benchmark_print(char *names[]) {
    print_benchmark_header();
    for (size_t i = 0; names[i] != nullptr; i += 1) {
        const char *name = names[i];
        auto const it = smap_bench_find(&g_benchmarks, name);
        print_benchmark_entry(name, it.ref ? &it.ref->second : nullptr);
    }
    print_benchmark_footer();
}

void ezs_benchmark_print_all(void) {
    print_benchmark_header();
    c_foreach(it, smap_bench, g_benchmarks) {
        print_benchmark_entry(cstr_str(&it.ref->first), &it.ref->second);
    }
    print_benchmark_footer();
}
