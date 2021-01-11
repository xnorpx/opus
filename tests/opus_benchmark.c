#include "SigProc_Flp.h"
#include "SigProc_FIX.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_opus_common.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
static LARGE_INTEGER gFrequency;
inline uint64_t GetHighResolutionTime(void)
{
    static LARGE_INTEGER performance_count;
    QueryPerformanceCounter(&performance_count);
    if (gFrequency.QuadPart == 0) {
        QueryPerformanceFrequency(&gFrequency);
    }
    return (uint64_t)((1000000 * (performance_count.QuadPart)) / gFrequency.QuadPart);
}
#else
#include <sys/time.h>
inline uint64_t GetHighResolutionTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return (uint64_t)((tv.tv_sec * 1000000) + (tv.tv_usec));
}
#endif

uint8_t gPrintHeadline = 0;

typedef struct {
    uint64_t min;
    uint64_t max;
    uint64_t total_time;
    uint64_t count_time;
} BenchmarkTimings;

uint64_t CalculateAverage(uint64_t total, uint64_t count)
{
    if (count > 0) {
        return total / count;
    }
    assert(0);
    return 0;
}

void PrintTimingsSummary(const char* timing_name, BenchmarkTimings timings)
{
    if (!gPrintHeadline) {
        for (int i = 0; i < 80; i++) {
            fprintf(stdout, "-");
        }
        fprintf(stdout, "\n");
        fprintf(stdout, "| Opus Benchmark                      | Avg [us] | Min [us] | Max [us] | Count |\n");
        for (int i = 0; i < 80; i++) {
            fprintf(stdout, "-");
        }
        fprintf(stdout, "\n");
        gPrintHeadline = 1;
    }
    fprintf(stdout, "| %-36.36s| %-9u| %-9u| %-9u| %-6u|\n", timing_name,
            (uint32_t)CalculateAverage(timings.total_time, timings.count_time), (uint32_t)timings.min,
            (uint32_t)timings.max, (uint32_t)timings.count_time);
}

const BenchmarkTimings kInitTimings = { UINT64_MAX, 0, 0, 0 };

#define TIME_FUNCTION(function, timings)                 \
    do {                                                 \
        uint64_t start = GetHighResolutionTime();        \
        (function);                                      \
        uint64_t time = GetHighResolutionTime() - start; \
        timings.total_time += time;                      \
        if (time > timings.max) {                        \
            timings.max = time;                          \
        }                                                \
        if (time < timings.min) {                        \
            timings.min = time;                          \
        }                                                \
        timings.count_time++;                            \
    } while (0)

#if defined(_MSC_VER)
void DoNotOptimize(void* value)
{
    (char const volatile*)value;
    _ReadWriteBarrier();
}
#else
void DoNotOptimize(void* value)
{
#if defined(__clang__)
    asm volatile("" : "+r,m"(value) : : "memory");
#else
    asm volatile("" : "+m,r"(value) : : "memory");
#endif
}
#endif

inline float randf(float max)
{
    return ((float)rand() / (float)(RAND_MAX)) * max;
}

void float2short_array(opus_int16* out, const silk_float* in, opus_int32 length)
{
    silk_float2short_array(out, in, length);
}

#include <immintrin.h>
void float2short_array_avx(opus_int16* out, const silk_float* in, opus_int32 length)
{
    __m256i tmp1;
    __m256i tmp2;
    int i = 0;

    for (; i < length - 16 + 1; i += 16) {
        tmp1 = _mm256_cvtps_epi32(_mm256_loadu_ps(&in[i]));
        tmp2 = _mm256_cvtps_epi32(_mm256_loadu_ps(&in[i + 8]));
        tmp1 = _mm256_packs_epi32(tmp1, tmp2);
        tmp1 = _mm256_permute4x64_epi64(tmp1, 0xD8);
        _mm256_storeu_si256((__m256i*)&out[i], tmp1);
    }

    for (; i < length - 8  + 1; i += 8) {
        tmp1 = _mm256_cvtps_epi32(_mm256_loadu_ps(&in[i]));
        tmp1 = _mm256_packs_epi32(tmp1, _mm256_setzero_si256());
        tmp1 = _mm256_permute4x64_epi64(tmp1, 0xD8);
        _mm256_storeu_si256((__m256i*)&out[i], tmp1);
    }

    for (; i < length - 4 + 1; i += 4) {
        __m128i tmp3 = _mm_cvtps_epi32(_mm_loadu_ps(&in[i]));
        tmp3 = _mm_packs_epi32(tmp3, _mm_setzero_si128());

    }

}

void benchmark_float2short_array()
{
    //#define FRAME_SIZE 1920
    #define FRAME_SIZE 1929
    float f[FRAME_SIZE] = { 0 };
    short s[FRAME_SIZE] = { 0 };
    short s_avx[FRAME_SIZE] = { 0 };

    DoNotOptimize(f);
    DoNotOptimize(s);
    DoNotOptimize(s_avx);

    BenchmarkTimings float2short_array_timings = kInitTimings;

    for (int i = 0; i < FRAME_SIZE; i++) {
        f[i] = randf((float)(INT16_MAX)-1.f);
    }

    // check saturation
    f[0] = INT16_MAX + 100;
    f[1] = INT16_MIN - 100;

    for (int i = 0; i < 1000; i++) {
        TIME_FUNCTION(float2short_array(&s, &f, FRAME_SIZE), float2short_array_timings);
        for (int j = 0; j < FRAME_SIZE; j++) {
            int16_t st = silk_SAT16((int32_t)f[j]);
            if (!(st == s[j] || st + 1 == s[j] || st - 1 == s[j])) {
                test_failed();
            }
        }
    }
    PrintTimingsSummary("float2short_array", float2short_array_timings);

    BenchmarkTimings float2short_avx_array_timings = kInitTimings;

    for (int i = 0; i < 1000; i++) {
        //TIME_FUNCTION(float2short_array_avx(&s_avx, &f, FRAME_SIZE), float2short_avx_array_timings);
        TIME_FUNCTION(float2short_array_avx(&s_avx, &f, 1), float2short_avx_array_timings);
        for (int j = 0; j < FRAME_SIZE; j++) {
            int16_t st = silk_SAT16((int32_t)f[j]);
            if (!(st == s_avx[j] || st + 1 == s_avx[j] || st - 1 == s_avx[j])) {
                test_failed();
            }
        }
    }
    PrintTimingsSummary("float2short_array_avx", float2short_avx_array_timings);

    for (int i = 0; i < FRAME_SIZE; i++) {
        if (s[i] != s_avx[i]) {
            test_failed();
        }
    }
}

int main(int argc, const char** argv)
{
    benchmark_float2short_array();
}
