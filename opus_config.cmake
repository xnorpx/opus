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
  list(APPEND OPUS_REQUIRED_LIBRARIES m)
endif()

opus_supports_cpu_detection(RUNTIME_CPU_CAPABILITY_DETECTION)

opus_detect_sse(COMPILER_SUPPORT_SIMD)
if(COMPILER_SUPPORT_SIMD)
  option(OPUS_USE_SSE "Option to turn off SSE" ON)
  option(OPUS_MAY_SUPPORT_SSE1 "Does runtime check for SSE1 support" ON)
  option(OPUS_MAY_SUPPORT_SSE2 "Does runtime check for SSE2 support" ON)
  option(OPUS_MAY_SUPPORT_SSE4_1 "Does runtime check for SSE4_1 support" ON)
  option(OPUS_MAY_SUPPORT_AVX "Does runtime check AVX support" ON)
  option(OPUS_PRESUME_SSE1 "Assume target CPU has SSE1 support" OFF)
  option(OPUS_PRESUME_SSE2 "Assume target CPU has SSE2 support" OFF)
  option(OPUS_PRESUME_SSE4_1 "Assume target CPU has SSE4_1 support" OFF)
  option(OPUS_PRESUME_AVX "Assume target CPU has AVX support" OFF)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8) # Assume 64 bit has SSE2 support
    set(OPUS_PRESUME_SSE1 ON)
    set(OPUS_PRESUME_SSE2 ON)
  endif()
endif()

opus_detect_neon(COMPILER_SUPPORT_NEON)
if(COMPILER_SUPPORT_NEON)
  option(OPUS_USE_NEON "Option to turn off SSE" ON)
  option(OPUS_MAY_SUPPORT_NEON "Does runtime check for neon support" ON)
  option(OPUS_PRESUME_NEON "Assume target CPU has NEON support" OFF)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(OPUS_PRESUME_NEON ON)
  endif()
endif()
