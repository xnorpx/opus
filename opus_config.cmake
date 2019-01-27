include(opus_functions.cmake)

configure_file(config.h.in config.h @ONLY)
add_definitions(-DHAVE_CONFIG_H)

option(FIXED_POINT "Use fixed-point code (for devices with less powerful FPU)"
       OFF)
option(USE_ALLOCA "Use alloca for stack arrays (on non-C99 compilers)" OFF)
option(BUILD_PROGRAMS "Build programs" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY C_STANDARD 99)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# It is strongly recommended to uncomment one of these VAR_ARRAYS: Use C99
# variable-length arrays for stack allocation USE_ALLOCA: Use alloca() for stack
# allocation If none is defined, then the fallback is a non-threadsafe global
# array
if(USE_ALLOCA OR MSVC)
  add_definitions(-DUSE_ALLOCA)
else()
  add_definitions(-DVAR_ARRAYS)
endif()

include(CheckLibraryExists)
check_library_exists(m floor "" HAVE_LIBM)
if(HAVE_LIBM)
  list(APPEND CMAKE_REQUIRED_LIBRARIES m)
endif()

opus_supports_cpu_detection(RUNTIME_CPU_CAPABILITY_DETECTION)

include(CMakeDependentOption)

opus_detect_sse(COMPILER_SUPPORT_SIMD)
if(COMPILER_SUPPORT_SIMD)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OPUS_PRESUME_SSE1 ON)
    set(OPUS_PRESUME_SSE2 ON)
  else()
    cmake_dependent_option(OPUS_PRESUME_SSE1
                           "Always use SSE1 (requires CPU with SSE1)"
                           ON
                           "COMPILER_SUPPORT_SIMD;OPUS_PRESUME_SSE1"
                           OFF)
    cmake_dependent_option(OPUS_PRESUME_SSE2
                           "Always use SSE2 (requires CPU with SSE2)"
                           ON
                           "COMPILER_SUPPORT_SIMD;OPUS_PRESUME_SSE2"
                           OFF)
  endif()
  cmake_dependent_option(OPUS_MAY_HAVE_SSE1
                         "Do runtime check and use SSE1 if available"
                         ON
                         "COMPILER_SUPPORT_SIMD;NOT OPUS_PRESUME_SSE1"
                         OFF)
  cmake_dependent_option(OPUS_MAY_HAVE_SSE2
                         "Do runtime check and use SSE2 if available"
                         ON
                         "COMPILER_SUPPORT_SIMD;NOT OPUS_PRESUME_SSE2"
                         OFF)
  cmake_dependent_option(OPUS_PRESUME_SSE4_1
                         "Always use SSE4.1 (requires CPU with SSE4.1)"
                         ON
                         "COMPILER_SUPPORT_SIMD;OPUS_PRESUME_SSE4_1"
                         OFF)
  cmake_dependent_option(OPUS_MAY_HAVE_SSE4_1
                         "Do runtime check and use SSE4.1 if available"
                         ON
                         "COMPILER_SUPPORT_SIMD;NOT OPUS_PRESUME_SSE4_1"
                         OFF)
  cmake_dependent_option(OPUS_PRESUME_AVX
                         "Always use AVX (requires CPU with AVX)"
                         ON
                         "COMPILER_SUPPORT_SIMD;OPUS_PRESUME_AVX"
                         OFF)
  cmake_dependent_option(OPUS_MAY_HAVE_AVX
                         "Do runtime check and use AVX if available"
                         ON
                         "COMPILER_SUPPORT_SIMD;NOT OPUS_PRESUME_AVX"
                         OFF)
endif()

opus_detect_neon(COMPILER_SUPPORT_NEON)
if(COMPILER_SUPPORT_NEON)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(OPUS_PRESUME_NEON ON)
  else()
    cmake_dependent_option(OPUS_PRESUME_NEON
                           "Always use NEON (requires CPU with NEON Support)"
                           ON
                           "COMPILER_SUPPORT_NEON;OPUS_PRESUME_NEON"
                           OFF)
  endif()
  cmake_dependent_option(OPUS_MAY_COMPILER_SUPPORT_NEON
                         "Use NEON if available"
                         ON
                         "COMPILER_SUPPORT_NEON;NOT OPUS_PRESUME_NEON"
                         OFF)
endif()
