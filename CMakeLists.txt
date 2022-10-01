cmake_minimum_required(VERSION 3.12)
project(PiecewiseGeometricModelIndex
        VERSION 1.0
        HOMEPAGE_URL https://github.com/gvinciguerra/PGM-index)

option(BUILD_EXAMPLES "Build the examples" ON)
option(BUILD_PGM_TUNER "Build the tuner target" OFF)
option(BUILD_PGM_BENCHMARK "Build the benchmark target" OFF)

include(CheckCXXCompilerFlag)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")

include_directories(include)

# PGM-index library
add_library(pgmindexlib INTERFACE)
target_include_directories(pgmindexlib INTERFACE include/)

if (BUILD_PGM_TUNER)
    add_subdirectory(tuner)
endif ()

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (BUILD_PGM_BENCHMARK)
    add_subdirectory(benchmark)
endif ()
