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
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/json/single_include")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/FastNoiseLite/Cpp")
target_include_directories(${PROJECT_NAME} PUBLIC  "../submodules/PhysX/physx/include")

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


SET(CMAKE_USE_OPENSSL OFF CACHE BOOL "")
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG c8d33915dbd88ad6c92b258869b03aba06587ff9) # the commit hash for 1.5.0
FetchContent_MakeAvailable(cpr)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)


#### Build PhysX library ####
# PHYSX_PATH - path to the `{cloned repository}/physx` repo directory git://github.com/NVIDIAGameWorks/PhysX.git
set( PHYSX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../submodules/PhysX/physx )
set( PHYSX_ROOT_DIR ${PHYSX_PATH} ) #This is needed for $ENV{PHYSX_PATH}/compiler/public/CMakeLists.txt
set( PHYSX_INCLUDE_DIRS ${PHYSX_PATH}/include/ ${PHYSX_PATH}/../pxshared/include/ )
set( PHYSX_LIBRARIES
	PhysXExtensions
	PhysX
	PhysXPvdSDK
	PhysXVehicle
	PhysXCharacterKinematic
	PhysXCooking
	PhysXCommon
	PhysXFoundation
	# SnippetUtils
)

set(TARGET_BUILD_PLATFORM "windows") # has to match the TARGET_BUILD_PLATFORM in $ENV{PHYSX_PATH}/physix/buildtools/cmake_generate_projects.py
set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")
set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
set(PX_GENERATE_STATIC_LIBRARIES ON CACHE BOOL "Generate static libraries")
set(PX_FLOAT_POINT_PRECISE_MATH OFF CACHE BOOL "Float point precise math")
set(NV_USE_STATIC_WINCRT ON CACHE BOOL "Use the statically linked windows CRT")
set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
set(PXSHARED_PATH ${PHYSX_PATH}/../pxshared)
set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CMAKEMODULES_VERSION "1.27")
set(CMAKEMODULES_PATH ${PHYSX_PATH}/../externals/cmakemodules)
set(PX_OUTPUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
set(PX_OUTPUT_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/externals/physx)
  
# Call into PhysX's CMake scripts
add_subdirectory(${PHYSX_PATH}/compiler/public externals/physx)

# Add physx libraries to target
target_link_libraries(${PROJECT_NAME} PUBLIC ${PHYSX_LIBRARIES})

install(TARGETS ${PROJECT_NAME})
