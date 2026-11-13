cmake_minimum_required(VERSION 3.15...3.30)

project(PasswordGuard VERSION 1.0
        DESCRIPTION "Production password policy validation tool"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Disallow in-source builds
function(disallow_in_source_builds)
  if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    message(FATAL_ERROR "ERROR: In-source builds are not allowed! Create a 'build' directory.")
  endif()
endfunction()
disallow_in_source_builds()

# 2. Communicate Compiler and Git Commit SHA1 into C++
set(COMPILER ${CMAKE_CXX_COMPILER})
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HEAD_SHA1
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT GIT_HEAD_SHA1)
  set(GIT_HEAD_SHA1 "initial")
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/src/BuildConfig.hpp.in"
  "${PROJECT_BINARY_DIR}/BuildConfig.hpp"
)

# 3. Third-Party Library Integration: nlohmann/json
include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json)

# 4. Core Library Target
add_library(pg_core
  src/Policies.cpp
  src/ReportAdapter.cpp
)
target_include_directories(pg_core PUBLIC
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
)
target_link_libraries(pg_core PUBLIC nlohmann_json::nlohmann_json)

# 5. Main Executable Target
add_executable(passwordguard src/main.cpp)
target_link_libraries(passwordguard PRIVATE pg_core)

# 6. Automated Unit Testing
option(ENABLE_TESTS "Enable unit tests" ON)
if(ENABLE_TESTS)
  enable_testing()
  add_executable(unit_tests tests/test_main.cpp)
  target_link_libraries(unit_tests PRIVATE pg_core)
  add_test(NAME PasswordGuardTests COMMAND unit_tests)
endif()

# 7. Automated Documentation Generation with Doxygen
option(BUILD_DOC "Build API documentation with Doxygen" ON)
if(BUILD_DOC)
  find_package(Doxygen QUIET)
  if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${PROJECT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMENT "Generating HTML API documentation with Doxygen"
      VERBATIM
    )
  else()
    message(STATUS "Doxygen not found. Target 'doc_doxygen' will not be built.")
  endif()
endif()
