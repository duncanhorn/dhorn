cmake_minimum_required(VERSION 3.12)

include(../cmake/build_flags.cmake)

project(functional_tests)

set(SOURCES
    main.cpp)

set(INCLUDES ${DHORN_INCLUDE_PATH})

add_executable(functional_tests)
target_sources(functional_tests PUBLIC ${SOURCES})
target_include_directories(functional_tests PUBLIC ${INCLUDES})
