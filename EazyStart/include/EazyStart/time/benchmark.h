#pragma once

/*
 * 在默认情况下，EazyStart的benchmark功能会在程序退出时自动打印报告，并释放资源
 * 当你不希望该功能时，可以在编译时定义宏EZS_BENCHMARK_NO_AUTO_EXIT
 * 此时你需要手动调用ezs_benchmark_final_report函数来打印报告并释放资源
*/

// 条目为name的benchmark开始计时
void ezs_benchmark_start(const char *name);

// 条目为name的benchmark结束计时
void ezs_benchmark_end(const char *name);

// 打印所有已注册的benchmark条目的统计数据
void ezs_benchmark_print_all(void);

// 打印指定名称列表的benchmark条目的统计数据
void ezs_benchmark_print(char *names[]);

// 清除所有已注册的benchmark条目
void ezs_benchmark_clear(void);

// 释放所有已注册的benchmark条目占用的内存
// 除非你确定不会再使用benchmark功能，否则不应调用此函数
void ezs_benchmark_drop(void);

// 打印所有已注册的benchmark条目的统计数据，并释放其占用的内存
// 该函数应当在完成所有benchmark工作后调用一次
// 在默认情况下，该函数会被注册为atexit处理程序
// 因此通常不需要手动调用
// 如果你不希望该函数被自动注册为atexit处理程序，可以在编译时定义宏EZS_BENCHMARK_NO_AUTO_EXIT
void ezs_benchmark_final_report(void);
