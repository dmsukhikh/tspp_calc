cmake_minimum_required(VERSION 3.2)
project('tspp_calc')

option(DEBUG_BUILD "Project is builded for debug" ON)

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
endif()

