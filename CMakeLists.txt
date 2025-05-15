cmake_minimum_required(VERSION 3.5)
project('tspp_calc')
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug" FORCE)
endif()

add_library(build_features INTERFACE)
target_include_directories(build_features INTERFACE include)

# Исполняемый файл утилиты
add_executable(main src/main.cpp
                    src/Calculator.cpp)
target_link_libraries(main PRIVATE build_features)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    # for language server support
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    set(COMPILE_FLAGS -Wall -Wextra -Wpedantic 
    -std=c++14 -g -fsanitize=address -fsanitize=leak)
    target_compile_options(build_features INTERFACE ${COMPILE_FLAGS})
    target_link_options(build_features INTERFACE -fsanitize=address)


    # main.cpp for manual testing
    add_executable(main_test test/main_test.cpp)
    target_link_libraries(main_test PRIVATE build_features)

    # testing
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/6910c9d9165801d8827d628cb72eb7ea9dd538c5.zip
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(tests test/test_basic.cpp
                         test/test_funcs.cpp
                         test/test_ops.cpp)
    target_link_libraries(
      tests
      GTest::gtest_main
    )
    target_link_libraries(tests build_features)

    include(GoogleTest)
    gtest_discover_tests(tests)

endif()

set(FILES_FOR_INSTALL include/MatrixOperation.hpp
                      include/MatrixGeneric.hpp
                      include/Exceptions.hpp)

install(FILES ${FILES_FOR_INSTALL} DESTINATION matrix_calc)

