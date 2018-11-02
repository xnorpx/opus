#[[Cmake helper function to parse source files from make files
this is to avoid breaking existing make and auto make support
but still have the option to use CMake with only lists at one place]]

cmake_minimum_required(VERSION 3.12)

function(get_opus_sources SOURCE_GROUP MAKE_FILE SOURCES)
  message(STATUS "Parse source file names from ${SOURCE_GROUP} in ${MAKE_FILE}")

  # read file, each item in list is one group
  file(STRINGS ${MAKE_FILE} opus_sources)

  # add wildcard for regex match
  string(APPEND SOURCE_GROUP ".*$")

  # find group
  foreach(val IN LISTS opus_sources)
    if(val MATCHES ${SOURCE_GROUP})
      list(TRANSFORM val STRIP)
      list(LENGTH val list_length)
      if(${list_length} EQUAL 1)
        # for tests split by '=' and clean up the rest into list
        string(FIND ${val} "=" index)
        math(EXPR index "${index} + 1")
        string(SUBSTRING ${val}
                         ${index}
                         -1
                         sources)
        string(STRIP ${sources} sources)
        string(REPLACE " "
                       ";"
                       sources
                       ${sources})
      else()
        # discard the group as not a file name
        list(SUBLIST val 1 -1 sources)
      endif()
      break()
    endif()
  endforeach()

  list(LENGTH sources list_length)

  if(${list_length} GREATER 0)
    message(
      STATUS
        "Succesfully parsed ${list_length} files from ${SOURCE_GROUP} in ${MAKE_FILE}"
      )
  else()
    message(
      FATAL_ERROR
        "No files parsed succesfully from ${SOURCE_GROUP} in ${MAKE_FILE}")
  endif()

  set(${SOURCES} ${sources} PARENT_SCOPE)
endfunction()

get_opus_sources(SILK_SOURCES silk_sources.mk silk_sources)
get_opus_sources(SILK_SOURCES_FLOAT silk_sources.mk silk_sources_float)
get_opus_sources(SILK_SOURCES_FIXED silk_sources.mk silk_sources_fixed)
get_opus_sources(SILK_SOURCES_SSE4_1 silk_sources.mk silk_sources_sse4_1)
get_opus_sources(SILK_SOURCES_FIXED_SSE4_1 silk_sources.mk
                 silk_sources_fixed_sse4_1)
get_opus_sources(SILK_SOURCES_ARM_NEON_INTR silk_sources.mk
                 silk_sources_arm_neon_intr)
get_opus_sources(SILK_SOURCES_FIXED_ARM_NEON_INTR silk_sources.mk
                 silk_sources_fixed_arm_neon_intr)

get_opus_sources(OPUS_SOURCES opus_sources.mk opus_sources)
get_opus_sources(OPUS_SOURCES_FLOAT opus_sources.mk opus_sources_float)

get_opus_sources(CELT_SOURCES celt_sources.mk celt_sources)
get_opus_sources(CELT_SOURCES_SSE celt_sources.mk celt_sources_sse)
get_opus_sources(CELT_SOURCES_SSE2 celt_sources.mk celt_sources_sse2)
get_opus_sources(CELT_SOURCES_SSE4_1 celt_sources.mk celt_sources_sse4_1)
get_opus_sources(CELT_SOURCES_ARM celt_sources.mk celt_sources_arm)
get_opus_sources(CELT_SOURCES_ARM_ASM celt_sources.mk celt_sources_arm_asm)
get_opus_sources(CELT_AM_SOURCES_ARM_ASM celt_sources.mk
                 celt_am_sources_arm_asm)
get_opus_sources(CELT_SOURCES_ARM_NEON_INTR celt_sources.mk
                 celt_sources_arm_neon_intr)
get_opus_sources(CELT_SOURCES_ARM_NE10 celt_sources.mk celt_sources_arm_ne10)

get_opus_sources(opus_custom_demo_SOURCES Makefile.am opus_demo_sources)
get_opus_sources(opus_compare_SOURCES Makefile.am opus_compare_sources)
get_opus_sources(tests_test_opus_api_SOURCES Makefile.am test_opus_api_sources)
get_opus_sources(tests_test_opus_encode_SOURCES Makefile.am test_opus_encode_sources)
get_opus_sources(tests_test_opus_decode_SOURCES Makefile.am test_opus_decode_sources)
get_opus_sources(tests_test_opus_padding_SOURCES Makefile.am test_opus_padding_sources)
