cmake_minimum_required(VERSION 3.12)

include(../cmake/build_flags.cmake)

project(d3d_tests)

set(SOURCES
    main.cpp)

set(INCLUDES ${DHORN_INCLUDE_PATH})

add_executable(d3d_tests WIN32)
target_sources(d3d_tests PUBLIC ${SOURCES})
target_include_directories(d3d_tests PUBLIC ${INCLUDES})
