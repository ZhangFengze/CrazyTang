cmake_minimum_required(VERSION 3.14)

project(client_core)

aux_source_directory(. source)
aux_source_directory(../common source)
add_library(${PROJECT_NAME} STATIC ${source})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(${PROJECT_NAME} PUBLIC "../submodules/asio/asio/include")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/eigen")