cmake_minimum_required(VERSION 3.10)

# Project settings
project(Simulation VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define build modes (Debug or Release)
option(DEBUG "Enable Debugging" ON)

# Set the project directories
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(SRC_DIR ${PROJECT_ROOT}/src)
set(LIBS_DIR ${PROJECT_ROOT}/libs)
set(IMGUI_DIR ${LIBS_DIR}/imgui)
set(BACKENDS_DIR ${IMGUI_DIR}/backends)
set(VEC2_DIR ${SRC_DIR}/vector2d)
set(SIMULATION_DIR ${SRC_DIR}/simulation)
set(BUILD_DIR ${PROJECT_ROOT}/build)

# Find all source files
file(GLOB_RECURSE SOURCES 
    ${SRC_DIR}/*.cpp 
    ${IMGUI_DIR}/*.cpp
)

# Define object directories
set(OBJ_DIR ${BUILD_DIR}/objects)

# Set the compiler flags for Debug and Release
if(DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else()
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Include directories
include_directories(
    ${IMGUI_DIR}
    ${BACKENDS_DIR}
    ${VEC2_DIR}
    ${SIMULATION_DIR}
)

# Define libraries
set(LIBS -lGL -lglfw)

# Define the executable
add_executable(simulation_try_1 ${SOURCES})

# Link libraries
target_link_libraries(simulation_try_1 ${LIBS})

# Clean target with a different name (to avoid reserved name)
add_custom_target(clean_build
    COMMAND rm -rf ${OBJ_DIR} ${BUILD_DIR}/simulation_try_1
    COMMENT "Clean build files"
)

# Debug and release targets (if you want them explicitly defined)
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ..
    COMMENT "Build in Debug mode"
)

add_custom_target(release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ..
    COMMENT "Build in Release mode"
)
