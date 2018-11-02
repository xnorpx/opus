cmake_minimum_required(VERSION 3.12)
project(opus LANGUAGES C)
include(opus_config.cmake)
include(opus_sources.cmake)
enable_testing()

# library
add_library(opus STATIC ${opus_sources} ${opus_sources_float})

target_include_directories(opus
                           PUBLIC $<BUILD_INTERFACE:
                                  ${CMAKE_CURRENT_SOURCE_DIR}/include
                                  ${CMAKE_CURRENT_SOURCE_DIR}/celt
                                  ${CMAKE_CURRENT_SOURCE_DIR}/silk
                                  ${CMAKE_CURRENT_SOURCE_DIR}/silk/float
                                  >
                                  $<INSTALL_INTERFACE:include>)
target_compile_definitions(opus PRIVATE USE_ALLOCA CUSTOM_MODES OPUS_BUILD)

add_sources_group(opus silk ${silk_sources})
add_sources_group(opus silk ${silk_sources_float})
add_sources_group(opus celt ${celt_sources})

target_link_libraries(opus PRIVATE ${system_libs})

# TODO: install lib, headers and tests

# demo
add_executable(opus_demo ${opus_demo_sources})
target_link_libraries(opus_demo PRIVATE opus)

# compare
add_executable(opus_compare ${opus_compare_sources})
target_link_libraries(opus_compare PRIVATE opus)

# tests
add_executable(test_opus_api ${test_opus_api_sources})
target_link_libraries(test_opus_api PRIVATE opus)
add_test(test_opus_api test_opus_api)

add_executable(test_opus_encode ${test_opus_encode_sources})
target_link_libraries(test_opus_encode PRIVATE opus)
add_test(test_opus_encode test_opus_encode)

add_executable(test_opus_decode ${test_opus_decode_sources})
target_link_libraries(test_opus_decode PRIVATE opus)
add_test(test_opus_decode test_opus_decode)

add_executable(test_opus_padding ${test_opus_padding_sources})
target_link_libraries(test_opus_padding PRIVATE opus)
add_test(test_opus_padding test_opus_padding)
