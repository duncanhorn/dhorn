/*
 * Duncan Horn
 *
 * CompressedBaseTests.cpp
 *
 * Tests for the compressed_base.h header
 */
#include "stdafx.h"

#include <dhorn/compressed_base.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
#pragma region Test Types

#pragma region Simple Types

    struct empty
    {
    };

    struct empty_final final
    {
    };

    struct non_empty
    {
        int value = 42;
    };

    struct non_empty_final final
    {
        int value = 42;
    };

#pragma endregion



#pragma region Cannot Construct Types

    struct cant_construct_empty
    {
        cant_construct_empty() = delete;
        cant_construct_empty(const cant_construct_empty&) = delete;
    };

    struct cant_construct_empty_final final
    {
        cant_construct_empty_final() = delete;
        cant_construct_empty_final(const cant_construct_empty_final&) = delete;
    };

    struct cant_construct_non_empty
    {
        cant_construct_non_empty() = delete;
        cant_construct_non_empty(const cant_construct_non_empty&) = delete;

        int value = 42;
    };

    struct cant_construct_non_empty_final final
    {
        cant_construct_non_empty_final() = delete;
        cant_construct_non_empty_final(const cant_construct_non_empty_final&) = delete;

        int value = 42;
    };

#pragma endregion



#pragma region Non-Noexcept Types

    struct throwing_empty
    {
        throwing_empty() {}
        throwing_empty(const throwing_empty&) {}
    };

    struct throwing_empty_final final
    {
        throwing_empty_final() {}
        throwing_empty_final(const throwing_empty_final&) {}
    };

    struct throwing_non_empty
    {
        throwing_non_empty() {}
        throwing_non_empty(const throwing_non_empty&) {}

        int value = 42;
    };

    struct throwing_non_empty_final final
    {
        throwing_non_empty_final() {}
        throwing_non_empty_final(const throwing_non_empty_final&) {}

        int value = 42;
    };

#pragma endregion



#pragma region Conversion Types

    struct from_empty
    {
    };

    struct from_empty_final final
    {
    };

    struct from_non_empty
    {
        int value = 42;
    };

    struct from_non_empty_final final
    {
        int value = 42;
    };



    struct to_empty
    {
        to_empty() = default;
        to_empty(const to_empty&) = default;
        to_empty(to_empty&&) = default;

        to_empty(const from_empty&) noexcept {}
        to_empty(from_empty&&) noexcept {}
    };

    struct to_empty_final final
    {
        to_empty_final() = default;
        to_empty_final(const to_empty_final&) = default;
        to_empty_final(to_empty_final&&) = default;

        to_empty_final(const from_empty_final&) noexcept {}
        to_empty_final(from_empty_final&&) noexcept {}
    };

    struct to_non_empty
    {
        to_non_empty() = default;
        to_non_empty(const to_non_empty&) = default;
        to_non_empty(to_non_empty&&) = default;

        to_non_empty(const from_non_empty& o) noexcept : value(o.value) {}
        to_non_empty(from_non_empty&& o) noexcept : value(o.value) { o.value = 0; }

        int value = 42;
    };

    struct to_non_empty_final final
    {
        to_non_empty_final() = default;
        to_non_empty_final(const to_non_empty_final&) = default;
        to_non_empty_final(to_non_empty_final&&) = default;

        to_non_empty_final(const from_non_empty_final& o) noexcept : value(o.value) {}
        to_non_empty_final(from_non_empty_final&& o) noexcept : value(o.value) { o.value = 0; }

        int value = 42;
    };



    struct throwing_to_empty
    {
        throwing_to_empty() = default;
        throwing_to_empty(const throwing_to_empty&) = default;
        throwing_to_empty(throwing_to_empty&&) = default;

        throwing_to_empty(const from_empty&) {}
        throwing_to_empty(from_empty&&) {}
    };

    struct throwing_to_empty_final final
    {
        throwing_to_empty_final() = default;
        throwing_to_empty_final(const throwing_to_empty_final&) = default;
        throwing_to_empty_final(throwing_to_empty_final&&) = default;

        throwing_to_empty_final(const from_empty_final&) {}
        throwing_to_empty_final(from_empty_final&&) {}
    };

    struct throwing_to_non_empty
    {
        throwing_to_non_empty() = default;
        throwing_to_non_empty(const throwing_to_non_empty&) = default;
        throwing_to_non_empty(throwing_to_non_empty&&) = default;

        throwing_to_non_empty(const from_non_empty& o) : value(o.value) {}
        throwing_to_non_empty(from_non_empty&& o) : value(o.value) { o.value = 0; }

        int value = 42;
    };

    struct throwing_to_non_empty_final final
    {
        throwing_to_non_empty_final() = default;
        throwing_to_non_empty_final(const throwing_to_non_empty_final&) = default;
        throwing_to_non_empty_final(throwing_to_non_empty_final&&) = default;

        throwing_to_non_empty_final(const from_non_empty_final& o) : value(o.value) {}
        throwing_to_non_empty_final(from_non_empty_final&& o) : value(o.value) { o.value = 0; }

        int value = 42;
    };

#pragma endregion

#pragma endregion



    TEST_CLASS(CompressedBaseTests)
    {
#pragma region Constructor Tests

#pragma region Default Construction Tests

        TEST_METHOD(DefaultConstructionTest)
        {
            Assert::IsTrue(std::is_default_constructible_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_default_constructible_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_default_constructible_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_default_constructible_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_default_constructible_v<compressed_base<cant_construct_empty>>);
            Assert::IsFalse(std::is_default_constructible_v<compressed_base<cant_construct_empty_final>>);
            Assert::IsFalse(std::is_default_constructible_v<compressed_base<cant_construct_non_empty>>);
            Assert::IsFalse(std::is_default_constructible_v<compressed_base<cant_construct_non_empty_final>>);

            compressed_base<non_empty> nonEmpty;
            Assert::AreEqual(42, nonEmpty.value().value);

            compressed_base<non_empty_final> nonEmptyFinal;
            Assert::AreEqual(42, nonEmptyFinal.value().value);
        }

        TEST_METHOD(DefaultConstructionNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_default_constructible_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_nothrow_default_constructible_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_nothrow_default_constructible_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_nothrow_default_constructible_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_nothrow_default_constructible_v<compressed_base<throwing_empty>>);
            Assert::IsFalse(std::is_nothrow_default_constructible_v<compressed_base<throwing_empty_final>>);
            Assert::IsFalse(std::is_nothrow_default_constructible_v<compressed_base<throwing_non_empty>>);
            Assert::IsFalse(std::is_nothrow_default_constructible_v<compressed_base<throwing_non_empty_final>>);
        }

#pragma endregion

#pragma region Value Copy Construction Tests

        TEST_METHOD(ValueCopyConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<compressed_base<empty>, const empty&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<empty_final>, const empty_final&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<non_empty>, const non_empty&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<non_empty_final>, const non_empty_final&>);

            Assert::IsFalse(std::is_constructible_v<compressed_base<cant_construct_empty>, const cant_construct_empty&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<cant_construct_empty_final>, const cant_construct_empty_final&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<cant_construct_non_empty>, const cant_construct_non_empty&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<cant_construct_non_empty_final>, const cant_construct_non_empty_final&>);
        }

        TEST_METHOD(ValueCopyConstructionNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<empty>, const empty&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<empty_final>, const empty_final&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<non_empty>, const non_empty&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<non_empty_final>, const non_empty_final&>);

            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_empty>, const throwing_empty&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_empty_final>, const throwing_empty_final&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_non_empty>, const throwing_non_empty&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_non_empty_final>, const throwing_non_empty_final&>);
        }

#pragma endregion

#pragma region Value Move Construction Tests

        TEST_METHOD(ValueMoveConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<compressed_base<empty>, empty&&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<empty_final>, empty_final&&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<non_empty>, non_empty&&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<non_empty_final>, non_empty_final&&>);

            Assert::IsFalse(std::is_constructible_v<compressed_base<cant_construct_empty>, cant_construct_empty&&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<cant_construct_empty_final>, cant_construct_empty_final&&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<cant_construct_non_empty>, cant_construct_non_empty&&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<cant_construct_non_empty_final>, cant_construct_non_empty_final&&>);
        }

        TEST_METHOD(ValueMoveConstructionNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<empty>, empty&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<empty_final>, empty_final&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<non_empty>, non_empty&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<non_empty_final>, non_empty_final&&>);

            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_empty>, throwing_empty&&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_empty_final>, throwing_empty_final&&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_non_empty>, throwing_non_empty&&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_non_empty_final>, throwing_non_empty_final&&>);
        }

#pragma endregion

#pragma region Copy Construction Tests

        TEST_METHOD(CopyConstructionTest)
        {
            Assert::IsTrue(std::is_copy_constructible_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_copy_constructible_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_copy_constructible_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_copy_constructible_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_copy_constructible_v<compressed_base<cant_construct_empty>>);
            Assert::IsFalse(std::is_copy_constructible_v<compressed_base<cant_construct_empty_final>>);
            Assert::IsFalse(std::is_copy_constructible_v<compressed_base<cant_construct_non_empty>>);
            Assert::IsFalse(std::is_copy_constructible_v<compressed_base<cant_construct_non_empty_final>>);
        }

        TEST_METHOD(CopyConstructionNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_nothrow_copy_constructible_v<compressed_base<throwing_empty>>);
            Assert::IsFalse(std::is_nothrow_copy_constructible_v<compressed_base<throwing_empty_final>>);
            Assert::IsFalse(std::is_nothrow_copy_constructible_v<compressed_base<throwing_non_empty>>);
            Assert::IsFalse(std::is_nothrow_copy_constructible_v<compressed_base<throwing_non_empty_final>>);
        }

#pragma endregion

#pragma region Copy Conversion Construction Tests

        TEST_METHOD(CopyConversionConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<compressed_base<to_empty>, const compressed_base<from_empty>&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<to_empty_final>, const compressed_base<from_empty_final>&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<to_non_empty>, const compressed_base<from_non_empty>&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<to_non_empty_final>, const compressed_base<from_non_empty_final>&>);

            Assert::IsFalse(std::is_constructible_v<compressed_base<from_empty>, const compressed_base<to_empty>&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<from_empty_final>, const compressed_base<to_empty_final>&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<from_non_empty>, const compressed_base<to_non_empty>&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<from_non_empty_final>, const compressed_base<to_non_empty_final>&>);
        }

        TEST_METHOD(CopyConversionConstructionNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<to_empty>, const compressed_base<from_empty>&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<to_empty_final>, const compressed_base<from_empty_final>&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<to_non_empty>, const compressed_base<from_non_empty>&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<to_non_empty_final>, const compressed_base<from_non_empty_final>&>);

            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_to_empty>, const compressed_base<from_empty>&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_to_empty_final>, const compressed_base<from_empty_final>&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_to_non_empty>, const compressed_base<from_non_empty>&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_to_non_empty_final>, const compressed_base<from_non_empty_final>&>);
        }

#pragma endregion

#pragma region Move Construction Tests

        TEST_METHOD(MoveConstructionTest)
        {
            Assert::IsTrue(std::is_move_constructible_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_move_constructible_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_move_constructible_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_move_constructible_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_move_constructible_v<compressed_base<cant_construct_empty>>);
            Assert::IsFalse(std::is_move_constructible_v<compressed_base<cant_construct_empty_final>>);
            Assert::IsFalse(std::is_move_constructible_v<compressed_base<cant_construct_non_empty>>);
            Assert::IsFalse(std::is_move_constructible_v<compressed_base<cant_construct_non_empty_final>>);
        }

        TEST_METHOD(MoveConstructionNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_move_constructible_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_nothrow_move_constructible_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_nothrow_move_constructible_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_nothrow_move_constructible_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_nothrow_move_constructible_v<compressed_base<throwing_empty>>);
            Assert::IsFalse(std::is_nothrow_move_constructible_v<compressed_base<throwing_empty_final>>);
            Assert::IsFalse(std::is_nothrow_move_constructible_v<compressed_base<throwing_non_empty>>);
            Assert::IsFalse(std::is_nothrow_move_constructible_v<compressed_base<throwing_non_empty_final>>);
        }

#pragma endregion

#pragma region Move Conversion Construction Tests

        TEST_METHOD(MoveConversionConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<compressed_base<to_empty>, compressed_base<from_empty>&&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<to_empty_final>, compressed_base<from_empty_final>&&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<to_non_empty>, compressed_base<from_non_empty>&&>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<to_non_empty_final>, compressed_base<from_non_empty_final>&&>);

            Assert::IsFalse(std::is_constructible_v<compressed_base<from_empty>, compressed_base<to_empty>&&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<from_empty_final>, compressed_base<to_empty_final>&&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<from_non_empty>, compressed_base<to_non_empty>&&>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<from_non_empty_final>, compressed_base<to_non_empty_final>&&>);
        }

        TEST_METHOD(MoveConversionConstructionNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<to_empty>, compressed_base<from_empty>&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<to_empty_final>, compressed_base<from_empty_final>&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<to_non_empty>, compressed_base<from_non_empty>&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<to_non_empty_final>, compressed_base<from_non_empty_final>&&>);

            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_to_empty>, compressed_base<from_empty>&&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_to_empty_final>, compressed_base<from_empty_final>&&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_to_non_empty>, compressed_base<from_non_empty>&&>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<throwing_to_non_empty_final>, compressed_base<from_non_empty_final>&&>);
        }

#pragma endregion

#pragma endregion
    };
}
