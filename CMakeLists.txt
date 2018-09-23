cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    # Modify the default flags CMake adds
    foreach (flag
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)

        string(REGEX REPLACE "/W[0-4]" "/W4" ${flag} "${${flag}}")
        string(APPEND ${flag} " /WX")
        add_compile_options(/permissive-)

    endforeach()
else()
# TODO: More compilers
endif()

add_definitions(/DNOMINMAX /DWIN32_LEAN_AND_MEAN /DUNICODE /D_UNICODE)

set(DHORN_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(googletest)

add_subdirectory(d3d_tests)
add_subdirectory(functional_tests)
add_subdirectory(window_tests)
