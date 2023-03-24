cmake_minimum_required(VERSION 3.18)

#
# Git
#
find_package(Git)

if(NOT GIT_FOUND)
  message(FATAL_ERROR "Need git installed on this computer!")
endif()

#
# Include Guard
#
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
    "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

#
# Project
#
set(PULSAR_VERSION_MAJOR 0)
set(PULSAR_VERSION_MINOR 1)
set(PULSAR_VERSION_PATCH 4)
execute_process(COMMAND
  git rev-parse --short HEAD
  OUTPUT_VARIABLE PULSAR_VERSION_SHA_1)
string(STRIP ${PULSAR_VERSION_SHA_1} PULSAR_VERSION_SHA_1)
execute_process(COMMAND
  git rev-parse --abbrev-ref HEAD
  OUTPUT_VARIABLE PULSAR_VERSION_BRANCH)
string(STRIP ${PULSAR_VERSION_BRANCH} PULSAR_VERSION_BRANCH)
set(PULSAR_VERSION_STRING_SHORT "${PULSAR_VERSION_MAJOR}.${PULSAR_VERSION_MINOR}.${PULSAR_VERSION_PATCH}")
set(PULSAR_VERSION_STRING "${PULSAR_VERSION_STRING_SHORT}-${PULSAR_VERSION_BRANCH}-${PULSAR_VERSION_SHA_1}")
project(
  "pulsar"
  LANGUAGES CXX
  VERSION ${PULSAR_VERSION_STRING_SHORT}
)
configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/pulsar_version.hpp.in"
  "${PROJECT_SOURCE_DIR}/include/pulsar/pulsar_version.hpp" @ONLY)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

#
# CPM
#
if(NOT EXISTS CPM_DOWNLOAD_VERSION)
  set(CPM_DOWNLOAD_VERSION 0.37)
endif()

set(CPM_DOWNLOAD_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpm/cpm.cmake")

if(NOT(EXISTS ${CPM_DOWNLOAD_LOCATION}))
  file(DOWNLOAD
    https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
    ${CPM_DOWNLOAD_LOCATION}
  )
endif()

include("cmake/cpm/cpm.cmake")

#
# Install Dirs
#
include(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../")
endif()

#
# Options
#
option(BUILD_TESTING "Build tests?" On)
option(BUILD_SHARED "Build as shared libary or static library?" Off)

#
# Compiler flags
#
if(MSVC)
  set(PULSAR_COMPILER_FLAGS /W4)
  add_compile_options(
    /W4
  )
else()
  set(PULSAR_COMPILER_FLAGS
    -Wall -Wextra -Wshadow -Wformat=2 -Wunused -pedantic -Wno-unknown-pragmas -fvisibility=hidden # Warnings
    -mmmx -msse -msse2 -mssse3 -msse3 -mfma -msse4 -msse4.1 -msse4.2 -mavx -mavx2 # Auto-Vectorisation
    -ffunction-sections -fdata-sections)
endif()

# Release
if(CMAKE_BUILD_TYPE STREQUAL Release)
  add_compile_options(-Ofast -funroll-loops -ftree-vectorize)
endif()

#
# Compiler Platform
#
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(OUTPUT_SUFFIX 64)
else()
  set(OUTPUT_SUFFIX 32)
endif()

#
# Host OS
#
if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
  set(OUTPUT_SYSTEM win${OUTPUT_SUFFIX})
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
  set(OUTPUT_SYSTEM lin${OUTPUT_SUFFIX})
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
  set(OUTPUT_SYSTEM mac${OUTPUT_SUFFIX})
else()
  message(FATAL_ERROR "Unknown system name!")
endif()

#
# Is debug configuration?
#
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(OUTPUT_SUFFIX _dbg${OUTPUT_SUFFIX})
endif()

#
# Library
#
set(PULSAR_LIBRARY_NAME ${PROJECT_NAME}-linked)
file(GLOB_RECURSE PULSAR_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/pulsar/*.hpp")
file(GLOB_RECURSE PULSAR_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

# Shared?
if(BUILD_SHARED)
  add_library(${PULSAR_LIBRARY_NAME} SHARED ${PULSAR_HEADERS} ${PULSAR_SOURCES})
  target_compile_definitions(${PULSAR_LIBRARY_NAME} PUBLIC PULSAR_SHARED=1)
else()
  add_library(${PULSAR_LIBRARY_NAME} STATIC ${PULSAR_HEADERS} ${PULSAR_SOURCES})
  target_compile_definitions(${PULSAR_LIBRARY_NAME} PUBLIC PULSAR_STATIC=1)
endif()

target_compile_definitions(${PULSAR_LIBRARY_NAME} PRIVATE PULSAR_BUILD=1)

# Options
target_compile_options(${PULSAR_LIBRARY_NAME} PRIVATE ${PULSAR_COMPILER_FLAGS})
set_target_properties(
  ${PULSAR_LIBRARY_NAME}
  PROPERTIES VERSION ${PROJECT_VERSION}
  LINKER_LANGUAGE CXX
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${OUTPUT_SYSTEM}/
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/archive/${OUTPUT_SYSTEM}/
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${OUTPUT_SYSTEM}/
  OUTPUT_NAME ${PROJECT_NAME}${OUTPUT_SUFFIX}
  PREFIX ""
)
target_include_directories(
  ${PULSAR_LIBRARY_NAME}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PRIVATE src
)
target_link_libraries(
  ${PULSAR_LIBRARY_NAME} PRIVATE imagehlp dbghelp
)

#
# CPM -> MiMalloc
#
set(ENV{MIMALLOC_LARGE_OS_PAGE} 1)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(ENV{MIMALLOC_SHOW_STATS} 1)
  set(ENV{MIMALLOC_VERBOSE} 1)
  set(ENV{MIMALLOC_SHOW_ERRORS} 1)
endif()

CPMAddPackage(
  NAME mimalloc
  GIT_TAG v2.0.9
  GITHUB_REPOSITORY microsoft/mimalloc
  OPTION MI_OVERRIDE=ON)
target_link_libraries(
  ${PULSAR_LIBRARY_NAME}
  PUBLIC mimalloc-static
)

#
# CPM -> FMT
#
CPMAddPackage(
  NAME fmt
  GIT_TAG 9.1.0
  GITHUB_REPOSITORY fmtlib/fmt
)
target_link_libraries(
  ${PULSAR_LIBRARY_NAME} PUBLIC fmt::fmt
)

#
# CPM -> MoodyCamel
#
CPMAddPackage(
  NAME MoodyCamel
  GIT_TAG master
  GITHUB_REPOSITORY cameron314/concurrentqueue
)

#
# Testing
#
set(PULSAR_TESTER_INTERNAL_LIBRARY_NAME
  ${CMAKE_PROJECT_NAME}-tester-internal)
set(PULSAR_TESTER_LIBRARY_NAME
  ${CMAKE_PROJECT_NAME}-tester)
set(PULSAR_TESTER_INTERNAL_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pulsar_tester/pulsar_tester.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pulsar_tester/pulsar_tester.cpp")
set(PULSAR_TESTER_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pulsar_tester/pulsar_tester_main.cpp")
add_library(
  ${PULSAR_TESTER_INTERNAL_LIBRARY_NAME}
  STATIC ${PULSAR_TESTER_INTERNAL_SOURCES})
target_link_libraries(
  ${PULSAR_TESTER_INTERNAL_LIBRARY_NAME}
  PUBLIC ${PULSAR_LIBRARY_NAME})
add_library(
  ${PULSAR_TESTER_LIBRARY_NAME}
  INTERFACE)
target_sources(
  ${PULSAR_TESTER_LIBRARY_NAME}
  INTERFACE ${PULSAR_TESTER_SOURCES})
target_link_libraries(
  ${PULSAR_TESTER_LIBRARY_NAME}
  INTERFACE ${PULSAR_TESTER_INTERNAL_LIBRARY_NAME})

if(BUILD_TESTING)
  enable_testing()

  # Discover the tests
  file(
    GLOB_RECURSE TEST_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "test/*.cpp")

  # Process the tests
  foreach(TEST_SOURCE IN ITEMS ${TEST_SOURCES})
    get_filename_component(BINARY_NAME ${TEST_SOURCE} NAME_WLE)
    add_executable(${BINARY_NAME} ${TEST_SOURCE})
    target_link_libraries(
      ${BINARY_NAME}
      PUBLIC ${PULSAR_TESTER_LIBRARY_NAME} concurrentqueue)
    target_include_directories(
      ${BINARY_NAME}
      PRIVATE src)
    target_compile_options(
      ${BINARY_NAME}
      PRIVATE ${PULSAR_COMPILER_FLAGS})
    set_target_properties(
      ${BINARY_NAME}
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/${OUTPUT_SYSTEM}/)

    add_test(${BINARY_NAME} ${BINARY_NAME})

    # cv2pdb
    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      add_custom_command(
        TARGET ${BINARY_NAME} POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/${OUTPUT_SYSTEM}/
        COMMAND cv2pdb ${BINARY_NAME}.exe
      )
    endif()
  endforeach()
endif()

# TODO: Export. Learn how to export
# install(TARGETS ${PULSAR_LIBRARY_NAME}
# EXPORT PulsarTarget
# DESTINATION lib)