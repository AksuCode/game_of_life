# Set cmake minimum version
cmake_minimum_required(VERSION 3.15)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Declare project
project(MGL VERSION 0.0.01 DESCRIPTION "For now the name is MGL (=My Game Library)")

# Set output directory for the static library
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add include directory
include_directories(include)

# Add source files from ./src
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Set the output directory for the static library
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)

# Create a static library
add_library(MGL STATIC ${SOURCES})

# Specify the output name of the library
set_target_properties(MGL PROPERTIES OUTPUT_NAME "MGL")

# Add CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/sdl2")

# Add SDL2 library
find_package(SDL2 REQUIRED)

# Platform-specific configurations
if (WIN32)
    # Additional configuration for Windows
    ### NOT YET IMPLEMENTED ###
elseif (UNIX)
    # Link SDL2 libraries and include directories
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES} pthread)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
endif()