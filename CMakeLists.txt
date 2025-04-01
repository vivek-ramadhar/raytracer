cmake_minimum_required(VERSION 3.30)
project(untitled)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

find_package(OpenMP REQUIRED)
if(OpenMPP_CXX_FOUND)
    target_link_libraries(untitled PUBLIC OpenMP::OpenMP_CXX)
endif()

add_executable(untitled main.cpp
        vec3.h
        color.h
        ray.h
        hittable.h
        sphere.h
        hittable_list.h
        rtweekend.h
        interval.h
        camera.h
        material.h
        aabb.h
        bvh.h
        texture.h
        external/stb_image.h
        rtw_stb_image.h
        perlin.h
        quad.h
        constant_medium.h)
