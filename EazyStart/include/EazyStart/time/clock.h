#pragma once

#include <time.h>

bool ezs_clock_get_performance_counter(struct timespec *ts, char *src, size_t src_size);

bool ezs_clock_get_time(time_t *t, char *src, size_t src_size);
