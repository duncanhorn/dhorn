/*
 * Duncan Horn
 *
 * UUIDTests.cpp
 *
 * Tests for dhorn::uuid structure
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/uuid.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


static const uint8_t byte_array[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                      0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
static const uint32_t int_array[] = { 0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF };

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(UUIDTests)
        {
            TEST_METHOD(DefaultConstructorTest)
            {
                dhorn::uuid id;

                // Make sure all fields are zero
                for (size_t i = 0; i < 16; i++)
                {
                    Assert::IsTrue(id.data[i] == 0);
                }
                for (size_t i = 0; i < 4; i++)
                {
                    Assert::IsTrue(id.data32[i] == 0);
                }
            }

            TEST_METHOD(ByteArrayConstructorAssignmentTest)
            {
                // Cannot construct with non-16-size uint8_t array
                uint8_t invalid_array[] = { 1, 2, 3, 4 };
                (void)invalid_array;
                Assert::IsFalse(std::is_constructible<dhorn::uuid, decltype(invalid_array)>::value);

                // Should be able to contruct with uint8_t[16]
                dhorn::uuid id(byte_array);

                for (size_t i = 0; i < 16; i++)
                {
                    Assert::IsTrue(id.data[i] == byte_array[i]);
                }

                // Should be able to use initializer list syntax
                id = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

                for (size_t i = 0; i < 16; i++)
                {
                    Assert::IsTrue(id.data[i] == byte_array[i]);
                }
            }

            TEST_METHOD(IntegerArrayConstructorAssignmentTest)
            {
                // Cannot construct with non-16-size uint8_t array
                uint32_t invalid_array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
                (void)invalid_array;
                Assert::IsFalse(std::is_constructible<dhorn::uuid, decltype(invalid_array)>::value);

                // Should be able to contruct with uint8_t[16]
                dhorn::uuid id(int_array);

                for (size_t i = 0; i < 4; i++)
                {
                    Assert::IsTrue(id.data32[i] == int_array[i]);
                }

                // Should be able to use initializer list syntax
                id = { 0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF };

                for (size_t i = 0; i < 4; i++)
                {
                    Assert::IsTrue(id.data32[i] == int_array[i]);
                }
            }

            TEST_METHOD(GuidConstructorAssignmentTest)
            {
                // Assuming little endian, so the non-8-bit integers' bytes are reversed
                dhorn::uuid id = { 0x33221100, 0x5544, 0x7766,
                    { 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF } };

                for (size_t i = 0; i < 16; i++)
                {
                    Assert::IsTrue(id.data[i] == byte_array[i]);
                }

                // Construct with GUID
                static const GUID guid = { 0x40ea2f58, 0x4589, 0x4ae6,
                    { 0x8f, 0xfa, 0xf1, 0xbc, 0x36, 0xcd, 0x98, 0x4c } };
                id = guid;

                Assert::IsTrue(guid.Data1 == id.Data1);
                Assert::IsTrue(guid.Data2 == id.Data2);
                Assert::IsTrue(guid.Data3 == id.Data3);
                for (size_t i = 0; i < 8; i++)
                {
                    Assert::IsTrue(guid.Data4[i] == id.Data4[i]);
                }
            }

            TEST_METHOD(CopyConstructorTest)
            {
                dhorn::uuid id(byte_array);
                dhorn::uuid id2 = id;

                for (size_t i = 0; i < 16; i++)
                {
                    Assert::IsTrue(id2.data[i] == byte_array[i]);
                }
            }

            TEST_METHOD(ComparisonTest)
            {
                dhorn::uuid x = { 0xa160dd94, 0x3c9d, 0x49cf,
                    { 0xaf, 0x65, 0xf7, 0xa6, 0x59, 0x23, 0x25, 0xfd } };
                dhorn::uuid y = { 0xd51675dc, 0xdebc, 0x49bc,
                    { 0x91, 0xa9, 0xf4, 0xa2, 0x85, 0x95, 0x7e, 0x6e } };

                Assert::IsTrue(x == x);
                Assert::IsTrue(x != y);

                Assert::IsFalse(x < x);
                Assert::IsTrue(x < y);
                Assert::IsTrue(x <= x);
                Assert::IsTrue(x <= y);

                Assert::IsFalse(x > x);
                Assert::IsTrue(y > x);
                Assert::IsTrue(x >= x);
                Assert::IsTrue(y >= x);
            }

            TEST_METHOD(HashTest)
            {
                dhorn::uuid x = { 0xa160dd94, 0x3c9d, 0x49cf,
                { 0xaf, 0x65, 0xf7, 0xa6, 0x59, 0x23, 0x25, 0xfd } };
                dhorn::uuid y = { 0xd51675dc, 0xdebc, 0x49bc,
                { 0x91, 0xa9, 0xf4, 0xa2, 0x85, 0x95, 0x7e, 0x6e } };

                std::hash<uuid> hasher;
                Assert::IsTrue(hasher(x) != hasher(y));
            }

#ifdef _DEBUG
            TEST_METHOD(DestructorTest)
            {
                dhorn::uuid id;

                // Should be zero-initialized. Once we run the destructor, it should be filled with
                // the patern 0xCC...
                id.~uuid();

                for (size_t i = 0; i < 16; i++)
                {
                    Assert::IsTrue(id.data[i] == 0xCC);
                }
                for (size_t i = 0; i < 4; i++)
                {
                    Assert::IsTrue(id.data32[i] == 0xCCCCCCCC);
                }
            }
#endif
        };
    }
}
