cmake_minimum_required(VERSION 3.18)

#
# Pulsar -> Guard
#
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "[Pulsar] In-source builds not allowed.")
endif()

#
# Pulsar -> Git
#
find_package(Git)

if(NOT GIT_FOUND)
  message(FATAL_ERROR "[Pulsar] Need git installed on this computer!")
endif()

#
# Pulsar -> CXX Compiler Options
#
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
  set(PF_COMPILE_OPTIONS ${PF_COMPILE_OPTIONS})
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(PF_COMPILE_OPTIONS
    ${PF_COMPILE_OPTIONS}
    -Wall
    -Wextra
    -pedantic
    -Wno-unknown-pragmas
    -fvisibility=hidden)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  set(PF_COMPILE_OPTIONS ${PF_COMPILE_OPTIONS})
endif()

if(MSVC)
  set(PF_COMPILE_OPTIONS ${PF_COMPILE_OPTIONS}
    /W4)
endif()

#
# Pulsar -> Output
#
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(PF_OUTPUT_SYSTEM 64)
else()
  set(PF_OUTPUT_SYSTEM 32)
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
  set(PF_OUTPUT_SYSTEM win${PF_OUTPUT_SYSTEM})
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
  set(PF_OUTPUT_SYSTEM lin${PF_OUTPUT_SYSTEM})
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
  set(PF_OUTPUT_SYSTEM osx${PF_OUTPUT_SYSTEM})
else()
  message(FATAL_ERROR "[pulsar] Invalid host system!")
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(PF_OUTPUT_SYSTEM _debug${PF_OUTPUT_SYSTEM})
endif()

#
# Pulsar
#
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(PF_RUNTIME_MACRO PF_DEBUG=1)
else()
  set(PF_RUNTIME_MACRO PF_RELEASE=1)
endif()

#
# Pulsar -> Options
#
option(PF_BUILD_SHARED "Build shared library" ON)
option(PF_BUILD_STATIC "Build static library" ON)
option(PF_BUILD_OBJECT "Build object library" ON)
option(PF_BUILD_TESTS "Build tests executables" ON)

#
# Pulsar -> Include Dirs
#
include(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT EQUAL TRUE)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

#
# Pulsar -> CXX
#
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pulsar -> Git Get Latest Version
function(pf_find_git_version)
endfunction()

# Git
# https://ipenguin.ws/2012/11/cmake-automatically-use-git-tags-as.html
# https://github.com/Munkei/munkei-cmake/blob/master/MunkeiVersionFromGit.cmake
# Tag -> Release
# Tag -> Dev

# VERSION_STRING="v1.0.0-dev-FK39FJ39"
# VERSION_STRING="v1.0.0-release-2104UIFFE"

#
# Pulsar -> CPM
# Based on https://github.com/cpm-cmake/CPM.cmake/blob/master/cmake/get_cpm.cmake
# Find latest version
pf_find_git_version(PF_CPM_VERSION)

# Cache
if(CPM_SOURCE_CACHE)
  set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
  set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

# Expand relative path. This is important if the provided path contains a tilde (~)
get_filename_component(CPM_DOWNLOAD_LOCATION ${CPM_DOWNLOAD_LOCATION} ABSOLUTE)

if(NOT(EXISTS ${CPM_DOWNLOAD_LOCATION}))
  message(STATUS "CPM: Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
  file(
    DOWNLOAD
    "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake"
    ${CPM_DOWNLOAD_LOCATION}
  )
endif()

# Include CPM.cmake
include(${CPM_DOWNLOAD_LOCATION})

#
# Pulsar -> Shared
#
if(PF_BUILD_SHARED EQUAL ON)
endif()

#
# Pulsar -> Static
#
if(PF_BUILD_STATIC EQUAL ON)
endif()

#
# Pulsar -> Object
#
if(PF_BUILD_OBJECT EQUAL ON)
endif()

#
# Pulsar -> Tests
#
if(PF_BUILD_TESTS EQUAL ON)
endif()

#
# Pulsar -> CPM -> Package Project
#
CPMAddPackage(NAME packageproject GIT_TAG master)