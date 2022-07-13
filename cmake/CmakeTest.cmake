# set this to true if you want to break on fail
set(BREAK_ON_FAIL FALSE)

if(${CMAKE_VERSION} VERSION_LESS "3.17.0")
  message(FATAL_ERROR "Opus CMake Tests needs 3.17.0")
  return()
endif()

set(PASSED_TESTS 0 CACHE INTERNAL "")
set(FAILED_TESTS 0 CACHE INTERNAL "")
set(SKIPPED_TESTS 0 CACHE INTERNAL "")

function(print_results)
  message(STATUS "Passed tests: ${PASSED_TESTS}, Skipped tests: ${SKIPPED_TESTS} Failed tests: ${FAILED_TESTS}")
endfunction()

function(check_failed_tests)
  if(FAILED_TESTS GREATER 0)
    message(FATAL_ERROR "Number of failed tests: ${FAILED_TESTS}")
  endif()
endfunction()

function(add_passed_test)
  math(EXPR NEW_PASSED_TESTS "${PASSED_TESTS} + 1" OUTPUT_FORMAT DECIMAL)
  set(PASSED_TESTS ${NEW_PASSED_TESTS} CACHE INTERNAL "")
endfunction()

function(add_skipped_test)
  math(EXPR NEW_SKIPPED_TESTS "${SKIPPED_TESTS} + 1" OUTPUT_FORMAT DECIMAL)
  set(SKIPPED_TESTS ${NEW_SKIPPED_TESTS} CACHE INTERNAL "")
endfunction()

function(add_failed_test)
  math(EXPR NEW_FAILED_TESTS "${FAILED_TESTS} + 1" OUTPUT_FORMAT DECIMAL)
  set(FAILED_TESTS ${NEW_FAILED_TESTS} CACHE INTERNAL "")
endfunction()

macro(TEST_INIT)
  message(CHECK_START "${CMAKE_CURRENT_FUNCTION}")
  set(TEST_RESULT FALSE)
endmacro()

macro(TEST_SKIP)
  message(STATUS "Skip - ${CMAKE_CURRENT_FUNCTION}")
  add_skipped_test()
  return()
endmacro()

macro(TEST_END)
  if(TEST_RESULT)
    add_passed_test()
    message(CHECK_PASS "Passed")
  else()
    if(BREAK_ON_FAIL)
      message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} - Failed")
    else()
      add_failed_test()
      message(CHECK_FAIL "Failed")
    endif()
  endif()
endmacro()

function(example_passed_test)
  TEST_INIT()
  set(TEST_RESULT TRUE)
  TEST_END()
endfunction()

function(example_failing_test)
  TEST_INIT()
  TEST_END()
endfunction()

function(example_skipped_test)
  TEST_INIT()
  TEST_SKIP()
endfunction()

function(test_c_feature_check_vla)
  TEST_INIT()
  c_feature_check(VLA)
  if(MSVC)
    if(NOT VLA_SUPPORTED)
      set(TEST_RESULT TRUE)
    endif()
  else()
    if(VLA_SUPPORTED)
      set(TEST_RESULT TRUE)
    endif()
  endif()
  TEST_END()
endfunction()

function(test_c_feature_check_cpu_info_by_asm)
  TEST_INIT()
  c_feature_check(CPU_INFO_BY_ASM)
  if(MSVC)
    if(NOT CPU_INFO_BY_ASM_SUPPORTED)
      set(TEST_RESULT TRUE)
    endif()
  else()
    if(CPU_INFO_BY_ASM_SUPPORTED)
      set(TEST_RESULT TRUE)
    endif()
  endif()
  TEST_END()
endfunction()

function(test_c_feature_check_cpu_info_by_c)
  TEST_INIT()
  c_feature_check(CPU_INFO_BY_C)
  if(MSVC)
    if(NOT CPU_INFO_BY_C_SUPPORTED)
      set(TEST_RESULT TRUE)
    endif()
  else()
    if(CPU_INFO_BY_C_SUPPORTED)
      set(TEST_RESULT TRUE)
    endif()
  endif()
  TEST_END()
endfunction()

function(test_get_library_version_from_file)
  TEST_INIT()
  set(test_file "${CMAKE_BINARY_DIR}/test_configure.ac")
  file(WRITE test_file "OPUS_LT_CURRENT=8\nOPUS_LT_REVISION=0\nOPUS_LT_AGE=8\n")
  get_library_version_from_file(test_file OPUS_LIBRARY_VERSION OPUS_LIBRARY_VERSION_MAJOR)
  file(REMOVE test_file)
  if(OPUS_LIBRARY_VERSION STREQUAL "0.8.0" AND OPUS_LIBRARY_VERSION_MAJOR STREQUAL "0")
    set(TEST_RESULT TRUE)
  endif()
  TEST_END()
endfunction()

function(test_check_flag)
  TEST_INIT()
  if(MSVC)
    check_flag(AVX /arch:AVX)
    if(AVX_SUPPORTED)
      set(TEST_RESULT TRUE)
    endif()
  else()
    check_flag(AVX -mavx)
    if(AVX_SUPPORTED)
      set(TEST_RESULT TRUE)
    endif()
  endif()
  TEST_END()
endfunction()

function(test_get_opus_sources)
  TEST_INIT()
  set(TEST_RESULT TRUE)
  set(test_file "${CMAKE_BINARY_DIR}/test_file_list.mk")
  file(WRITE test_file 
"OPUS_SOURCES = \\
src/opus.c \\
src/opus_decoder.c \\
src/opus_encoder.c

OPUS_SOURCES_FLOAT = \\
src/analysis.c \\
src/mlp.c \\
src/mlp_data.c

CELT_SOURCES_SSE = \\
celt/x86/pitch_sse.c
")
  get_opus_sources(OPUS_SOURCES test_file opus_sources)
  if(NOT "${opus_sources}" STREQUAL "src/opus.c;src/opus_decoder.c;src/opus_encoder.c")
    message(STATUS "${opus_sources} != src/opus.c;src/opus_decoder.c;src/opus_encoder.c")
    set(TEST_RESULT FALSE)
  endif()

  get_opus_sources(OPUS_SOURCES_FLOAT test_file opus_sources_float)
  if(NOT "${opus_sources_float}" STREQUAL "src/analysis.c;src/mlp.c;src/mlp_data.c")
    message(STATUS "${opus_sources_float} != src/analysis.c;src/mlp.c;src/mlp_data.c")
    set(TEST_RESULT FALSE)
  endif()

  get_opus_sources(CELT_SOURCES_SSE test_file celt_sources_sse)
  if(NOT "${celt_sources_sse}" STREQUAL "celt/x86/pitch_sse.c")
    message(STATUS "${celt_sources_sse} != celt/x86/pitch_sse.c")
    set(TEST_RESULT FALSE)
  endif()
  
  file(REMOVE test_file)
  TEST_END()
endfunction()

include(CFeatureCheck)
include(OpusFunctions)

test_c_feature_check_vla()
test_c_feature_check_cpu_info_by_asm()
test_c_feature_check_cpu_info_by_c()
test_get_library_version_from_file()
test_check_flag()
test_get_opus_sources()

# These need refactoring so will add test as we go
# TODO: get_package_version(PACKAGE_VERSION PROJECT_VERSION)
# TODO: opus_detect_neon COMPILER_SUPPORT_NEON)
# TODO: opus_detect_sse COMPILER_SUPPORT_SIMD)

print_results()
check_failed_tests()
