#define CATCH_CONFIG_MAIN
cmake_minimum_required(VERSION 3.14)

project(test)

aux_source_directory(. source)
aux_source_directory(../common common)
aux_source_directory(../server server)
list(REMOVE_ITEM server "../server/main.cpp")

foreach(test ${source})
    string(SUBSTRING ${test} 2 -1 test)

    set(test_name "${test}")

    add_executable(${test_name} ${test} ${common} ${server})

    set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${test_name} PROPERTY CXX_STANDARD_REQUIRED ON)

    set(CMAKE_CXX_STACK_SIZE 100000000)

    target_include_directories(${test_name} PUBLIC "../submodules/asio/asio/include")
    target_include_directories(${test_name} PUBLIC "../submodules/Catch2/single_include/catch2")
    target_include_directories(${test_name} PUBLIC "../submodules/eigen")
endforeach()