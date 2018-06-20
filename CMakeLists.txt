cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Download and unpack googletest at configure time
configure_file(CMakeLists_gtest.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                RESULT_VARIABLE result
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
                RESULT_VARIABLE result
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Build googletest only
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON  CACHE BOOL "" FORCE)
set(INSTALL_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# Include axcc source file directory
include_directories(axcc/src)

# Build tests
add_executable(token_test token_test.cc axcc/src/token.cc)
target_link_libraries(token_test gtest_main)
add_test(NAME run_token_test COMMAND token_test)

add_executable(scanner_test scanner_test.cc axcc/src/token.cc
               axcc/src/scanner.cc)
target_link_libraries(scanner_test gtest_main)
add_test(NAME run_scanner_test COMMAND scanner_test
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testfiles)

# Do not include preprocessor.cc because it has already been included by
# preprocessor_test.cc
add_executable(preprocessor_test preprocessor_test.cc axcc/src/token.cc
               axcc/src/scanner.cc)
target_link_libraries(preprocessor_test gtest_main)
add_test(NAME run_preprocessor_test COMMAND preprocessor_test
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testfiles)
