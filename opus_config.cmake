set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message(STATUS "Build target:")
message(STATUS "  CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message(STATUS "Builds on:")
message(STATUS "  CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "  CMAKE_HOST_SYSTEM_VERSION: ${CMAKE_HOST_SYSTEM_VERSION}")
message(STATUS "  CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)
  message(FATAL_ERROR "Only tested on Windows")
endif()

# Windows compile options
add_compile_options($<$<C_COMPILER_ID:MSVC>:/MP>)
add_compile_options($<$<C_COMPILER_ID:MSVC>:/W4>)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()