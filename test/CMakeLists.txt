#define CATCH_CONFIG_MAIN
cmake_minimum_required(VERSION 3.14)

project(test)

aux_source_directory(. source)
aux_source_directory(../common common)

set(test_id 0)

foreach(test ${source})
    set(test_name "test_${test_id}")
    math(EXPR test_id "${test_id}+1")

    add_executable(${test_name} ${test} ${common})

    set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${test_name} PROPERTY CXX_STANDARD_REQUIRED ON)

    set(CMAKE_CXX_STACK_SIZE 100000000)

    target_include_directories(${test_name} PUBLIC "../submodules/asio/asio/include")
    target_include_directories(${test_name} PUBLIC "../submodules/Catch2/single_include/catch2")

endforeach()