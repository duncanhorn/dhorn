/*
 * D3DVectorTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn::d3d::vector class
 */

#include "stdafx.h"
#include "CppUnitTest.h"
#include <DirectXMath.h>

#include "dhorn/d3d.h"

using namespace dhorn;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(D3DVectorTests)
        {
        public:

            template <int _Dim>
            DirectX::XMVECTOR MakeRandomVector(void)
            {
                float x = (float)(RAND_MAX / 2 - rand());
                float y = (float)(RAND_MAX / 2 - rand());
                float z = (float)(RAND_MAX / 2 - rand());
                float w = (float)(RAND_MAX / 2 - rand());

                return DirectX::XMVectorSet(x, y, (_Dim >= 3) ? z : 0, (_Dim >= 4) ? w : 0);
            }

            void AssertFloatingPointEqual(_In_ float f1, _In_ float f2)
            {
                float tolerance = 1e-5f;

                for (float x = fabs(f1); x >= 1; x /= 10)
                {
                    tolerance *= 10;
                }

                Assert::AreEqual(f1, f2, tolerance);
            }

            void AssertVectorsEqual(_In_ DirectX::FXMVECTOR v1, _In_ DirectX::FXMVECTOR v2)
            {
                DirectX::XMFLOAT4 res1, res2;
                DirectX::XMStoreFloat4(&res1, v1);
                DirectX::XMStoreFloat4(&res2, v2);

                Assert::AreEqual(res1.w, res2.w, 1e-6f);
                Assert::AreEqual(res1.x, res2.x, 1e-6f);
                Assert::AreEqual(res1.y, res2.y, 1e-6f);
                Assert::AreEqual(res1.z, res2.z, 1e-6f);
            }



            TEST_METHOD(ConstructorTest)
            {
                ConstructorTestHelper<2>();
                ConstructorTestHelper<3>();
                ConstructorTestHelper<4>();
            }

            template <int _Dim>
            void ConstructorTestHelper(void)
            {
                using test_type = d3d::vector<_Dim>;
                using traits = d3d::garbage::vector_traits<_Dim>;
                using storage_type = typename traits::storage_type;

                // No-arg constructor should yield an all-zero vector
                test_type empty;
                Assert::IsTrue(DirectX::XMVector4Equal(empty, DirectX::g_XMZero));

                // Load from the storage class
                storage_type vector;
                for (int i = 0; i < _Dim; i++)
                {
                    (&vector.x)[i] = (float)i;
                }

                test_type storage(vector);
                auto test = d3d::garbage::vector_traits<_Dim>::load(vector);
                Assert::IsTrue(DirectX::XMVector4Equal(storage, test));

                // Load from an XMVECTOR
                test = DirectX::XMVectorSet(1, 2, 3, 4);
                test_type result(test);

                DirectX::XMFLOAT4 v1;
                DirectX::XMFLOAT4 v2;
                DirectX::XMStoreFloat4(&v1, result);
                DirectX::XMStoreFloat4(&v2, test);

                Assert::AreEqual(v1.x, (_Dim >= 1) ? v2.x : 0);
                Assert::AreEqual(v1.y, (_Dim >= 2) ? v2.y : 0);
                Assert::AreEqual(v1.z, (_Dim >= 3) ? v2.z : 0);
                Assert::AreEqual(v1.w, (_Dim >= 4) ? v2.w : 0);
            }



            TEST_METHOD(AssignmentTest)
            {
                AssignmentTestHelper<2>();
                AssignmentTestHelper<3>();
                AssignmentTestHelper<4>();
            }

            template <int _Dim>
            void AssignmentTestHelper(void)
            {
                using test_type = d3d::vector<_Dim>;
                using traits = d3d::garbage::vector_traits<_Dim>;
                using storage_type = typename traits::storage_type;

                test_type vector;

                // Test operator= with the storage type
                for (int i = 0; i < 100; i++)
                {
                    storage_type ty;
                    for (int j = 0; j < _Dim; j++)
                    {
                        (&ty.x)[j] = (float)(i + j);
                    }

                    vector = ty;
                    Assert::IsTrue(DirectX::XMVector4Equal(vector, traits::load(ty)));
                }

                // Test operator= with XMVECTOR
                for (float i = 0; i < 100; i++)
                {
                    auto v = DirectX::XMVectorSet(i, i + 1, i + 2, i + 3);
                    vector = v;

                    DirectX::XMFLOAT4 v1;
                    DirectX::XMFLOAT4 v2;
                    DirectX::XMStoreFloat4(&v1, vector);
                    DirectX::XMStoreFloat4(&v2, v);

                    Assert::AreEqual(v1.x, (_Dim >= 1) ? v2.x : 0);
                    Assert::AreEqual(v1.y, (_Dim >= 2) ? v2.y : 0);
                    Assert::AreEqual(v1.z, (_Dim >= 3) ? v2.z : 0);
                    Assert::AreEqual(v1.w, (_Dim >= 4) ? v2.w : 0);
                }
            }



            TEST_METHOD(EqualityTest)
            {
                EqualityTestHelper<2>();
                EqualityTestHelper<3>();
                EqualityTestHelper<4>();

                // Test different size vectors
                auto vector = DirectX::XMVectorSet(1, 2, 3, 4);
                d3d::vector2 v2 = vector;
                d3d::vector3 v3 = vector;
                d3d::vector4 v4 = vector;

                Assert::IsTrue(v2 != v3);
                Assert::IsTrue(v3 != v2);
                Assert::IsFalse(v2 == v3);
                Assert::IsFalse(v3 == v2);

                Assert::IsTrue(v2 != v4);
                Assert::IsTrue(v4 != v2);
                Assert::IsFalse(v2 == v4);
                Assert::IsFalse(v4 == v2);

                Assert::IsTrue(v3 != v4);
                Assert::IsTrue(v4 != v3);
                Assert::IsFalse(v3 == v4);
                Assert::IsFalse(v4 == v3);
            }

            template <int _Dim>
            void EqualityTestHelper(void)
            {
                using test_type = d3d::vector<_Dim>;
                using traits = d3d::garbage::vector_traits<_Dim>;
                using storage_type = typename traits::storage_type;

                auto vector = DirectX::XMVectorSet(1, 2, 3, 4);
                auto expect = DirectX::XMVectorSet(1, 2, (_Dim >= 3) ? 3 : 0, (_Dim >= 4) ? 4 : 0);

                // Test for test_type == test_type
                test_type v1 = vector;
                test_type v2 = expect;
                Assert::IsTrue(v1 == v2);
                Assert::IsFalse(v1 != v2);

                // Test for test_type == storage_type
                storage_type storage;
                traits::store(vector, storage);
                Assert::IsTrue(v1 == storage);
                Assert::IsFalse(v1 != storage);

                // Test for storage_type == test_type
                Assert::IsTrue(storage == v1);
                Assert::IsFalse(storage != v1);

                // Test for test_type == XMVECTOR
                Assert::IsTrue(v1 == expect);
                Assert::IsFalse(v1 != expect);

                // Test for XMVECTOR == test_type
                Assert::IsTrue(expect == v1);
                Assert::IsFalse(expect != v1);
            }



            TEST_METHOD(NormalizeTest)
            {
                NormalizeTestHelper<2>();
                NormalizeTestHelper<3>();
                NormalizeTestHelper<4>();
            }

            template <int _Dim>
            void NormalizeTestHelper(void)
            {
                for (int i = 0; i < 100; i++)
                {
                    auto v = MakeRandomVector<_Dim>();
                    d3d::vector<_Dim> vector(v);

                    auto res1 = DirectX::XMVector4Normalize(v);
                    auto res2 = vector.normalize();

                    AssertVectorsEqual(res1, res2);
                }
            }



            TEST_METHOD(DotProductTest)
            {
                DotProductTestHelper<2, 2>();
                DotProductTestHelper<2, 3>();
                DotProductTestHelper<2, 4>();

                DotProductTestHelper<3, 2>();
                DotProductTestHelper<3, 3>();
                DotProductTestHelper<3, 4>();

                DotProductTestHelper<4, 2>();
                DotProductTestHelper<4, 3>();
                DotProductTestHelper<4, 4>();

                DotProductTestHelper<2>();
                DotProductTestHelper<3>();
                DotProductTestHelper<4>();
            }

            template <int _Dim1, int _Dim2>
            void DotProductTestHelper(void)
            {
                using test1_t = d3d::vector<_Dim1>;
                using test2_t = d3d::vector<_Dim2>;

                for (int i = 0; i < 100; i++)
                {
                    auto v1 = MakeRandomVector<_Dim1>();
                    auto v2 = MakeRandomVector<_Dim2>();
                    test1_t test1(v1);
                    test2_t test2(v2);

                    float res1 = DirectX::XMVectorGetX(DirectX::XMVector4Dot(v1, v2));
                    float res2 = test1.dot_product(v2);

                    AssertFloatingPointEqual(res1, res2);
                }
            }

            template <int _Dim>
            void DotProductTestHelper(void)
            {
                using test_type = d3d::vector<_Dim>;
                using traits = d3d::garbage::vector_traits<_Dim>;
                using storage_type = typename traits::storage_type;

                for (int i = 0; i < 100; i++)
                {
                    auto v1 = MakeRandomVector<_Dim>();
                    auto v2 = MakeRandomVector<_Dim>();

                    test_type vector(v1);
                    float expect = DirectX::XMVectorGetX(DirectX::XMVector4Dot(v1, v2));
                    AssertFloatingPointEqual(vector.dot_product(v2), expect);
                }
            }



            TEST_METHOD(CrossProductTest)
            {
                CrossProductTest2Helper();
                CrossProductTest3Helper();
                CrossProductTest4Helper();
            }

            void CrossProductTest2Helper(void)
            {
                for (int i = 0; i < 100; i++)
                {
                    auto v1 = MakeRandomVector<2>();
                    auto v2 = MakeRandomVector<2>();

                    d3d::vector2 vector1(v1);
                    d3d::vector2 vector2(v2);

                    auto expect = DirectX::XMVectorGetX(DirectX::XMVector2Cross(v1, v2));

                    AssertFloatingPointEqual(expect, vector1.cross_product(vector2));
                    AssertFloatingPointEqual(expect, vector1.cross_product(v2));
                    AssertFloatingPointEqual(-expect, vector2.cross_product(vector1));
                    AssertFloatingPointEqual(-expect, vector2.cross_product(v1));
                }
            }

            void CrossProductTest3Helper(void)
            {
                for (int i = 0; i < 100; i++)
                {
                    auto v1 = MakeRandomVector<3>();
                    auto v2 = MakeRandomVector<3>();

                    d3d::vector3 vector1(v1);
                    d3d::vector3 vector2(v2);

                    auto expect = DirectX::XMVector3Cross(v1, v2);

                    Assert::IsTrue(expect == vector1.cross_product(vector2));
                    Assert::IsTrue(expect == vector1.cross_product(v2));
                    Assert::IsTrue(expect == -vector2.cross_product(vector1));
                    Assert::IsTrue(expect == -vector2.cross_product(v1));
                }
            }

            void CrossProductTest4Helper(void)
            {
                for (int i = 0; i < 100; i++)
                {
                    auto v1 = MakeRandomVector<4>();
                    auto v2 = MakeRandomVector<4>();
                    auto v3 = MakeRandomVector<4>();

                    d3d::vector4 vector1(v1);
                    d3d::vector4 vector2(v2);
                    d3d::vector4 vector3(v3);

                    auto expect = DirectX::XMVector4Cross(v1, v2, v3);

                    Assert::IsTrue(expect == vector1.cross_product(vector2, vector3));
                }
            }



            TEST_METHOD(NegationTest)
            {

            }

            template <int _Dim>
            void NegationTestHelper(void)
            {

            }
        };
    }
}
