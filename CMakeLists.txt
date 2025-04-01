cmake_minimum_required(VERSION 3.10)
project(RayTracer VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenMP package
find_package(OpenMP REQUIRED)

# Set build type to Release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# All .h files in the project
file(GLOB HEADER_FILES "*.h")

# All source files
file(GLOB SRC_FILES "*.cpp" "*.cc")

# Create an external directory where we place stb_image.h
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external)

# Main executable
add_executable(raytracer main.cpp ${HEADER_FILES})
target_link_libraries(raytracer ${OpenMP_CXX_LIBRARIES})

# Additional executables if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pi.cc")
    add_executable(pi pi.cc)
    target_link_libraries(pi ${OpenMP_CXX_LIBRARIES})
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pi_conv.cc")
    add_executable(pi_conv pi_conv.cc)
    target_link_libraries(pi_conv ${OpenMP_CXX_LIBRARIES})
endif()

# Create directory for output images
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/images)

# Create a command to copy image files if they exist
add_custom_target(copy_assets 
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/images
    COMMAND ${CMAKE_COMMAND} -E echo "Copying image assets to build directory"
)

# Add dependency so assets are copied before running the executable
add_dependencies(raytracer copy_assets)

# Install targets
install(TARGETS raytracer 
        RUNTIME DESTINATION bin)

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "OpenMP Found: ${OPENMP_FOUND}")
