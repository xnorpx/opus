include(opus_functions.cmake)

if(NOT WIN32)
  message(FATAL_ERROR "Only tested on Windows")
endif()

get_opus_version(OPUS_VERSION)
add_compile_definitions("PACKAGE_VERSION \"${OPUS_VERSION}\"")

opus_detect_sse(HAVE_SSE)

include(CmakeDependentOption)
cmake_dependent_option(OPUS_PRESUME_SSE
                       "Use SSE always (requires CPU with SSE)"
                       ON
                       "HAVE_SSE;OPUS_PRESUME_SSE"
                       OFF)
cmake_dependent_option(OPUS_MAY_HAVE_SSE
                       "Use SSE if available"
                       ON
                       "HAVE_SSE;NOT OPUS_PRESUME_SSE"
                       OFF)
option(FIXED_POINT "Use fixed-point code (for device with less powerful FPU" NO)
option(USE_ALLOCA "Use alloca for stack arrays (on non-C99 compilers)" NO)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY C_STANDARD 99)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  string(REGEX
         REPLACE " /W[0-4]"
                 ""
                 CMAKE_C_FLAGS
                 "${CMAKE_C_FLAGS}")
  add_compile_options(/W4)
endif()

# It is strongly recommended to uncomment one of these VAR_ARRAYS: Use C99
# variable-length arrays for stack allocation USE_ALLOCA: Use alloca() for stack
# allocation If none is defined, then the fallback is a non-threadsafe global
# array
if(USE_ALLOCA OR MSVC)
  add_definitions(-DUSE_ALLOCA)
else(USE_ALLOCA OR MSVC)
  add_definitions(-DVAR_ARRAYS)
endif(USE_ALLOCA OR MSVC)

include(CheckLibraryExists)
check_library_exists(m floor "" HAVE_LIBM)
if(HAVE_LIBM)
  list(APPEND CMAKE_REQUIRED_LIBRARIES m)
endif()
