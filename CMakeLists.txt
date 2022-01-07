# Minimum CMake version
cmake_minimum_required(VERSION 3.18)

# Default
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../")
endif()

# Options
option(BUILD_TESTING "[pulsar]: Build test executable(s)?" On)

# Version (from git local repository)
find_package(Git)
if(NOT GIT_FOUND)
  message(FATAL_ERROR "[pulsar]: Need git installed on this machine!")
endif()
execute_process(
  COMMAND "${GIT_EXECUTABLE}" describe --tags --always
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE VERSION_STRING
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(TOLOWER "${PROJECT_NAME}/pulsar_version.hpp" VERSION_HEADER_PATH)

# Version extracting from full version string
string(REGEX REPLACE "([0-9]+).*" "\\1" VERSION_MAJOR "${VERSION_STRING}")
string(REGEX REPLACE "[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION_STRING}")
string(REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION_STRING}")

# Commit hash value (SHA-1) -> Version
execute_process(
  COMMAND "${GIT_EXECUTABLE}" rev-parse --short=7 HEAD
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE VERSION_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(VERSION_STRING ${VERSION_STRING}-${VERSION_HASH})
message(STATUS "[pulsar]: v${VERSION_STRING}")

# Project
project(
  pulsar
  LANGUAGES CXX
  VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Version File
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pulsar_version.hpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pulsar/pulsar_version.hpp"
)

# CPM
if(NOT EXISTS CPM_DOWNLOAD_VERSION)
  set(CPM_DOWNLOAD_VERSION 0.34.2)
endif()
set(CPM_DOWNLOAD_LOCATION "${CMAKE_SOURCE_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  file(DOWNLOAD
       https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
       ${CPM_DOWNLOAD_LOCATION}
  )
endif()
include(cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake)

# Additionnal parameters (Warnings L4)
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

# Library
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(${PROJECT_NAME} INTERFACE ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} LINKER_LANGUAGE CXX)
target_include_directories(
  ${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include>
)

# CPM -> PackageProject
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.8.0")

# Package
packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/pulsar
  VERSION_HEADER "${VERSION_HEADER_PATH}"
  COMPATIBILITY SameMajorVersion
)

# Testing
if(BUILD_TESTING)
  include(CTest)
  file(
    GLOB_RECURSE TEST_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "test/*.cpp"
  )
  foreach(TEST_SOURCE IN ITEMS ${TEST_SOURCES})
    get_filename_component(BINARY_NAME ${TEST_SOURCE} NAME_WLE)
    add_executable(${BINARY_NAME} ${TEST_SOURCE})
    add_test(NAME ${BINARY_NAME} COMMAND ${BINARY_NAME})
    target_link_libraries(${BINARY_NAME} PRIVATE ${PROJECT_NAME})
  endforeach()
endif()
