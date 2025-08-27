#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stc/cstr.h>

// 1. 定义您的结构体
typedef struct {
    bool idle;
    uint64_t count;
    struct timespec lastTime;
    struct timespec minDuration;
    struct timespec maxDuration;
    struct timespec sumDuration;
    double sumSquaredDuration;
} BenchmarkEntry;

// 辅助函数，用于打印 BenchmarkEntry 的内容
void print_benchmark_entry(const BenchmarkEntry* entry) {
    printf("  idle: %s, count: %llu, lastTime: %ld.%09ld\n",
           entry->idle ? "true" : "false",
           (unsigned long long)entry->count,
           entry->lastTime.tv_sec, entry->lastTime.tv_nsec);
}

// 2. 使用 STC 宏定义有序 map
// i_keypro cstr:  键是 STC 的 cstr 类型
// i_val BenchmarkEntry: 值是我们的 BenchmarkEntry 结构体
// i_tag bench:      为所有生成的类型添加 'bench' 标签 (例如 smap_bench)
#define i_keypro cstr
#define i_val BenchmarkEntry
#define i_tag bench
#include <stc/sortedmap.h>

int main(void)
{
    // 3. 初始化有序 map
    smap_bench benchmarks = {};

    // 4. 创建一些条目数据并插入到 map 中
    // emplace() 函数可以直接从 const char* (键的原始类型)
    // 和 BenchmarkEntry (值的类型) 创建条目。
    printf("Inserting entries...\n");
    smap_bench_emplace(&benchmarks, "network_request", (BenchmarkEntry){
        .idle = false, .count = 25, .lastTime = {1678886420, 0}
    });
    smap_bench_emplace(&benchmarks, "database_query", (BenchmarkEntry){
        .idle = false, .count = 10, .lastTime = {1678886400, 0}
    });

    smap_bench_emplace(&benchmarks, "file_io_read", (BenchmarkEntry){
        .idle = true, .count = 5, .lastTime = {1678886410, 0}
    });



    // 5. 迭代并打印 map 的内容
    // 条目将按键的字母顺序排序 (database, file_io, network)
    printf("\nIterating over sorted map:\n");
    for (c_each(i, smap_bench, benchmarks)) {
        printf("Key: [%s]\n", cstr_str(&i.ref->first));
        print_benchmark_entry(&i.ref->second);
    }

    // 6. 查找并访问特定条目
    printf("\nAccessing 'database_query':\n");
    const BenchmarkEntry* entry = smap_bench_at(&benchmarks, "database_query");
    if (entry) {
        print_benchmark_entry(entry);
    }

    // 7. 清理并释放 map
    smap_bench_drop(&benchmarks);
    printf("\nMap dropped.\n");

    return 0;
}