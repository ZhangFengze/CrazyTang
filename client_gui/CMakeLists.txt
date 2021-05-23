cmake_minimum_required(VERSION 3.19)

project(client_gui)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../submodules/magnum/modules" ${CMAKE_MODULE_PATH})

SET(BUILD_STATIC ON CACHE BOOL "")
add_subdirectory(../submodules/corrade EXCLUDE_FROM_ALL corrade)
SET(WITH_WGLCONTEXT ON CACHE BOOL "")
add_subdirectory(../submodules/magnum EXCLUDE_FROM_ALL magnum)
add_subdirectory(../submodules/GLFW EXCLUDE_FROM_ALL glfw)

find_package(GLFW REQUIRED)
find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED GL Shaders)
find_package(Magnum REQUIRED WglContext)

aux_source_directory(. source)
aux_source_directory(../common source)

file(GLOB imgui_source ../submodules/imgui/imgui*.cpp)
list(APPEND imgui_source
"../submodules/imgui/backends/imgui_impl_glfw.cpp"
"../submodules/imgui/backends/imgui_impl_opengl3.cpp"
"../submodules/imgui/examples/libs/gl3w/GL/gl3w.c"
)

add_executable(${PROJECT_NAME} ${source} ${imgui_source})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STACK_SIZE 100000000)

target_include_directories(${PROJECT_NAME} PUBLIC "../submodules/asio/asio/include")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/eigen")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/ZSerializer")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/imgui")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/imgui/backends")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/imgui/examples/libs/gl3w")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/imgui/examples/libs/glfw/include")

target_link_libraries(${PROJECT_NAME} PRIVATE
    GLFW::GLFW
    Corrade::Main
    Magnum::GL
    Magnum::GLContext
    Magnum::Magnum
    Magnum::Shaders)

install(TARGETS ${PROJECT_NAME})