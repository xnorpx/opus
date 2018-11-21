#[[Cmake helper function to parse source files from make files
this is to avoid breaking existing make and auto make support
but still have the option to use CMake with only lists at one place]]

cmake_minimum_required(VERSION 3.1)

function(check_and_set_flag NAME FLAG)
  include(CheckCCompilerFlag)
  check_c_compiler_flag(${FLAG} ${NAME}_FLAG_SUPPORTED)
  if(${NAME}_FLAG_SUPPORTED)
    add_definitions(${FLAG})
  endif()
endfunction()

function(get_package_version PACKAGE_VERSION)
  find_package(Git)
  if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --match "v*"
                    OUTPUT_VARIABLE OPUS_PACKAGE_VERSION)
    string(STRIP ${OPUS_PACKAGE_VERSION}, OPUS_PACKAGE_VERSION)
    string(REPLACE \n
                   ""
                   OPUS_PACKAGE_VERSION
                   ${OPUS_PACKAGE_VERSION})
    string(REPLACE ,
                   ""
                   OPUS_PACKAGE_VERSION
                   ${OPUS_PACKAGE_VERSION})
    set(PACKAGE_VERSION ${OPUS_PACKAGE_VERSION} PARENT_SCOPE)
  else(GIT_FOUND)
    set(PACKAGE_VERSION unknown PARENT_SCOPE)
  endif(GIT_FOUND)
endfunction()

include(CheckIncludeFile)
function(opus_detect_sse HAVE_SSE)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "(i[0-9]86|x86|X86|amd64|AMD64|x86_64)")
    check_include_file(xmmintrin.h HAVE_XMMINTRIN_H)
    if(HAVE_XMMINTRIN_H)
      set(HAVE_SSE ${HAVE_XMMINTRIN_H} PARENT_SCOPE)
      if(MSVC)
        check_and_set_flag(SSE4 /arch:AVX)
      else(MSVC)
        check_and_set_flag(SSE4 -msse4.1)
      endif(MSVC)
    endif()
  endif()
endfunction()

function(add_sources_group target group)
  target_sources(${target} PRIVATE ${ARGN})
  source_group(${group} FILES ${ARGN})
endfunction()

function(get_opus_sources SOURCE_GROUP MAKE_FILE SOURCES)
  # read file, each item in list is one group
  file(STRINGS ${MAKE_FILE} opus_sources)

  # add wildcard for regex match
  string(CONCAT SOURCE_GROUP ${SOURCE_GROUP} ".*$")

  # find group
  foreach(val IN LISTS opus_sources)
    if(val MATCHES ${SOURCE_GROUP})
      list(LENGTH val list_length)
      if(${list_length} EQUAL 1)
        # for tests split by '=' and clean up the rest into a list
        string(FIND ${val} "=" index)
        math(EXPR index "${index} + 1")
        string(SUBSTRING ${val}
                         ${index}
                         -1
                         sources)
        string(REPLACE " "
                       ";"
                       sources
                       ${sources})
      else()
        # discard the group
        list(REMOVE_AT val 0)
        set(sources ${val})
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

  # remove trailing whitespaces
  set(list_var "")
  foreach(source ${sources})
    string(STRIP "${source}" source)
    list(APPEND list_var "${source}")
  endforeach()

  set(${SOURCES} ${list_var} PARENT_SCOPE)
endfunction()
