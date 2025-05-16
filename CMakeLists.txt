cmake_minimum_required(VERSION 3.5)
project('tspp_calc')
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# for language server support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT DEFINED TESTS)
    set(TESTS true)
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug" FORCE)
endif()

add_library(build_features INTERFACE)
target_include_directories(build_features INTERFACE include)

# Исполняемый файл утилиты
add_executable(tspp_calc ${CMAKE_SOURCE_DIR}/src/main.cpp)
add_library(calc_ins ${CMAKE_SOURCE_DIR}/src/Calculator.cpp)
target_link_libraries(calc_ins PUBLIC build_features)
target_link_libraries(tspp_calc PRIVATE calc_ins)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(COMPILE_FLAGS -Wall -Wextra -Wpedantic 
    -std=c++14 -g -fsanitize=address -fsanitize=leak)
    target_compile_options(build_features INTERFACE ${COMPILE_FLAGS})
    target_link_options(build_features INTERFACE -fsanitize=address)
endif()

if (TESTS)
    # main.cpp for manual testing
    set(INSTALL_GTEST OFF CACHE BOOL "Disable install of googletest" FORCE)
    add_executable(main_test ${CMAKE_SOURCE_DIR}/test/main_test.cpp)
    target_link_libraries(main_test PRIVATE build_features)

    # testing
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/6910c9d9165801d8827d628cb72eb7ea9dd538c5.zip
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(tests ${CMAKE_SOURCE_DIR}/test/test_basic.cpp
                         ${CMAKE_SOURCE_DIR}/test/test_funcs.cpp
                         ${CMAKE_SOURCE_DIR}/test/test_ops.cpp
                         ${CMAKE_SOURCE_DIR}/test/test_util.cpp)
    target_link_libraries(
      tests
      GTest::gtest_main
    )
    target_link_libraries(tests build_features)
    target_link_libraries(tests calc_ins)

    add_custom_command(
        TARGET tests PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test/test_suite $<TARGET_FILE_DIR:tests>/test_suite)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()

set(FILES_FOR_INSTALL  ${CMAKE_SOURCE_DIR}/include/MatrixOperation.hpp
                       ${CMAKE_SOURCE_DIR}/include/MatrixGeneric.hpp
                       ${CMAKE_SOURCE_DIR}/include/Exceptions.hpp)
                      
install(FILES ${FILES_FOR_INSTALL} DESTINATION /usr/include)
install(TARGETS tspp_calc DESTINATION /usr/bin)

