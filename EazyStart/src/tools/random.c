#include "EazyStart/tools/random.h"
#include "EazyStart/time/clock.h"
#include <assert.h>
#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <bcrypt.h>
#endif
#include <time.h>

/*---------------------------EZS_RANDOM的工具性质宏---------------------------*/

// 浮点数fma泛型函数
#define fma_g(x, y, z) _Generic((x) + (y) + (z), \
    float:       fmaf, \
    double:      fma,  \
    long double: fmal  \
)((x), (y), (z))

/*---------------------------EZS_RANDOM的内部状态---------------------------*/

// 随机数种子
static uint64_t seed = 0;
// xoshiro256**状态
static uint64_t random_state[4];
// 惰性初始化的标志
static bool is_initialized = false;

/*---------------------------EZS_RANDOM的内部函数---------------------------*/

// 尝试从操作系统的随机数生成器获取种子
static bool try_seed_from_os_rng(char *src, const size_t src_size) {
#if defined(_WIN32)
    snprintf(src, src_size, "\'BCryptGenRandom\'");
    auto const status = BCryptGenRandom(nullptr, (PUCHAR) & seed, sizeof(seed), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#ifdef NT_SUCCESS
    return NT_SUCCESS(status);
#else
    return status >= 0;
#endif
#elif defined(__unix__) || defined(__APPLE__)
    snprintf(src, src_size, "\'/dev/urandom\'");
    FILE *f = fopen("/dev/urandom", "rb");
    if (nullptr == f) {
        return false;
    }
    if (fread(&seed, sizeof(seed), 1, f) != 1) {
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
#else
    snprintf(src, src_size, "no OS random source");
    return false;
#endif
}

// 尝试从高精度时间获取种子
static bool try_seed_from_hres_time(char *src, const size_t src_size) {
    struct timespec ts;
    if (ezs_clock_get_performance_counter(&ts, src, src_size)) {
        seed = (uint64_t) ts.tv_sec ^ (uint64_t) ts.tv_nsec << 1;
        return true;
    }
    return false;
}

// 尝试从当前时间获取种子
static bool try_seed_from_time(char *src, const size_t src_size) {
    time_t t = {0};
    if (ezs_clock_get_time(&t, src, src_size)) {
        seed = (uint64_t) t;
        return true;
    }
    return false;
}

// 使用固定资源初始化随机数种子
// 该函数应当确保总是成功
static bool try_seed_from_fallback(char *src, const size_t src_size) {
    snprintf(src, src_size, "fallback");
    seed = 0xdeadbeefdeadbeef;
    return true;
}

// 初始化随机数种子
static void init_seed() {
    bool (*seed_attempts[])(char *, size_t) = {
        try_seed_from_os_rng,
        try_seed_from_hres_time,
        try_seed_from_time,
        try_seed_from_fallback
    };
    printf("---------------------------[EZS RANDOM] AUTOMATICALLY INITIALIZING THE SEED---------------------------\n");
    for (size_t i = 0; i < sizeof(seed_attempts) / sizeof(seed_attempts[0]); ++i) {
        char src[32] = {0};
        if (seed_attempts[i](src, sizeof(src))) {
            printf("[EZS RANDOM] random seed <- %s\n", src);
            break;
        }
        printf("[EZS RANDOM] Failed to initialize seed from %s, "
               "the initial random number state will be degraded.\n", src);
    }
    printf("---------------------------[EZS RANDOM] SEED IS SET TO [0x%016" PRIx64 "]---------------------------\n",
           seed);
}

// splitmix64算法的下一个随机数生成函数
// 用于生成xoshiro256**算法的初始状态
static uint64_t splitmix64_next(uint64_t *state) {
    *state += 0x9e3779b97f4a7c15;
    uint64_t z = *state;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

// 初始化xoshiro256**算法的内部状态
static void init_state() {
    uint64_t state_splitmix64 = seed;
    random_state[0] = splitmix64_next(&state_splitmix64);
    random_state[1] = splitmix64_next(&state_splitmix64);
    random_state[2] = splitmix64_next(&state_splitmix64);
    random_state[3] = splitmix64_next(&state_splitmix64);
}

// xoshiro256**算法的左旋转函数
static uint64_t rotl(const uint64_t x, const int k) {
    return (x << k) | (x >> (64 - k));
}

// xoshiro256**算法生成下一个随机数
static uint64_t random_next() {
    if (!is_initialized) {
        ezs_random_init();
    }

    const uint64_t result = rotl(random_state[1] * 5, 7) * 9;
    const uint64_t t = random_state[1] << 17;
    random_state[2] ^= random_state[0];
    random_state[3] ^= random_state[1];
    random_state[1] ^= random_state[2];
    random_state[0] ^= random_state[3];
    random_state[2] ^= t;
    random_state[3] = rotl(random_state[3], 45);
    return result;
}

// 生成指定位数的随机数
// bits的范围为1到64，由调用者保证
static uint64_t random_next_bits(const int bits) {
    assert(bits > 0 && bits <= 64);
    return random_next() >> (64 - bits);
}

// 使用两次随机数生成128位的随机数
// 仅当long double的有效位数大于64且编译器支持__uint128_t时可用
#if LDBL_MANT_DIG > 64 && defined(__SIZEOF_INT128__)
[[maybe_unused]] static __uint128_t random_next_128() {
    return (__uint128_t) random_next() << 64 | random_next();
}

// 生成指定位数的随机数
// bits的范围为65到128，由调用者保证
// 仅当long double的有效位数大于64且编译器支持__uint128_t时可用
[[maybe_unused]] static __uint128_t random_next_bits_128(const int bits) {
    assert(bits > 64 && bits <= 128);
    return random_next_128() >> (128 - bits);
}
#endif


// 生成范围在[0, 1)的float随机数
static float random_float_01() {
    static_assert(FLT_MANT_DIG <= 64, "FLT_MANT_DIG must be less than or equal to 64.");
    return ldexpf((float) random_next_bits(FLT_MANT_DIG), -FLT_MANT_DIG);
}

// 生成范围在[0, 1)的double随机数
static double random_double_01() {
    static_assert(DBL_MANT_DIG <= 64, "DBL_MANT_DIG must be less than or equal to 64.");
    return ldexp((double) random_next_bits(DBL_MANT_DIG), -DBL_MANT_DIG);
}

// 生成范围在[0, 1)的long double随机数
static long double random_long_double_01() {
#if LDBL_MANT_DIG <= 64
    return ldexpl((long double) random_next_bits(LDBL_MANT_DIG), -LDBL_MANT_DIG);
#elif LDBL_MANT_DIG > 64 && LDBL_MANT_DIG <= 128 && defined(__SIZEOF_INT128__)
    return ldexpl((long double) random_next_bits_128(LDBL_MANT_DIG), -LDBL_MANT_DIG);
#else
#error "Unsupported long double: LDBL_MANT_DIG > 128, or __uint128_t is not available with this compiler for LDBL_MANT_DIG > 64."
#endif
}

// 生成范围在[0, 1]的float随机数
static float random_float_01_inclusive() {
    static_assert(FLT_MANT_DIG <= 64, "FLT_MANT_DIG must be less than or equal to 64.");
    return (float) random_next() / (float) UINT64_MAX;
}

// 生成范围在[0, 1]的double随机数
static double random_double_01_inclusive() {
    static_assert(DBL_MANT_DIG <= 64, "DBL_MANT_DIG must be less than or equal to 64.");
    return (double) random_next() / (double) UINT64_MAX;
}

// 生成范围在[0, 1]的long double随机数
static long double random_long_double_01_inclusive() {
#if LDBL_MANT_DIG <= 64
    return (long double) random_next() / (long double) UINT64_MAX;
#elif LDBL_MANT_DIG > 64 && LDBL_MANT_DIG <= 128 && defined(__SIZEOF_INT128__)
    const __uint128_t MAX_U128 = -1;
    return ((long double) random_next_128()) / (long double) MAX_U128;
#else
#error "Unsupported long double: LDBL_MANT_DIG > 128, or __uint128_t is not available with this compiler for LDBL_MANT_DIG > 64."
#endif
}

// 生成范围在[0, range]的uint64_t随机数
static uint64_t random_integer_range(const uint64_t range) {
    if (UINT64_MAX == range) {
        return random_next();
    }
    if (0 == range) {
        return 0;
    }
    const uint64_t num_outcomes = range + 1;
    const uint64_t threshold = -num_outcomes % num_outcomes;
    const uint64_t limit = UINT64_MAX - threshold;
    uint64_t r;
    do {
        r = random_next();
    } while (r > limit);

    return r % num_outcomes;
}

/*---------------------------EZS_RANDOM的初始化函数定义部分---------------------------*/

// 主动初始化随机数生成器
void ezs_random_init(void) {
    init_seed();
    init_state();
    is_initialized = true;
}

// 使用指定种子主动初始化随机数生成器
void ezs_random_init_with_seed(const uint64_t new_seed) {
    seed = new_seed;
    printf("---------------------------[EZS RANDOM] SEED IS SET TO [0x%016" PRIx64 "]---------------------------\n",
           seed);
    init_state();
    is_initialized = true;
}

// 检查随机数生成器是否已初始化
[[nodiscard]] bool ezs_random_is_initialized(void) {
    return is_initialized;
}

// 获取当前的随机数种子
[[nodiscard]] uint64_t ezs_random_get_current_seed(void) {
    return seed;
}

/*---------------------------EZS_RANDOM的常规类型函数定义部分---------------------------*/

// 整型随机数生成函数模板
#define DEFINE_RANDOM_INTEGER_FUNC(TYPE, SUFFIX) \
[[nodiscard]] TYPE ezs_random_##SUFFIX##_inclusive(const TYPE min, const TYPE max) { \
    static_assert(sizeof(TYPE) <= sizeof(uint64_t), \
                  "[EZS] ezs_random_" #SUFFIX " only supports types that can fit in 64 bits."); \
    assert(min <= max && "[EZS][ERROR] min must be less than or equal to max."); \
    if (min == max) { \
        return min; \
    } \
    const uint64_t range = ((uint64_t) max) - ((uint64_t) min); \
    return min + (TYPE) random_integer_range(range); \
} \
[[nodiscard]] TYPE ezs_random_##SUFFIX(const TYPE min, const TYPE max) { \
    static_assert(sizeof(TYPE) <= sizeof(uint64_t), \
                  "[EZS] ezs_random_" #SUFFIX " only supports types that can fit in 64 bits."); \
    assert(min < max && "[EZS][ERROR] min must be less than max."); \
    const uint64_t range = ((uint64_t) max - 1) - ((uint64_t) min); \
    return min + (TYPE) random_integer_range(range); \
}
// 批量生成整型随机数生成函数
I_EZS_RANDOM_INTEGER_TYPES_LIST(DEFINE_RANDOM_INTEGER_FUNC)

// 浮点随机数生成函数模板
#define DEFINE_RANDOM_FLOAT_FUNC(TYPE, SUFFIX) \
[[nodiscard]] TYPE ezs_random_##SUFFIX(const TYPE min, const TYPE max) { \
    assert(min < max && "[EZS][ERROR] min must be less than max."); \
    return fma_g(random_##SUFFIX##_01(), (max - min), min); \
} \
[[nodiscard]] TYPE ezs_random_##SUFFIX##_inclusive(const TYPE min, const TYPE max) { \
    assert(min <= max && "[EZS][ERROR] min must be less than or equal to max."); \
    if (min == max) { \
        return min; \
    } \
    return fma_g(random_##SUFFIX##_01_inclusive(), (max - min), min); \
}
// 批量生成浮点随机数生成函数
I_EZS_RANDOM_FLOAT_TYPES_LIST(DEFINE_RANDOM_FLOAT_FUNC)

/*---------------------------EZS_RANDOM的特殊类型函数定义部分---------------------------*/

[[nodiscard]] bool ezs_random_bool(void) {
    return 0 != random_next_bits(1);
}

/*---------------------------清理局部宏---------------------------*/

#undef DEFINE_RANDOM_INTEGER_FUNC
#undef DEFINE_RANDOM_FLOAT_FUNC
#undef fma_g
