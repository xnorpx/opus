#[[Cmake helper function to parse source files from make files
this is to avoid breaking existing make and auto make support
but still have the option to use CMake with only lists at one place]]

cmake_minimum_required(VERSION 3.12)

include(CheckSymbolExists)
if(MSVC)
  check_symbol_exists("_M_AMD64" "" x86_64)
  check_symbol_exists("_M_IX86" "" x86)
  check_symbol_exists("_M_ARM" "" ARM)
else()
  check_symbol_exists("__x86_64__" "" x86_64)
  check_symbol_exists("__i386__" "" x86)
  check_symbol_exists("__arm__" "" ARM)
endif()

function(opus_detect_sse HAVE_SSE)
  if(x86 OR x86_64)
    set(HAVE_SSE ON PARENT_SCOPE)
  else(x86 OR x86_64)
    set(HAVE_SSE OFF PARENT_SCOPE)
  endif(x86 OR x86_64)

endfunction()

function(add_sources_group target group)
  target_sources(${target} PRIVATE ${ARGN})
  source_group(${group} FILES ${ARGN})
endfunction()

function(get_opus_sources SOURCE_GROUP MAKE_FILE SOURCES)
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

  if(${list_length} LESS 1)
    message(
      FATAL_ERROR
        "No files parsed succesfully from ${SOURCE_GROUP} in ${MAKE_FILE}")
  endif()

  set(${SOURCES} ${sources} PARENT_SCOPE)
endfunction()
