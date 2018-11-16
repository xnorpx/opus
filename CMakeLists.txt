cmake_minimum_required(VERSION 3.1)
project(opus LANGUAGES C)
include(opus_config.cmake)
include(opus_sources.cmake)
include(opus_functions.cmake)

add_library(objopus OBJECT ${opus_sources} ${opus_sources_float})
add_library(opus SHARED $<TARGET_OBJECTS:objopus>)
add_library(opus_static STATIC $<TARGET_OBJECTS:objopus>)

if(NOT MSVC)
  set_target_properties(opus_static PROPERTIES OUTPUT_NAME opus)
endif(NOT MSVC)

target_include_directories(opus
                           PUBLIC $<BUILD_INTERFACE:
                                  ${CMAKE_CURRENT_SOURCE_DIR}/include >)
target_link_libraries(opus PRIVATE ${CMAKE_REQUIRED_LIBRARIES})

target_include_directories(opus_static
                           PUBLIC $<BUILD_INTERFACE:
                                  ${CMAKE_CURRENT_SOURCE_DIR}/include >)
target_link_libraries(opus_static PRIVATE ${CMAKE_REQUIRED_LIBRARIES})
set_property(TARGET objopus PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(objopus
                           PRIVATE $<BUILD_INTERFACE:
                                   ${CMAKE_CURRENT_SOURCE_DIR}/include
                                   ${CMAKE_CURRENT_SOURCE_DIR}/celt
                                   ${CMAKE_CURRENT_SOURCE_DIR}/silk
                                   >)
target_compile_definitions(objopus PRIVATE CUSTOM_MODES OPUS_BUILD DLL_EXPORT)

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

  add_sources_group(objopus celt ${celt_sources_sse})
  add_sources_group(objopus celt ${celt_sources_sse2})

  if(SSE4_FLAG_SUPPORTED)
    add_sources_group(objopus celt ${celt_sources_sse4_1})
    if(FIXED_POINT)
      add_sources_group(objopus silk ${silk_sources_fixed_sse4_1})
    else(FIXED_POINT)
      add_sources_group(objopus silk ${silk_sources_sse4_1})
    endif(FIXED_POINT)
  endif(SSE4_FLAG_SUPPORTED)

  if(OPUS_MAY_HAVE_SSE)
    # Do runtime check for SIMD
    target_compile_definitions(objopus
                               PRIVATE
                               OPUS_X86_MAY_HAVE_SSE
                               OPUS_X86_MAY_HAVE_SSE2
                               OPUS_HAVE_RTCD)
    if(SSE4_FLAG_SUPPORTED)
      target_compile_definitions(objopus PRIVATE OPUS_X86_MAY_HAVE_SSE4_1)
    endif(SSE4_FLAG_SUPPORTED)

  elseif(OPUS_PRESUME_SSE)
    target_compile_definitions(objopus
                               PRIVATE
                               OPUS_X86_PRESUME_HAVE_SSE
                               OPUS_X86_PRESUME_HAVE_SSE2)
    if(SSE4_FLAG_SUPPORTED)
      target_compile_definitions(objopus PRIVATE OPUS_X86_PRESUME_SSE4_1)
    endif(SSE4_FLAG_SUPPORTED)
  endif()
endif(HAVE_SSE)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "(armv7-a)")
  add_sources_group(objopus celt ${celt_sources_arm})
endif()

if(HAVE_NEON)

  add_sources_group(objopus celt ${celt_sources_arm_neon_intr})
  add_sources_group(objopus silk ${silk_sources_arm_neon_intr})

  # silk arm neon depends on main_Fix.h
  target_include_directories(
    objopus
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/silk/fixed>)

  if(FIXED_POINT)
    add_sources_group(objopus silk ${silk_sources_fixed_arm_neon_intr})
  endif(FIXED_POINT)

  if(OPUS_MAY_HAVE_NEON)
    # Do runtime check for NEON
    target_compile_definitions(objopus
                               PRIVATE
                               OPUS_ARM_MAY_HAVE_NEON
                               OPUS_ARM_MAY_HAVE_NEON_INTR
                               OPUS_HAVE_RTCD)
  elseif(OPUS_PRESUME_NEON)
    target_compile_definitions(objopus
                               PRIVATE
                               OPUS_ARM_PRESUME_NEON
                               OPUS_ARM_PRESUME_NEON_INTR)
  endif()
endif()


include(GNUInstallDirs)
install(TARGETS opus opus_static
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

if(BUILD_TESTING)
  enable_testing()

  # demo
  add_executable(opus_demo ${opus_demo_sources})
  target_link_libraries(opus_demo PRIVATE opus_static)

  # compare
  add_executable(opus_compare ${opus_compare_sources})
  target_link_libraries(opus_compare PRIVATE opus_static)

  # tests
  add_executable(test_opus_api ${test_opus_api_sources})
  target_include_directories(test_opus_api
                             PRIVATE $<BUILD_INTERFACE:
                                     ${CMAKE_CURRENT_SOURCE_DIR}/celt >)
  target_link_libraries(test_opus_api PRIVATE opus_static)
  if(FIXED_POINT)
    target_compile_definitions(test_opus_api PRIVATE DISABLE_FLOAT_API)
  endif(FIXED_POINT)
  add_test(test_opus_api test_opus_api)

  add_executable(test_opus_encode ${test_opus_encode_sources})
  target_include_directories(test_opus_encode
                             PRIVATE $<BUILD_INTERFACE:
                                     ${CMAKE_CURRENT_SOURCE_DIR}/celt >)
  target_link_libraries(test_opus_encode PRIVATE opus_static)
  add_test(test_opus_encode test_opus_encode)

  add_executable(test_opus_decode ${test_opus_decode_sources})
  target_link_libraries(test_opus_decode PRIVATE opus_static)
  if(FIXED_POINT)
    target_compile_definitions(test_opus_decode PRIVATE DISABLE_FLOAT_API)
  endif(FIXED_POINT)
  add_test(test_opus_decode test_opus_decode)

  add_executable(test_opus_padding ${test_opus_padding_sources})
  target_link_libraries(test_opus_padding PRIVATE opus_static)
  add_test(test_opus_padding test_opus_padding)
endif(BUILD_TESTING)
