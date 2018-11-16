cmake_minimum_required(VERSION 3.12)
project(opus LANGUAGES C)
include(opus_config.cmake)
include(opus_sources.cmake)

add_library(objopus OBJECT ${opus_sources} ${opus_sources_float})
add_library(${OPUS_SHARED_LIB} SHARED $<TARGET_OBJECTS:objopus>)
add_library(${OPUS_STATIC_LIB} STATIC $<TARGET_OBJECTS:objopus>)

target_include_directories(${OPUS_STATIC_LIB}
                           PUBLIC $<BUILD_INTERFACE:
                                  ${CMAKE_CURRENT_SOURCE_DIR}/include >
)

target_include_directories(${OPUS_SHARED_LIB}
                           PUBLIC $<BUILD_INTERFACE:
                                  ${CMAKE_CURRENT_SOURCE_DIR}/include >
)

target_include_directories(objopus
                           PRIVATE $<BUILD_INTERFACE:
                                   ${CMAKE_CURRENT_SOURCE_DIR}/include
                                   ${CMAKE_CURRENT_SOURCE_DIR}/celt
                                   ${CMAKE_CURRENT_SOURCE_DIR}/silk
                                   >)
target_compile_definitions(objopus PRIVATE CUSTOM_MODES OPUS_BUILD DLL_EXPORT)
target_link_libraries(objopus PRIVATE ${CMAKE_REQUIRED_LIBRARIES})

add_sources_group(objopus silk ${silk_sources})
add_sources_group(objopus celt ${celt_sources})

if(FIXED_POINT)
  add_sources_group(objopus silk ${silk_sources_fixed})
  target_include_directories(
    objopus
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/silk/fixed>)
  target_compile_definitions(objopus PRIVATE FIXED_POINT=1 DISABLE_FLOAT_API)
else(FIXED_POINT)
  add_sources_group(objopus silk ${silk_sources_float})
  target_include_directories(
    objopus
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/silk/float>)
endif(FIXED_POINT)

if(HAVE_SSE)
  if(FIXED_POINT)
    add_sources_group(objopus silk ${silk_sources_fixed_sse4_1})
  else(FIXED_POINT)
    add_sources_group(objopus silk ${silk_sources_sse4_1})
  endif(FIXED_POINT)

  add_sources_group(objopus celt ${celt_sources_sse})
  add_sources_group(objopus celt ${celt_sources_sse2})
  add_sources_group(objopus celt ${celt_sources_sse4_1})

  if(OPUS_MAY_HAVE_SSE)
    # Do runtime check for SIMD
    target_compile_definitions(objopus
                               PRIVATE
                               OPUS_X86_MAY_HAVE_SSE
                               OPUS_X86_MAY_HAVE_SSE2
                               OPUS_X86_MAY_HAVE_SSE4_1
                               OPUS_HAVE_RTCD)
  elseif(OPUS_PRESUME_SSE)
    target_compile_definitions(objopus
                               PRIVATE
                               OPUS_X86_PRESUME_HAVE_SSE
                               OPUS_X86_PRESUME_HAVE_SSE2
                               OPUS_X86_PRESUME_SSE4_1)
  endif()

  if(NOT MSVC)
    set(CMAKE_C_FLAGS "-msse4.1")
  endif(NOT MSVC)
endif(HAVE_SSE)

if(BUILD_TESTING)
  enable_testing()

  # demo
  add_executable(opus_demo ${opus_demo_sources})
  target_link_libraries(opus_demo PRIVATE ${OPUS_STATIC_LIB})

  # compare
  add_executable(opus_compare ${opus_compare_sources})
  target_link_libraries(opus_compare PRIVATE ${OPUS_STATIC_LIB})

  # tests
  add_executable(test_opus_api ${test_opus_api_sources})
  target_include_directories(test_opus_api
                             PRIVATE $<BUILD_INTERFACE:
                                     ${CMAKE_CURRENT_SOURCE_DIR}/celt >)
  target_link_libraries(test_opus_api PRIVATE ${OPUS_STATIC_LIB})
  if(FIXED_POINT)
    target_compile_definitions(test_opus_api PRIVATE DISABLE_FLOAT_API)
  endif(FIXED_POINT)
  add_test(test_opus_api test_opus_api)

  add_executable(test_opus_encode ${test_opus_encode_sources})
  target_include_directories(test_opus_encode
                             PRIVATE $<BUILD_INTERFACE:
                                     ${CMAKE_CURRENT_SOURCE_DIR}/celt >)
  target_link_libraries(test_opus_encode PRIVATE ${OPUS_STATIC_LIB})
  add_test(test_opus_encode test_opus_encode)

  add_executable(test_opus_decode ${test_opus_decode_sources})
  target_link_libraries(test_opus_decode PRIVATE ${OPUS_STATIC_LIB})
  if(FIXED_POINT)
    target_compile_definitions(test_opus_decode PRIVATE DISABLE_FLOAT_API)
  endif(FIXED_POINT)
  add_test(test_opus_decode test_opus_decode)

  add_executable(test_opus_padding ${test_opus_padding_sources})
  target_link_libraries(test_opus_padding PRIVATE ${OPUS_STATIC_LIB})
  add_test(test_opus_padding test_opus_padding)
endif(BUILD_TESTING)

# install TODO
