/*
 * D3DVectorTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn::d3d::vector class
 */

#include "stdafx.h"

#include "dhorn/d3d.h"

using namespace dhorn;
using namespace DirectX;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_COUNT          (100)

//namespace dhorn
//{
//    namespace tests
//    {
//        TEST_CLASS(D3DVectorTests)
//        {
//        public:
//
//            template <int _Dim>
//            XMVECTOR MakeRandomVector(void)
//            {
//                float x = (float)(RAND_MAX / 2 - rand());
//                float y = (float)(RAND_MAX / 2 - rand());
//                float z = (_Dim >= 3) ? (float)(RAND_MAX / 2 - rand()) : 0;
//                float w = (_Dim >= 4) ? (float)(RAND_MAX / 2 - rand()) : 0;
//
//                return XMVectorSet(x, y, z, w);
//            }
//
//            void AssertFloatingPointEqual(_In_ float f1, _In_ float f2)
//            {
//                float tolerance = 1e-5f;
//
//                for (float x = fabs(f1); x >= 1; x /= 10)
//                {
//                    tolerance *= 10;
//                }
//
//                Assert::AreEqual(f1, f2, tolerance);
//            }
//
//            void AssertVectorsEqual(_In_ FXMVECTOR v1, _In_ FXMVECTOR v2)
//            {
//                XMFLOAT4 res1, res2;
//                XMStoreFloat4(&res1, v1);
//                XMStoreFloat4(&res2, v2);
//
//                AssertFloatingPointEqual(res1.w, res2.w);
//                AssertFloatingPointEqual(res1.x, res2.x);
//                AssertFloatingPointEqual(res1.y, res2.y);
//                AssertFloatingPointEqual(res1.z, res2.z);
//            }
//
//
//
//            TEST_METHOD(ConstructorTest)
//            {
//                ConstructorTestHelper<2>();
//                ConstructorTestHelper<3>();
//                ConstructorTestHelper<4>();
//            }
//
//            template <int _Dim>
//            void ConstructorTestHelper(void)
//            {
//                using test_type = d3d::vector<_Dim>;
//                using traits = d3d::garbage::vector_traits<_Dim>;
//                using storage_type = typename traits::storage_type;
//
//                // No-arg constructor should yield an all-zero vector
//                test_type empty;
//                Assert::IsTrue(XMVector4Equal(empty, g_XMZero));
//
//                // Load from the storage class
//                storage_type vector;
//                for (int i = 0; i < _Dim; i++)
//                {
//                    (&vector.x)[i] = (float)i;
//                }
//
//                test_type storage(vector);
//                auto test = d3d::garbage::vector_traits<_Dim>::load(vector);
//                Assert::IsTrue(XMVector4Equal(storage, test));
//
//                // Load from an XMVECTOR
//                test = XMVectorSet(1, 2, 3, 4);
//                test_type result(test);
//
//                XMFLOAT4 v1;
//                XMFLOAT4 v2;
//                XMStoreFloat4(&v1, result);
//                XMStoreFloat4(&v2, test);
//
//                Assert::AreEqual(v1.x, (_Dim >= 1) ? v2.x : 0);
//                Assert::AreEqual(v1.y, (_Dim >= 2) ? v2.y : 0);
//                Assert::AreEqual(v1.z, (_Dim >= 3) ? v2.z : 0);
//                Assert::AreEqual(v1.w, (_Dim >= 4) ? v2.w : 0);
//            }
//
//
//
//            TEST_METHOD(AssignmentTest)
//            {
//                AssignmentTestHelper<2>();
//                AssignmentTestHelper<3>();
//                AssignmentTestHelper<4>();
//            }
//
//            template <int _Dim>
//            void AssignmentTestHelper(void)
//            {
//                using test_type = d3d::vector<_Dim>;
//                using traits = d3d::garbage::vector_traits<_Dim>;
//                using storage_type = typename traits::storage_type;
//
//                test_type vector;
//
//                // Test operator= with the storage type
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    storage_type ty;
//                    for (int j = 0; j < _Dim; j++)
//                    {
//                        (&ty.x)[j] = (float)(i + j);
//                    }
//
//                    vector = ty;
//                    Assert::IsTrue(XMVector4Equal(vector, traits::load(ty)));
//                }
//
//                // Test operator= with XMVECTOR
//                for (float i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v = XMVectorSet(i, i + 1, i + 2, i + 3);
//                    vector = v;
//
//                    XMFLOAT4 v1;
//                    XMFLOAT4 v2;
//                    XMStoreFloat4(&v1, vector);
//                    XMStoreFloat4(&v2, v);
//
//                    Assert::AreEqual(v1.x, (_Dim >= 1) ? v2.x : 0);
//                    Assert::AreEqual(v1.y, (_Dim >= 2) ? v2.y : 0);
//                    Assert::AreEqual(v1.z, (_Dim >= 3) ? v2.z : 0);
//                    Assert::AreEqual(v1.w, (_Dim >= 4) ? v2.w : 0);
//                }
//            }
//
//
//
//            TEST_METHOD(EqualityTest)
//            {
//                EqualityTestHelper<2>();
//                EqualityTestHelper<3>();
//                EqualityTestHelper<4>();
//
//                // Test different size vectors
//                auto vector = XMVectorSet(1, 2, 3, 4);
//                d3d::vector2 v2 = vector;
//                d3d::vector3 v3 = vector;
//                d3d::vector4 v4 = vector;
//
//                Assert::IsTrue(v2 != v3);
//                Assert::IsTrue(v3 != v2);
//                Assert::IsFalse(v2 == v3);
//                Assert::IsFalse(v3 == v2);
//
//                Assert::IsTrue(v2 != v4);
//                Assert::IsTrue(v4 != v2);
//                Assert::IsFalse(v2 == v4);
//                Assert::IsFalse(v4 == v2);
//
//                Assert::IsTrue(v3 != v4);
//                Assert::IsTrue(v4 != v3);
//                Assert::IsFalse(v3 == v4);
//                Assert::IsFalse(v4 == v3);
//
//                Assert::IsTrue(v2 != vector);
//                Assert::IsFalse(v2 == vector);
//                Assert::IsTrue(v3 != vector);
//                Assert::IsFalse(v3 == vector);
//                Assert::IsTrue(v4 == vector);
//                Assert::IsFalse(v4 != vector);
//
//                Assert::IsTrue(vector != v2);
//                Assert::IsFalse(vector == v2);
//                Assert::IsTrue(vector != v3);
//                Assert::IsFalse(vector == v3);
//                Assert::IsTrue(vector == v4);
//                Assert::IsFalse(vector != v4);
//            }
//
//            template <int _Dim>
//            void EqualityTestHelper(void)
//            {
//                using test_type = d3d::vector<_Dim>;
//                using traits = d3d::garbage::vector_traits<_Dim>;
//                using storage_type = typename traits::storage_type;
//
//                float x = (float)rand();
//                float y = (float)rand();
//                float z = (float)rand();
//                float w = (float)rand();
//
//                auto vector = XMVectorSet(x, y, z, w);
//                auto expect = XMVectorSet(x, y, (_Dim >= 3) ? z : 0, (_Dim >= 4) ? w : 0);
//
//                // Test for test_type == test_type
//                test_type v1 = vector;
//                test_type v2 = expect;
//                Assert::IsTrue(v1 == v2);
//                Assert::IsFalse(v1 != v2);
//
//                // Test for test_type == storage_type
//                storage_type storage;
//                traits::store(vector, storage);
//                Assert::IsTrue(v1 == storage);
//                Assert::IsFalse(v1 != storage);
//
//                // Test for storage_type == test_type
//                Assert::IsTrue(storage == v1);
//                Assert::IsFalse(storage != v1);
//
//                // Test for test_type == XMVECTOR
//                if (_Dim < 4)
//                {
//                    Assert::IsFalse(v1 == vector);
//                    Assert::IsTrue(v1 != vector);
//                }
//                else
//                {
//                    Assert::IsTrue(v1 == vector);
//                    Assert::IsFalse(v1 != vector);
//                }
//                Assert::IsTrue(v1 == expect);
//                Assert::IsFalse(v1 != expect);
//
//                // Test for XMVECTOR == test_type
//                if (_Dim < 4)
//                {
//                    Assert::IsFalse(vector == v1);
//                    Assert::IsTrue(vector != v1);
//                }
//                else
//                {
//                    Assert::IsTrue(vector == v1);
//                    Assert::IsFalse(vector != v1);
//                }
//                Assert::IsTrue(expect == v1);
//                Assert::IsFalse(expect != v1);
//            }
//
//
//
//            TEST_METHOD(NormalizeTest)
//            {
//                NormalizeTestHelper<2>();
//                NormalizeTestHelper<3>();
//                NormalizeTestHelper<4>();
//            }
//
//            template <int _Dim>
//            void NormalizeTestHelper(void)
//            {
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v = MakeRandomVector<_Dim>();
//                    d3d::vector<_Dim> vector(v);
//
//                    auto res1 = XMVector4Normalize(v);
//                    auto res2 = vector.normalize();
//
//                    AssertVectorsEqual(res1, res2);
//                }
//            }
//
//
//
//            TEST_METHOD(DotProductTest)
//            {
//                DotProductTestHelper<2, 2>();
//                DotProductTestHelper<2, 3>();
//                DotProductTestHelper<2, 4>();
//
//                DotProductTestHelper<3, 2>();
//                DotProductTestHelper<3, 3>();
//                DotProductTestHelper<3, 4>();
//
//                DotProductTestHelper<4, 2>();
//                DotProductTestHelper<4, 3>();
//                DotProductTestHelper<4, 4>();
//
//                DotProductTestHelper<2>();
//                DotProductTestHelper<3>();
//                DotProductTestHelper<4>();
//            }
//
//            template <int _Dim1, int _Dim2>
//            void DotProductTestHelper(void)
//            {
//                using test1_t = d3d::vector<_Dim1>;
//                using test2_t = d3d::vector<_Dim2>;
//
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v1 = MakeRandomVector<_Dim1>();
//                    auto v2 = MakeRandomVector<_Dim2>();
//                    test1_t test1(v1);
//                    test2_t test2(v2);
//
//                    float res1 = XMVectorGetX(XMVector4Dot(v1, v2));
//                    float res2 = test1.dot_product(v2);
//
//                    AssertFloatingPointEqual(res1, res2);
//                }
//            }
//
//            template <int _Dim>
//            void DotProductTestHelper(void)
//            {
//                using test_type = d3d::vector<_Dim>;
//                using traits = d3d::garbage::vector_traits<_Dim>;
//                using storage_type = typename traits::storage_type;
//
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v1 = MakeRandomVector<_Dim>();
//                    auto v2 = MakeRandomVector<_Dim>();
//
//                    test_type vector(v1);
//                    float expect = XMVectorGetX(XMVector4Dot(v1, v2));
//                    AssertFloatingPointEqual(vector.dot_product(v2), expect);
//                }
//            }
//
//
//
//            TEST_METHOD(CrossProductTest)
//            {
//                CrossProductTest2Helper();
//                CrossProductTest3Helper();
//                CrossProductTest4Helper();
//            }
//
//            void CrossProductTest2Helper(void)
//            {
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v1 = MakeRandomVector<2>();
//                    auto v2 = MakeRandomVector<2>();
//
//                    d3d::vector2 vector1(v1);
//                    d3d::vector2 vector2(v2);
//
//                    auto expect = XMVectorGetX(XMVector2Cross(v1, v2));
//
//                    AssertFloatingPointEqual(expect, vector1.cross_product(vector2));
//                    AssertFloatingPointEqual(expect, vector1.cross_product(v2));
//                    AssertFloatingPointEqual(-expect, vector2.cross_product(vector1));
//                    AssertFloatingPointEqual(-expect, vector2.cross_product(v1));
//                }
//            }
//
//            void CrossProductTest3Helper(void)
//            {
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v1 = MakeRandomVector<3>();
//                    auto v2 = MakeRandomVector<3>();
//
//                    d3d::vector3 vector1(v1);
//                    d3d::vector3 vector2(v2);
//
//                    auto expect = XMVector3Cross(v1, v2);
//
//                    Assert::IsTrue(expect == vector1.cross_product(vector2));
//                    Assert::IsTrue(expect == vector1.cross_product(v2));
//                    Assert::IsTrue(expect == -vector2.cross_product(vector1));
//                    Assert::IsTrue(expect == -vector2.cross_product(v1));
//                }
//            }
//
//            void CrossProductTest4Helper(void)
//            {
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v1 = MakeRandomVector<4>();
//                    auto v2 = MakeRandomVector<4>();
//                    auto v3 = MakeRandomVector<4>();
//
//                    d3d::vector4 vector1(v1);
//                    d3d::vector4 vector2(v2);
//                    d3d::vector4 vector3(v3);
//
//                    auto expect = XMVector4Cross(v1, v2, v3);
//
//                    Assert::IsTrue(expect == vector1.cross_product(vector2, vector3));
//                }
//            }
//
//
//
//            TEST_METHOD(NegationTest)
//            {
//                NegationTestHelper<2>();
//                NegationTestHelper<3>();
//                NegationTestHelper<4>();
//            }
//
//            template <int _Dim>
//            void NegationTestHelper(void)
//            {
//                auto v = MakeRandomVector<_Dim>();
//                d3d::vector<_Dim> vector(v);
//
//                AssertVectorsEqual(-vector, XMVectorNegate(v));
//            }
//
//
//
//            TEST_METHOD(AdditionTest)
//            {
//                AdditionTestHelper<2>();
//                AdditionTestHelper<2, 2>();
//                AdditionTestHelper<2, 3>();
//                AdditionTestHelper<2, 4>();
//
//                AdditionTestHelper<3>();
//                AdditionTestHelper<3, 2>();
//                AdditionTestHelper<3, 3>();
//                AdditionTestHelper<3, 4>();
//
//                AdditionTestHelper<4>();
//                AdditionTestHelper<4, 2>();
//                AdditionTestHelper<4, 3>();
//                AdditionTestHelper<4, 4>();
//            }
//
//            template <int _Dim>
//            void AdditionTestHelper(void)
//            {
//                using traits = d3d::garbage::vector_traits<_Dim>;
//                using storage_type = typename traits::storage_type;
//
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v1 = MakeRandomVector<_Dim>();
//                    auto v2 = MakeRandomVector<_Dim>();
//                    auto v3 = MakeRandomVector<_Dim>();
//                    auto expect = XMVectorAdd(v1, v2);
//                    expect = XMVectorAdd(expect, v3);
//
//                    storage_type storage;
//                    traits::store(v3, storage);
//
//                    d3d::vector<_Dim> vector(v1);
//                    vector += v2;
//                    vector += storage;
//
//                    Assert::IsTrue(vector == expect);
//                }
//            }
//
//            template <int _Dim1, int _Dim2>
//            void AdditionTestHelper(void)
//            {
//                using traits1 = d3d::garbage::vector_traits<_Dim1>;
//                using traits2 = d3d::garbage::vector_traits<_Dim2>;
//                using storage_type1 = typename traits1::storage_type;
//                using storage_type2 = typename traits2::storage_type;
//
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v1 = MakeRandomVector<_Dim1>();
//                    auto v2 = MakeRandomVector<_Dim2>();
//
//                    storage_type1 storage1;
//                    traits1::store(v1, storage1);
//                    storage_type2 storage2;
//                    traits2::store(v2, storage2);
//
//                    auto expect = XMVectorAdd(v1, v2);
//
//                    d3d::vector<_Dim1> vector1(v1);
//                    d3d::vector<_Dim2> vector2(v2);
//
//                    // Test for vector + vector
//                    Assert::IsTrue((vector1 + vector2) == expect);
//
//                    // Test for vector + XMVECTOR (and vice versa)
//                    Assert::IsTrue((vector1 + v2) == expect);
//                    Assert::IsTrue((v1 + vector2) == expect);
//
//                    // Test for vector + XMFLOAT*
//                    Assert::IsTrue((vector1 + storage2) == expect);
//                    Assert::IsTrue((storage1 + vector2) == expect);
//                }
//            }
//
//
//
//            TEST_METHOD(SubtractionTest)
//            {
//                SubtractionTestHelper<2>();
//                SubtractionTestHelper<2, 2>();
//                SubtractionTestHelper<2, 3>();
//                SubtractionTestHelper<2, 4>();
//
//                SubtractionTestHelper<3>();
//                SubtractionTestHelper<3, 2>();
//                SubtractionTestHelper<3, 3>();
//                SubtractionTestHelper<3, 4>();
//
//                SubtractionTestHelper<4>();
//                SubtractionTestHelper<4, 2>();
//                SubtractionTestHelper<4, 3>();
//                SubtractionTestHelper<4, 4>();
//            }
//
//            template <int _Dim>
//            void SubtractionTestHelper(void)
//            {
//                using traits = d3d::garbage::vector_traits<_Dim>;
//                using storage_type = typename traits::storage_type;
//
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v1 = MakeRandomVector<_Dim>();
//                    auto v2 = MakeRandomVector<_Dim>();
//                    auto v3 = MakeRandomVector<_Dim>();
//                    auto expect = XMVectorSubtract(v1, v2);
//                    expect = XMVectorSubtract(expect, v3);
//
//                    storage_type storage;
//                    traits::store(v3, storage);
//
//                    d3d::vector<_Dim> vector(v1);
//                    vector -= v2;
//                    vector -= storage;
//
//                    Assert::IsTrue(vector == expect);
//                }
//            }
//
//            template <int _Dim1, int _Dim2>
//            void SubtractionTestHelper(void)
//            {
//                using traits1 = d3d::garbage::vector_traits<_Dim1>;
//                using traits2 = d3d::garbage::vector_traits<_Dim2>;
//                using storage_type1 = typename traits1::storage_type;
//                using storage_type2 = typename traits2::storage_type;
//
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v1 = MakeRandomVector<_Dim1>();
//                    auto v2 = MakeRandomVector<_Dim2>();
//
//                    storage_type1 storage1;
//                    traits1::store(v1, storage1);
//                    storage_type2 storage2;
//                    traits2::store(v2, storage2);
//
//                    auto expect = XMVectorSubtract(v1, v2);
//
//                    d3d::vector<_Dim1> vector1(v1);
//                    d3d::vector<_Dim2> vector2(v2);
//
//                    // Test for vector + vector
//                    Assert::IsTrue((vector1 - vector2) == expect);
//
//                    // Test for vector + XMVECTOR (and vice versa)
//                    Assert::IsTrue((vector1 - v2) == expect);
//                    Assert::IsTrue((v1 - vector2) == expect);
//
//                    // Test for vector + XMFLOAT*
//                    Assert::IsTrue((vector1 - storage2) == expect);
//                    Assert::IsTrue((storage1 - vector2) == expect);
//                }
//            }
//
//
//
//            TEST_METHOD(ScalarMultiplicationTest)
//            {
//                ScalarMultiplicationTestHelper<2>();
//                ScalarMultiplicationTestHelper<3>();
//                ScalarMultiplicationTestHelper<4>();
//            }
//
//            template <int _Dim>
//            void ScalarMultiplicationTestHelper(void)
//            {
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    float scalar = (float)(RAND_MAX / 2 - rand());
//
//                    auto v = MakeRandomVector<_Dim>();
//                    auto scalar_vector = XMVectorSet(scalar, scalar, scalar, scalar);
//                    auto expect = XMVectorMultiply(v, scalar_vector);
//
//                    d3d::vector<_Dim> vector(v);
//                    AssertVectorsEqual(vector * scalar, expect);
//
//                    AssertVectorsEqual(scalar * vector, expect);
//
//                    vector *= scalar;
//                    AssertVectorsEqual(vector, expect);
//                }
//            }
//
//
//
//            TEST_METHOD(ScalarDivisionTest)
//            {
//                ScalarDivisionTestHelper<2>();
//                ScalarDivisionTestHelper<3>();
//                ScalarDivisionTestHelper<4>();
//            }
//
//            template <int _Dim>
//            void ScalarDivisionTestHelper(void)
//            {
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    float scalar = (float)(RAND_MAX / 2 - rand());
//
//                    auto v = MakeRandomVector<_Dim>();
//                    auto scalar_vector = XMVectorSet(scalar, scalar, scalar, scalar);
//                    auto expect = XMVectorDivide(v, scalar_vector);
//
//                    d3d::vector<_Dim> vector(v);
//                    AssertVectorsEqual(vector / scalar, expect);
//
//                    vector /= scalar;
//                    AssertVectorsEqual(vector, expect);
//                }
//            }
//
//
//
//            TEST_METHOD(LengthTest)
//            {
//                LengthTestHelper<2>();
//                LengthTestHelper<3>();
//                LengthTestHelper<4>();
//            }
//
//            template <int _Dim>
//            void LengthTestHelper(void)
//            {
//                using test_type = d3d::vector<_Dim>;
//
//                for (int i = 0; i < TEST_COUNT; i++)
//                {
//                    auto v = MakeRandomVector<_Dim>();
//                    test_type vector(v);
//
//                    float expect = XMVectorGetX(XMVector4Length(v));
//                    float expect_sq = XMVectorGetX(XMVector4LengthSq(v));
//
//                    AssertFloatingPointEqual(vector.length(), expect);
//                    AssertFloatingPointEqual(vector.length_sq(), expect_sq);
//                }
//
//                test_type v(g_XMZero);
//                Assert::AreEqual(v.length(), 0.0f);
//                Assert::AreEqual(v.length_sq(), 0.0f);
//
//                v = test_type(g_XMIdentityR0);
//                Assert::AreEqual(v.length(), 1.0f);
//                Assert::AreEqual(v.length_sq(), 1.0f);
//
//                v = test_type(g_XMIdentityR1);
//                Assert::AreEqual(v.length(), 1.0f);
//                Assert::AreEqual(v.length_sq(), 1.0f);
//            }
//        };
//    }
//}
