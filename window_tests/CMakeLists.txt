cmake_minimum_required(VERSION 3.12)

project(window_tests)

set(SOURCES
    main.cpp
    worker.cpp)

set(INCLUDES ${DHORN_INCLUDE_PATH})

add_executable(window_tests WIN32)
target_sources(window_tests PUBLIC ${SOURCES})
target_include_directories(window_tests PUBLIC ${INCLUDES})
