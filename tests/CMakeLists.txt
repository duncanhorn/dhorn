cmake_minimum_required(VERSION 3.12)

include(../cmake/build_flags.cmake)

if (MSVC)
    foreach (flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        # GoogleTest builds with /MT, so therefore we do, too
        string(REPLACE "/MD" "-MT" ${flag_var} "${${flag_var}}")

        # Disable some clang warnings
        string(APPEND ${flag_var} " -Wno-self-assign-overloaded")
        string(APPEND ${flag_var} " -Wno-missing-field-initializers")
    endforeach()
endif()

project(tests)

set(SOURCES
    AlgorithmTests.cpp
    AllIncludeTest.cpp
#    AnimationManagerTests.cpp
#    AnimationTests.cpp
#    ArrayReferenceTests.cpp
#    AsyncTests.cpp
    BitmaskTests.cpp
#    CommandLineTests.cpp
    CompressedBaseTests.cpp
    CompressedPairTests.cpp
    ComPtrTests.cpp
    ComTraitsTests.cpp
    CRTPBaseTests.cpp
#    EventSourceTests.cpp
#    FunctionalTest.cpp
    FunctionalTests.cpp
    GuidTests.cpp
#    HStringTests.cpp
    InplaceFunctionTests.cpp
#    InstanceTests.cpp
#    IteratorTests.cpp
#    JsonScannerTests.cpp
#    JSONTests.cpp
    main.cpp
#    MessageQueueTests.cpp
#    NumericTests.cpp
    ScopeGuardTests.cpp
#    ServiceContainerTests.cpp
#    SocketsTests.cpp
#    SocketStreamTests.cpp
#    StringLiteralTests.cpp
    StringTests.cpp
#    SynchronizedObjectTests.cpp
    ThreadPoolTests.cpp
    TypeTraitsTests.cpp
    UnicodeEncodingTests.cpp
    UnicodeIteratorTests.cpp
#    UniqueAnyTests.cpp
#    UnitsTests.cpp
#    UtfStringTests.cpp
    UtilityTests.cpp
#    UUIDTests.cpp
    VisitorTests.cpp
    WeakRefTests.cpp
#    WindowsTests.cpp
    )

set(INCLUDES
    ${DHORN_INCLUDE_PATH}
    ../googletest/googletest/include)

add_executable(tests)
target_sources(tests PUBLIC ${SOURCES})
target_include_directories(tests PUBLIC ${INCLUDES})
target_link_libraries(tests gtest)
