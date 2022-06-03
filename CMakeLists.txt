cmake_minimum_required(VERSION 3.18)

#
# Git
#
find_package(Git)
if(NOT GIT_FOUND)
  message(FATAL_ERROR "Need git installed on this computer!")
endif()

#
# Version from Git
#
execute_process(
  COMMAND "${GIT_EXECUTABLE}" describe --tags --always
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE VERSION_STRING
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX REPLACE "([0-9]+).*" "\\1" VERSION_MAJOR "${VERSION_STRING}")
string(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION_STRING}")
string(REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION_STRING}")
set(VERSION_STRING_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

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
project(
  pulsar
  LANGUAGES CXX
  VERSION ${VERSION_STRING_SHORT}
)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

#
# Configure file version
#
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pulsar_version.hpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pulsar/pulsar_version.hpp"
)

# CPM
#
if(NOT EXISTS CPM_DOWNLOAD_VERSION)
  set(CPM_DOWNLOAD_VERSION 0.35.1)
endif()
set(CPM_DOWNLOAD_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpm/cpm_${CPM_DOWNLOAD_VERSION}.cmake")
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  file(DOWNLOAD
       https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
       ${CPM_DOWNLOAD_LOCATION}
  )
endif()
include("cmake/cpm/cpm_${CPM_DOWNLOAD_VERSION}.cmake")

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

#
# Compiler flags
#
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(
    -Wall
    -Wextra
    -pedantic
    -Wno-unknown-pragmas
    -fvisibility=hidden
    -mmmx
    -msse
    -msse2
    -mssse3
    -msse3
    -msse4.1
    -msse4.2
    -mavx
    -mavx2
  )
endif()
# Release
if(CMAKE_BUILD_TYPE STREQUAL Release)
  add_compile_options(-O3)
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
  set(OUTPUT_SUFFIX _debug${OUTPUT_SUFFIX})
endif()

#
# Files
#
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

#
# Library
#
add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE PULSAR_CONSTRUCT=1)
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES VERSION ${PROJECT_VERSION}
             LINKER_LANGUAGE CXX
             RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUT_SYSTEM}/
             ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUT_SYSTEM}/
             LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUT_SYSTEM}/
             OUTPUT_NAME ${PROJECT_NAME}${OUTPUT_SUFFIX}
             PREFIX ""
)
target_link_libraries(${PROJECT_NAME} PRIVATE dbghelp)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> 
         $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PRIVATE src
)

#
# Testing
#
if(BUILD_TESTING)
  # Catch2
  CPMAddPackage(
    NAME catch2
    GITHUB_REPOSITORY catchorg/Catch2
    GIT_TAG v3.0.0-preview5
    OPTIONS "CATCH_BUILD_TESTING OFF" "gtest_force_shared_crt"
  )

  enable_testing()

  # Discover the tests
  file(
    GLOB_RECURSE TEST_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "test/*.cpp"
  )

  # Process the tests
  foreach(TEST_SOURCE IN ITEMS ${TEST_SOURCES})
    get_filename_component(BINARY_NAME ${TEST_SOURCE} NAME_WLE)
    add_executable(${BINARY_NAME} ${TEST_SOURCE})
    target_link_libraries(
      ${BINARY_NAME}
      PRIVATE ${PROJECT_NAME}
      PUBLIC Catch2::Catch2WithMain
    )
    set_target_properties(
      ${BINARY_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                ${PROJECT_BINARY_DIR}/test/${OUTPUT_SYSTEM}/
    )
    add_test(NAME ${BINARY_NAME} COMMAND ${BINARY_NAME})
  endforeach()
endif()

#
# CPM Package Project
#
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.9.0")
PackageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include"
  INCLUDE_DESTINATION "include/pulsar"
  VERSION_HEADER "include/pulsar/pulsar_version.hpp"
  COMPATIBILITY SameMajorVersion
  DEPENDENCIES "dbghelp"
)
