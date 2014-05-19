/*
 * D3DVectorEfficiencyTests.cpp
 *
 * Duncan Horn
 *
 * Efficiency tests for the d3d::vector class
 */

#pragma once

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include "dhorn/d3d.h"
#include "efficiency.h"

using namespace dhorn;
using namespace dhorn::tests;

#define GLOBAL_TEST_COUNT       2000000000l



static void printStats(
    _In_ const std::string &name,
    _In_ const std::vector<efficiency_test::time_val> &times)
{
    auto expect = times[0].count();
    auto actual = times[1].count();

    assert(times.size() == 2);
    printf("%-20s%-20lld%-20lld", name.c_str(), expect, actual);

    if (expect < actual)
    {
        // Slower; decrease = actual / expect
        double delta = (double)actual / expect;
        printf("%.3fx slower\n", delta);
    }
    else if (expect > actual)
    {
        // Faster; increase = expect / actual
        double delta = (double)expect / actual;
        printf("%.3fx faster\n", delta);
    }
    else
    {
        // Same
        printf("N/A\n");
    }
}

template <int _Dim>
DirectX::XMVECTOR GenerateRandomVector(void)
{
    return DirectX::XMVectorSet((float)rand(), (float)rand(),
        (_Dim >= 3) ? (float)rand() : 0, (_Dim >= 4) ? (float)rand() : 0);
}



template <int _Dim>
static void EqualityTest(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT;
    using traits = d3d::garbage::vector_traits<_Dim>;

    efficiency_test test;
    auto v1 = GenerateRandomVector<_Dim>();
    auto v2 = GenerateRandomVector<_Dim>();

    d3d::vector<_Dim> vector1(v1);
    d3d::vector<_Dim> vector2(v2);

    // We need to make sure the comparisons don't get optimized away
    static volatile bool result = false;

    // Test for DirectX::XMVector*Equal
    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        result = traits::equals(v1, v2) || result;
        v1 = DirectX::XMVectorSetX(v1, DirectX::XMVectorGetX(v1) + 1);
    }
    test.stop();
    if (result) std::cout << "hmm";

    // Test for vector == vector
    result = false;
    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        result = (vector1 == vector2) || result;
        vector1.x()++;
    }
    test.stop();
    if (result) std::cout << "hmm";

    std::stringstream name;
    name << "vector" << _Dim << " equality";
    printStats(name.str(), test.get_times());
}

static void EqualityTest(void)
{
    EqualityTest<2>();
    EqualityTest<3>();
    EqualityTest<4>();

    std::cout << std::endl;
}



template <int _Dim>
static void NegationTest(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT;

    efficiency_test test;
    auto v1 = GenerateRandomVector<_Dim>();
    auto v2 = GenerateRandomVector<_Dim>();

    d3d::vector<_Dim> vector1(v1);
    d3d::vector<_Dim> vector2(v2);

    // Test for -DirectX::XMVECTOR
    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        v1 = DirectX::XMVectorNegate(v1);
    }
    test.stop();
    if (DirectX::XMVectorGetX(v1) == 0) std::cout << "hmm";

    // Test for -d3d::vector
    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        vector1 = -vector1;
    }
    test.stop();
    if (vector1.x() == 0) std::cout << "hmm";

    std::stringstream name;
    name << "vector" << _Dim << " negation";
    printStats(name.str(), test.get_times());
}

static void NegationTest(void)
{
    NegationTest<2>();
    NegationTest<3>();
    NegationTest<4>();

    std::cout << std::endl;
}



template <int _Dim>
static void DotProductTest(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT;
    using traits = d3d::garbage::vector_traits<_Dim>;

    efficiency_test test;
    auto v1 = GenerateRandomVector<_Dim>();
    auto v2 = GenerateRandomVector<_Dim>();

    d3d::vector<_Dim> vector1(v1);
    d3d::vector<_Dim> vector2(v2);

    // To prevent optimization
    float total = 0;

    // Test for DirectX::XMVECTOR . DirectX::XMVECTOR
    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        total += DirectX::XMVectorGetX(traits::dot(v1, v2));
    }
    test.stop();
    if (total == 0) std::cout << "hmm";

    // Test for d3d::vector . d3d::vector
    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        total += vector1.dot_product(vector2);
    }
    test.stop();
    if (total == 0) std::cout << "hmm";

    std::stringstream name;
    name << "vector" << _Dim << " dot";
    printStats(name.str(), test.get_times());
}

static void DotProductTest(void)
{
    DotProductTest<2>();
    DotProductTest<3>();
    DotProductTest<4>();

    std::cout << std::endl;
}



static void CrossProductTest2(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT;

    efficiency_test test;
    auto v1 = GenerateRandomVector<2>();
    auto v2 = GenerateRandomVector<2>();

    d3d::vector2 vector1(v1);
    d3d::vector2 vector2(v2);

    // To prevent optimization
    float total = 0;

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        total += DirectX::XMVectorGetX(DirectX::XMVector2Cross(v1, v2));
    }
    test.stop();
    if (total == 0) std::cout << "hmm";

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        total += vector1.cross_product(vector2);
    }
    test.stop();
    if (total == 0) std::cout << "hmm";

    std::stringstream name;
    name << "vector2 cross";
    printStats(name.str(), test.get_times());
}

static void CrossProductTest3(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT;

    efficiency_test test;
    auto v1 = GenerateRandomVector<3>();
    auto v2 = GenerateRandomVector<3>();

    d3d::vector3 vector1(v1);
    d3d::vector3 vector2(v2);

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        v1 = DirectX::XMVector3Cross(v1, v2);
    }
    test.stop();
    if (DirectX::XMVectorGetX(v1) == 0) std::cout << "hmm";

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        vector1 = vector1.cross_product(vector2);
    }
    test.stop();
    if (vector1.x() == 0) std::cout << "hmm";

    std::stringstream name;
    name << "vector3 cross";
    printStats(name.str(), test.get_times());
}

static void CrossProductTest4(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT;

    efficiency_test test;
    auto v1 = GenerateRandomVector<4>();
    auto v2 = GenerateRandomVector<4>();
    auto v3 = GenerateRandomVector<4>();

    d3d::vector4 vector1(v1);
    d3d::vector4 vector2(v2);
    d3d::vector4 vector3(v3);

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        v1 = DirectX::XMVector4Cross(v1, v2, v3);
    }
    test.stop();
    if (DirectX::XMVectorGetX(v1) == 0) std::cout << "hmm";

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        vector1 = vector1.cross_product(vector2, vector3);
    }
    test.stop();
    if (vector1.x() == 0) std::cout << "hmm";

    std::stringstream name;
    name << "vector4 cross";
    printStats(name.str(), test.get_times());
}

static void CrossProductTest(void)
{
    CrossProductTest2();
    CrossProductTest3();
    CrossProductTest4();

    std::cout << std::endl;
}



template <int _Dim>
static void NormalizeTest(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT;
    using traits = d3d::garbage::vector_traits<_Dim>;

    efficiency_test test;
    auto v1 = GenerateRandomVector<_Dim>();

    d3d::vector<_Dim> vector1(v1);

    // To prevent optimizations
    float total = 0;

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        total += DirectX::XMVectorGetX(traits::normalize(vector1));
    }
    test.stop();
    if (total == 0) std::cout << "hmm";

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        total += DirectX::XMVectorGetX(vector1.normalize());
    }
    test.stop();
    if (total == 0) std::cout << "hmm";

    std::stringstream name;
    name << "vector" << _Dim << " normalize";
    printStats(name.str(), test.get_times());
}

static void NormalizeTest(void)
{
    NormalizeTest<2>();
    NormalizeTest<3>();
    NormalizeTest<4>();

    std::cout << std::endl;
}



template <int _Dim>
static void AdditionTest(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT / 2;
    using test_type = d3d::vector<_Dim>;

    efficiency_test test;
    auto v1 = GenerateRandomVector<_Dim>();
    auto v2 = GenerateRandomVector<_Dim>();
    auto v3 = GenerateRandomVector<_Dim>();
    auto v4 = GenerateRandomVector<_Dim>();
    auto v5 = GenerateRandomVector<_Dim>();
    auto v6 = GenerateRandomVector<_Dim>();

    test_type vector1(v1);
    test_type vector2(v2);
    test_type vector3(v3);
    test_type vector4(v4);
    test_type vector5(v5);
    test_type vector6(v6);

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        auto t1 = DirectX::XMVectorAdd(v1, v2);
        auto t2 = DirectX::XMVectorAdd(v3, v4);
        auto t3 = DirectX::XMVectorAdd(v5, v6);

        auto t4 = DirectX::XMVectorAdd(t1, t2);
        auto t5 = DirectX::XMVectorAdd(t2, t3);

        auto t6 = DirectX::XMVectorAdd(t4, t5);

        v1 = DirectX::XMVectorAdd(v1, t6);
        v2 = DirectX::XMVectorAdd(v2, t6);
        v3 = DirectX::XMVectorAdd(v3, t6);
        v4 = DirectX::XMVectorAdd(v4, t6);
        v5 = DirectX::XMVectorAdd(v5, t6);
        v6 = DirectX::XMVectorAdd(v6, t6);
    }
    test.stop();
    if (DirectX::XMVectorGetX(v1) == 0) std::cout << "hmm";

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        test_type t1 = vector1 + vector2;
        test_type t2 = vector3 + vector4;
        test_type t3 = vector5 + vector6;

        test_type t4 = t1 + t2;
        test_type t5 = t2 + t3;

        test_type t6 = t4 + t5;

        vector1 += t6;
        vector2 += t6;
        vector3 += t6;
        vector4 += t6;
        vector5 += t6;
        vector6 += t6;
    }
    test.stop();
    if (vector1.x() == 0) std::cout << "hmm";

    std::stringstream name;
    name << "vector" << _Dim << " addition";
    printStats(name.str(), test.get_times());
}

static void AdditionTest(void)
{
    AdditionTest<2>();
    AdditionTest<3>();
    AdditionTest<4>();

    std::cout << std::endl;
}



template <int _Dim>
static void SubtractionTest(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT / 2;
    using test_type = d3d::vector<_Dim>;

    efficiency_test test;
    auto v1 = GenerateRandomVector<_Dim>();
    auto v2 = GenerateRandomVector<_Dim>();
    auto v3 = GenerateRandomVector<_Dim>();
    auto v4 = GenerateRandomVector<_Dim>();
    auto v5 = GenerateRandomVector<_Dim>();
    auto v6 = GenerateRandomVector<_Dim>();

    test_type vector1(v1);
    test_type vector2(v2);
    test_type vector3(v3);
    test_type vector4(v4);
    test_type vector5(v5);
    test_type vector6(v6);

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        auto t1 = DirectX::XMVectorSubtract(v1, v2);
        auto t2 = DirectX::XMVectorSubtract(v3, v4);
        auto t3 = DirectX::XMVectorSubtract(v5, v6);

        auto t4 = DirectX::XMVectorSubtract(t1, t2);
        auto t5 = DirectX::XMVectorSubtract(t2, t3);

        auto t6 = DirectX::XMVectorSubtract(t4, t5);

        v1 = DirectX::XMVectorSubtract(v1, t6);
        v2 = DirectX::XMVectorSubtract(v2, t6);
        v3 = DirectX::XMVectorSubtract(v3, t6);
        v4 = DirectX::XMVectorSubtract(v4, t6);
        v5 = DirectX::XMVectorSubtract(v5, t6);
        v6 = DirectX::XMVectorSubtract(v6, t6);
    }
    test.stop();
    if (DirectX::XMVectorGetX(v1) == 0) std::cout << "hmm";

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        test_type t1 = vector1 - vector2;
        test_type t2 = vector3 - vector4;
        test_type t3 = vector5 - vector6;

        test_type t4 = t1 - t2;
        test_type t5 = t2 - t3;

        test_type t6 = t4 - t5;

        vector1 -= t6;
        vector2 -= t6;
        vector3 -= t6;
        vector4 -= t6;
        vector5 -= t6;
        vector6 -= t6;
    }
    test.stop();
    if (vector1.x() == 0) std::cout << "hmm";

    std::stringstream name;
    name << "vector" << _Dim << " subtraction";
    printStats(name.str(), test.get_times());
}

static void SubtractionTest(void)
{
    SubtractionTest<2>();
    SubtractionTest<3>();
    SubtractionTest<4>();

    std::cout << std::endl;
}



template <int _Dim>
static void LengthTest(void)
{
    static const uint64_t TEST_COUNT = GLOBAL_TEST_COUNT / 2;
    using traits = d3d::garbage::vector_traits<_Dim>;

    efficiency_test test;
    auto v1 = GenerateRandomVector<_Dim>();

    d3d::vector<_Dim> vector1(v1);

    // To prevent optimizations
    float total = 0;

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        total += traits::length(v1);
        total += traits::length_sq(v1);
    }
    test.stop();
    if (total == 0) std::cout << "hmm";

    test.start();
    for (uint64_t i = 0; i < TEST_COUNT; i++)
    {
        total += vector1.length();
        total += vector1.length_sq();
    }
    test.stop();
    if (total == 0) std::cout << "hmm";

    std::stringstream name;
    name << "vector" << _Dim << " length";
    printStats(name.str(), test.get_times());
}

static void LengthTest(void)
{
    LengthTest<2>();
    LengthTest<3>();
    LengthTest<4>();

    std::cout << std::endl;
}



template <int _Dim>
static void ArithmeticTest(void)
{

}

static void ArithmeticTest(void)
{

}



void D3DVectorEfficiencyTests(void)
{
    std::cout << "----- Starting d3d::vector efficiency tests -----" << std::endl << std::endl;
    //      **********----------**********----------**********----------**********----------
    printf("TEST NAME           DirectX Time        d3d::vector Time    Percentage\n");
    printf("-------------------------------------------------------------------------------\n");

    EqualityTest();
    NegationTest();
    DotProductTest();
    CrossProductTest();
    NormalizeTest();
    AdditionTest();
    SubtractionTest();
    LengthTest();

    // Situational Tests

}
