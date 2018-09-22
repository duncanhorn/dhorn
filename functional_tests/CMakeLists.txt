cmake_minimum_required(VERSION 3.12)

project(functional_tests)

set(SOURCES
    main.cpp)

set(INCLUDES ${DHORN_INCLUDE_PATH})

add_executable(functional_tests WIN32)
target_sources(functional_tests PUBLIC ${SOURCES})
target_include_directories(functional_tests PUBLIC ${INCLUDES})
