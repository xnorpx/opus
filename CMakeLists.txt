cmake_minimum_required(VERSION 3.12)
project(opus LANGUAGES C)
include(opus_sources.cmake)
enable_testing()

message(STATUS "Build target:")
message(STATUS "  CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message(STATUS "Builds on:")
message(STATUS "  CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "  CMAKE_HOST_SYSTEM_VERSION: ${CMAKE_HOST_SYSTEM_VERSION}")
message(STATUS "  CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  message(FATAL_ERROR "Only tested on Windows and Linux")
endif()

list(APPEND opus_sources
            ${opus_sources_float}
            ${celt_sources}
            ${silk_sources})

# TODO: add Fixed and optimizations
list(APPEND opus_sources ${silk_sources_float})

# library
add_library(opus ${opus_sources})
set_property(TARGET opus PROPERTY C_STANDARD 99)
target_include_directories(opus
                           PUBLIC $<BUILD_INTERFACE:
                                  ${CMAKE_CURRENT_SOURCE_DIR}/include
                                  ${CMAKE_CURRENT_SOURCE_DIR}/celt
                                  ${CMAKE_CURRENT_SOURCE_DIR}/silk
                                  ${CMAKE_CURRENT_SOURCE_DIR}/silk/float
                                  >
                                  $<INSTALL_INTERFACE:include>)
target_compile_definitions(opus PRIVATE USE_ALLOCA CUSTOM_MODES OPUS_BUILD)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_link_libraries(opus m)
endif()

# TODO: install lib, headers and tests

# demo
add_executable(opus_demo ${opus_demo})
set_property(TARGET opus_demo PROPERTY C_STANDARD 99)
target_link_libraries(opus_demo opus)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_compile_definitions(opus_demo PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# tests
add_executable(test_opus_api ${test_opus_api})
set_property(TARGET test_opus_api PROPERTY C_STANDARD 99)
target_link_libraries(test_opus_api opus)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_compile_definitions(test_opus_api PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(test_opus_api test_opus_api)

add_executable(test_opus_encode ${test_opus_encode})
set_property(TARGET test_opus_encode PROPERTY C_STANDARD 99)
target_link_libraries(test_opus_encode opus)
target_compile_definitions(test_opus_encode PRIVATE _CRT_SECURE_NO_WARNINGS)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_compile_definitions(test_opus_encode PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(test_opus_encode test_opus_encode)

add_executable(test_opus_decode ${test_opus_decode})
set_property(TARGET test_opus_decode PROPERTY C_STANDARD 99)
target_link_libraries(test_opus_decode opus)
target_compile_definitions(test_opus_decode PRIVATE _CRT_SECURE_NO_WARNINGS)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_compile_definitions(test_opus_decode PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(test_opus_decode test_opus_decode)

add_executable(test_opus_padding ${test_opus_padding})
set_property(TARGET test_opus_padding PROPERTY C_STANDARD 99)
target_link_libraries(test_opus_padding opus)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_compile_definitions(test_opus_padding PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
add_test(test_opus_padding test_opus_padding)
