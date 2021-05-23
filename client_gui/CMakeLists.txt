cmake_minimum_required(VERSION 3.19)

project(client_gui)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../submodules/magnum/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../submodules/magnum-integration/modules" ${CMAKE_MODULE_PATH})

SET(BUILD_STATIC ON CACHE BOOL "")
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../submodules/imgui)
SET(WITH_IMGUI ON CACHE BOOL "" FORCE)
add_subdirectory(../submodules/corrade EXCLUDE_FROM_ALL corrade)
add_subdirectory(../submodules/GLFW EXCLUDE_FROM_ALL glfw)
SET(WITH_GLFWAPPLICATION ON CACHE BOOL "")
add_subdirectory(../submodules/magnum EXCLUDE_FROM_ALL magnum)
add_subdirectory(../submodules/magnum-integration EXCLUDE_FROM_ALL magnum-integration)

find_package(GLFW REQUIRED)
find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED
    GL
    Shaders
    MeshTools
    Primitives
    Shaders
    GlfwApplication
)
find_package(MagnumIntegration REQUIRED ImGui)

aux_source_directory(. source)
aux_source_directory(../common source)

add_executable(${PROJECT_NAME} ${source})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STACK_SIZE 100000000)

target_include_directories(${PROJECT_NAME} PUBLIC "../submodules/asio/asio/include")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/eigen")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/ZSerializer")

target_link_libraries(${PROJECT_NAME} PRIVATE
    GLFW::GLFW
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::Shaders
    MagnumIntegration::ImGui)

install(TARGETS ${PROJECT_NAME})