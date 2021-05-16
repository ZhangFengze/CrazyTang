cmake_minimum_required(VERSION 3.19)

project(client_cli)

aux_source_directory(. source)
aux_source_directory(../common source)
add_executable(${PROJECT_NAME} ${source})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STACK_SIZE 100000000)

target_include_directories(${PROJECT_NAME} PUBLIC "../submodules/asio/asio/include")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/eigen")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/ZSerializer")

install(TARGETS ${PROJECT_NAME})