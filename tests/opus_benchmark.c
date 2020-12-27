#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        fprintf(stdout, "Benchmark\t\t\t\tAverage Time [us]\tMin Time [us]\tMax Time [us]\tCount [us]\t");
        for (int i = 0; i < 80; i++) {
            fprintf(stdout, "-");
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "%s\t\t\t\t%u\t%u\t%u\t%u\n", timing_name, (uint32_t)CalculateAverage(timings.total_time, timings.count_time), (uint32_t)timings.min, (uint32_t)timings.max, (uint32_t)timings.count_time);
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

#if 0
// Generates random float numbers between min and max.
// Split [min, max] interval into 1/step values.
static inline float randf(float min, float max, float step)
{
    long range = (max - min) / step;
    long rint = rand() % (range + 1);

    return min + rint * step;
}

// Typedef and struct for handling individual tests.
typedef int (*test_fn)(const int, const float, const float, const float, double*, double*);
struct benchmark_test {
    const char* name;
    test_fn function;
};

void print_usage(const char* program)
{
    int i;
    //int test_nr = sizeof(benchmark_tests) / sizeof(pitch_tests[0]);
    int test_nr = 0;

    printf(
        "Usage:\n\t%s <test_function> -i <iterations(optional)> "
        "-s <data_size(optional)> -e <error(optional)>\n",
        program);
    printf("Available tests:\n");

    for (i = 0; i < test_nr; i++) {
        printf("\t%s\n", pitch_tests[i].name);
    }

    printf("\n");
}
#endif

void print_usage(char* _argv[])
{
    fprintf(stdout, "lol\n");
}

void benchmark_something()
{
    BenchmarkTimings something = kInitTimings;

    for (int i = 0; i < 100; i++) {
        TIME_FUNCTION(printf(""), something);
    }
    PrintTimingsSummary("Something", something);
}

int main(int argc, const char** argv)
{
    fprintf(stdout, "Opus Benchmark\n");
    benchmark_something();

#if 0
    int data_size = 1024;
    int iterations = 1;
    double time_c = 0.0, time_simd = 0.0;
    int i;
    int pass = 0;
    float step = 0.0001;

    const float min_range = -1, max_range = 1;
    const char* test_name = NULL;
    test_fn test = NULL;

    srand(0);

    if (argc < 2) {
        print_usage(argv[0]);
        // Running all tests with default values for "make check".
        int test_nr = sizeof(pitch_tests) / sizeof(pitch_tests[0]);
        for (i = 0; i < test_nr; i++) {
            pass += pitch_tests[i].function(data_size, min_range, max_range, step, &time_c, &time_simd);
        }
        return test_nr - pass;
    } else {
        test_name = argv[1];
        test = (test_fn)load_symbol(test_name);
        if (test == NULL) {
            fprintf(stdout, "Unable to find symbol: %s, exiting\n\n", test_name);
            print_usage(argv[0]);
            return 0;
        }
    }

    argv++;
    argc--;

    while (argc > 1) {
        if (strcmp(argv[1], "-i") == 0) {
            iterations = atoi(argv[2]);
        } else if (strcmp(argv[1], "-s") == 0) {
            data_size = atoi(argv[2]);
        } else if (strcmp(argv[1], "-e") == 0) {
            step = atof(argv[2]);
        }
        (--argc, ++argv);
    }

    for (i = 0; i < iterations; i++) {
        pass += test(data_size, min_range, max_range, step, &time_c, &time_simd);
    }

    // Print summary.
    printf("Results for %s, with %d iterations and %d data size:\n", test_name, iterations, data_size);
    printf("%.6lf seconds (avg: %.7lf)\tC time\n", time_c, time_c / iterations);
    printf("%.6lf seconds (avg: %.7lf)\t%s time\n", time_simd, time_simd / iterations, SIMD_TYPE);
    printf("%.6lf x\tSpeedup\n", time_c / time_simd);
    printf("%d/%d passed\n\n", pass, iterations);

    return iterations - pass;
#endif
    return 0;
}
