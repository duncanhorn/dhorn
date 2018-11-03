cmake_minimum_required(VERSION 3.12)

set(DHORN_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(benchmark)
add_subdirectory(googletest)

add_subdirectory(d3d_tests)
add_subdirectory(efficiency_tests)
add_subdirectory(functional_tests)
add_subdirectory(tests)
add_subdirectory(window_tests)
