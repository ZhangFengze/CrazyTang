cmake_minimum_required(VERSION 3.19)

project(server)

aux_source_directory(. source)
aux_source_directory(../common source)
add_executable(${PROJECT_NAME} ${source})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS " -static")
set(CMAKE_CXX_STACK_SIZE 100000000)

target_include_directories(${PROJECT_NAME} PUBLIC "../submodules/asio/asio/include")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/eigen")

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${PROJECT_NAME})