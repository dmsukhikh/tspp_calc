cmake_minimum_required(VERSION 3.2)
project('tspp_calc')
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DEBUG_BUILD "Project will build for debug" ON)

if (DEBUG_BUILD)
    set(CMAKE_BUILD_TYPE Debug)
    # for language server support
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # main.cpp for manual testing
    add_executable(main test/main.cpp)
    target_include_directories(main PRIVATE include)
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic 
                                        -std=c++17 -g -fsanitize=address
                                        -fsanitize=leak)
    target_link_options(main PRIVATE -fsanitize=address)

    # testing
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/6910c9d9165801d8827d628cb72eb7ea9dd538c5.zip
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(tests test/test_basic.cpp)
    target_include_directories(tests PRIVATE include)
    target_link_libraries(
      tests
      GTest::gtest_main
    )
    include(GoogleTest)
    gtest_discover_tests(tests)
endif()

