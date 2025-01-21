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
set(STB_DIR ${LIBS_DIR}/stb)
set(BACKENDS_DIR ${IMGUI_DIR}/backends)
set(SIMULATION_DIR ${SRC_DIR}/simulation)
set(GUI_DIR ${SIMULATION_DIR}/gui)  # Added correct path for gui.cpp

# Find all source files
file(GLOB_RECURSE SOURCES 
    ${SRC_DIR}/*.cpp 
    ${IMGUI_DIR}/*.cpp
)

# Validate that the required file exists
if(NOT EXISTS "${GUI_DIR}/gui.cpp")
    message(FATAL_ERROR "Missing required file: ${GUI_DIR}/gui.cpp")
endif()

# Include directories
include_directories(
    ${IMGUI_DIR}
    ${STB_DIR}
    ${BACKENDS_DIR}
    ${SIMULATION_DIR}
    ${GUI_DIR}  # Include GUI directory
    ${SRC_DIR}
)

# Set the compiler flags for Debug and Release
if(DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable Position Independent Code (PIC) for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set CMake to use vcpkg toolchain for dependency management
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Define the executable
add_executable(simulation_try_1 ${SOURCES})

# Link libraries
target_link_libraries(simulation_try_1 PRIVATE glfw OpenGL::GL)

# Clean target for removing build files
add_custom_target(clean_build
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMENT "Clean build files"
)

# Debug and Release configuration options
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build enabled")
    target_compile_definitions(simulation_try_1 PRIVATE DEBUG_MODE=1)
else()
    message(STATUS "Release build enabled")
endif()
