if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)
  message(FATAL_ERROR "Only tested on Windows")
endif()

option(FIXED_POINT "Use fixed-point code (for device with less powerful FPU" NO)
option(USE_ALLOCA "Use alloca for stack arrays (on non-C99 compilers)" NO)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY C_STANDARD 99)

# Windows compile options
add_compile_options($<$<C_COMPILER_ID:MSVC>:/MP>)
add_compile_options($<$<C_COMPILER_ID:MSVC>:/W4>)

if(MSVC)
  add_definitions(
    -D_CRT_SECURE_NO_WARNINGS
  )
endif()

# It is strongly recommended to uncomment one of these
# VAR_ARRAYS: Use C99 variable-length arrays for stack allocation
# USE_ALLOCA: Use alloca() for stack allocation
# If none is defined, then the fallback is a non-threadsafe global array
if (USE_ALLOCA OR MSVC)
    add_definitions(-DUSE_ALLOCA)
else (USE_ALLOCA OR MSVC)
    add_definitions(-DVAR_ARRAYS)
endif (USE_ALLOCA OR MSVC)

set(system_libs "")

include(CheckLibraryExists)
check_library_exists(m floor "" HAVE_LIBM)
if(HAVE_LIBM)
  list(APPEND system_libs m)
  list(APPEND CMAKE_REQUIRED_LIBRARIES m)
endif()
