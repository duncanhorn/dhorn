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
        cant_construct_empty& operator=(const cant_construct_empty&) = delete;
    };

    struct cant_construct_empty_final final
    {
        cant_construct_empty_final() = delete;
        cant_construct_empty_final(const cant_construct_empty_final&) = delete;
        cant_construct_empty_final& operator=(const cant_construct_empty_final&) = delete;
    };

    struct cant_construct_non_empty
    {
        cant_construct_non_empty() = delete;
        cant_construct_non_empty(const cant_construct_non_empty&) = delete;
        cant_construct_non_empty& operator=(const cant_construct_non_empty&) = delete;

        int value = 42;
    };

    struct cant_construct_non_empty_final final
    {
        cant_construct_non_empty_final() = delete;
        cant_construct_non_empty_final(const cant_construct_non_empty_final&) = delete;
        cant_construct_non_empty_final& operator=(const cant_construct_non_empty_final&) = delete;

        int value = 42;
    };

#pragma endregion



#pragma region Non-Noexcept Types

    struct throwing_empty
    {
        throwing_empty() {}
        throwing_empty(const throwing_empty&) {}
        throwing_empty& operator=(const throwing_empty&) {}
        throwing_empty& operator=(throwing_empty&&) {}
    };

    struct throwing_empty_final final
    {
        throwing_empty_final() {}
        throwing_empty_final(const throwing_empty_final&) {}
        throwing_empty_final& operator=(const throwing_empty_final&) {}
        throwing_empty_final& operator=(throwing_empty_final&&) {}
    };

    struct throwing_non_empty
    {
        throwing_non_empty() {}
        throwing_non_empty(const throwing_non_empty&) {}
        throwing_non_empty& operator=(const throwing_non_empty&) {}
        throwing_non_empty& operator=(throwing_non_empty&&) {}

        int value = 42;
    };

    struct throwing_non_empty_final final
    {
        throwing_non_empty_final() {}
        throwing_non_empty_final(const throwing_non_empty_final&) {}
        throwing_non_empty_final& operator=(const throwing_non_empty_final&) {}
        throwing_non_empty_final& operator=(throwing_non_empty_final&&) {}

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
        to_empty(const from_empty&) noexcept {}
        to_empty(from_empty&&) noexcept {}

        to_empty& operator=(const from_empty&) noexcept { return *this; }
        to_empty& operator=(from_empty&&) noexcept { return *this; }
    };

    struct to_empty_final final
    {
        to_empty_final() = default;
        to_empty_final(const from_empty_final&) noexcept {}
        to_empty_final(from_empty_final&&) noexcept {}

        to_empty_final& operator=(const from_empty_final&) noexcept { return *this; }
        to_empty_final& operator=(from_empty_final&&) noexcept { return *this; }
    };

    struct to_non_empty
    {
        to_non_empty() = default;
        to_non_empty(const from_non_empty& o) noexcept : value(o.value) {}
        to_non_empty(from_non_empty&& o) noexcept : value(o.value) { o.value = 0; }

        to_non_empty& operator=(const from_non_empty&) noexcept { return *this; }
        to_non_empty& operator=(from_non_empty&&) noexcept { return *this; }

        int value = 42;
    };

    struct to_non_empty_final final
    {
        to_non_empty_final() = default;
        to_non_empty_final(const from_non_empty_final& o) noexcept : value(o.value) {}
        to_non_empty_final(from_non_empty_final&& o) noexcept : value(o.value) { o.value = 0; }

        to_non_empty_final& operator=(const from_non_empty_final&) noexcept { return *this; }
        to_non_empty_final& operator=(from_non_empty_final&&) noexcept { return *this; }

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



#pragma region Constructable Types

    struct construct_empty
    {
        construct_empty() = default;
        construct_empty(char*, int) noexcept {}
        construct_empty(const std::string&) {}
    };

    struct construct_empty_final final
    {
        construct_empty_final() = default;
        construct_empty_final(char*, int) noexcept {}
        construct_empty_final(const std::string&) {}
    };

    struct construct_non_empty
    {
        construct_non_empty() = default;
        construct_non_empty(char* str, int len) noexcept : str(str, len) {}
        construct_non_empty(const std::string& str) : str(str) {}

        std::string str;
    };

    struct construct_non_empty_final final
    {
        construct_non_empty_final() = default;
        construct_non_empty_final(char* str, int len) noexcept : str(str, len) {}
        construct_non_empty_final(const std::string& str) : str(str) {}

        std::string str;
    };

#pragma endregion

#pragma endregion



    TEST_CLASS(CompressedBaseTests)
    {
        TEST_METHOD_INITIALIZE(Setup)
        {
            object_counter::reset();
        }

        TEST_METHOD_CLEANUP(Cleanup)
        {
            Assert::AreEqual(0u, object_counter::instance_count);
        }



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

            compressed_base<object_counter> obj;
            Assert::AreEqual(1u, object_counter::instance_count);
            Assert::AreEqual(1u, object_counter::constructed_count);
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

            object_counter cnt;
            compressed_base<object_counter> obj(cnt);
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::copy_count);
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

            compressed_base<object_counter> obj(object_counter{});
            Assert::AreEqual(1u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::move_count);
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

            compressed_base<object_counter> obj;
            compressed_base<object_counter> objCopy(obj);
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::copy_count);
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

            struct derived : public object_counter {};
            compressed_base<derived> d;
            compressed_base<object_counter> obj(d);
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::copy_count);
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

            compressed_base<object_counter> obj;
            compressed_base<object_counter> objCopy(std::move(obj));
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::move_count);
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

            struct derived : public object_counter {};
            compressed_base<derived> d;
            compressed_base<object_counter> obj(std::move(d));
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::move_count);
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

#pragma region Emplace Construction Tests

        TEST_METHOD(EmplaceConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_empty>, char*, int>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_empty>, std::string>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_empty_final>, char*, int>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_empty_final>, std::string>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_non_empty>, char*, int>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_non_empty>, std::string>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_non_empty_final>, char*, int>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_non_empty_final>, std::string>);

            Assert::IsFalse(std::is_constructible_v<compressed_base<construct_empty>, const char*, int>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<construct_empty_final>, const char*, int>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<construct_non_empty>, const char*, int>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<construct_non_empty_final>, const char*, int>);

            struct test
            {
                test(const object_counter& obj) : obj(obj) {}
                test(object_counter&& obj) : obj(std::move(obj)) {}

                object_counter obj;
            };

            object_counter o;
            compressed_base<test> a(object_counter{});
            compressed_base<test> b(o);
            Assert::AreEqual(3u, object_counter::instance_count);
            Assert::AreEqual(1u, object_counter::copy_count);
            Assert::AreEqual(1u, object_counter::move_count);
        }

        TEST_METHOD(EmplaceConstructionNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<construct_empty>, char*, int>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<construct_empty>, std::string>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<construct_empty_final>, char*, int>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<construct_empty_final>, std::string>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<construct_non_empty>, char*, int>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<construct_non_empty>, std::string>);
            Assert::IsTrue(std::is_nothrow_constructible_v<compressed_base<construct_non_empty_final>, char*, int>);
            Assert::IsFalse(std::is_nothrow_constructible_v<compressed_base<construct_non_empty_final>, std::string>);
        }

#pragma endregion

#pragma region Tuple Construction Tests

        TEST_METHOD(TupleConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_empty>, std::tuple<char*, int>>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_empty>, std::tuple<std::string>>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_empty_final>, std::tuple<char*, int>>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_empty_final>, std::tuple<std::string>>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_non_empty>, std::tuple<char*, int>>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_non_empty>, std::tuple<std::string>>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_non_empty_final>, std::tuple<char*, int>>);
            Assert::IsTrue(std::is_constructible_v<compressed_base<construct_non_empty_final>, std::tuple<std::string>>);

            Assert::IsFalse(std::is_constructible_v<compressed_base<construct_empty>, std::tuple<const char*, int>>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<construct_empty_final>, std::tuple<const char*, int>>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<construct_non_empty>, std::tuple<const char*, int>>);
            Assert::IsFalse(std::is_constructible_v<compressed_base<construct_non_empty_final>, std::tuple<const char*, int>>);

            struct test
            {
                test(const object_counter& obj) : obj(obj) {}
                test(object_counter&& obj) : obj(std::move(obj)) {}

                object_counter obj;
            };

            object_counter o;
            compressed_base<test> a(std::make_tuple(object_counter{})); // move (twice)
            compressed_base<test> b(std::forward_as_tuple(o)); // copy
            Assert::AreEqual(3u, object_counter::instance_count);
            Assert::AreEqual(1u, object_counter::copy_count);
            Assert::AreEqual(2u, object_counter::move_count);
        }

        //TEST_METHOD(TupleConstructionNoexceptTest)
        //{
        //}

#pragma endregion

#pragma endregion



#pragma region Assignment Operator Tests

#pragma region Value Copy Assignment Operator Tests

        TEST_METHOD(ValueCopyAssignmentOperatorTest)
        {
            Assert::IsTrue(std::is_assignable_v<compressed_base<empty>, const empty&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<empty_final>, const empty_final&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<non_empty>, const non_empty&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<non_empty_final>, const non_empty_final&>);

            Assert::IsFalse(std::is_assignable_v<compressed_base<cant_construct_empty>, const cant_construct_empty&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<cant_construct_empty_final>, const cant_construct_empty_final&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<cant_construct_non_empty>, const cant_construct_non_empty&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<cant_construct_non_empty_final>, const cant_construct_non_empty_final&>);

            object_counter cnt;
            compressed_base<object_counter> obj;
            Assert::AreEqual(2u, object_counter::instance_count);

            obj = cnt;
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::copy_count);
        }

        TEST_METHOD(ValueCopyAssignmentOperatorNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<empty>, const empty&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<empty_final>, const empty_final&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<non_empty>, const non_empty&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<non_empty_final>, const non_empty_final&>);

            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_empty>, const throwing_empty&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_empty_final>, const throwing_empty_final&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_non_empty>, const throwing_non_empty&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_non_empty_final>, const throwing_non_empty_final&>);
        }

#pragma endregion

#pragma region Value Move Assignment Operator Tests

        TEST_METHOD(ValueMoveAssignmentOperatorTest)
        {
            Assert::IsTrue(std::is_assignable_v<compressed_base<empty>, empty&&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<empty_final>, empty_final&&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<non_empty>, non_empty&&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<non_empty_final>, non_empty_final&&>);

            Assert::IsFalse(std::is_assignable_v<compressed_base<cant_construct_empty>, cant_construct_empty&&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<cant_construct_empty_final>, cant_construct_empty_final&&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<cant_construct_non_empty>, cant_construct_non_empty&&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<cant_construct_non_empty_final>, cant_construct_non_empty_final&&>);

            compressed_base<object_counter> obj;
            Assert::AreEqual(1u, object_counter::instance_count);

            obj = object_counter{};
            Assert::AreEqual(1u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::move_count);
        }

        TEST_METHOD(ValueMoveAssignmentOperatorNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<empty>, empty&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<empty_final>, empty_final&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<non_empty>, non_empty&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<non_empty_final>, non_empty_final&&>);

            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_empty>, throwing_empty&&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_empty_final>, throwing_empty_final&&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_non_empty>, throwing_non_empty&&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_non_empty_final>, throwing_non_empty_final&&>);
        }

#pragma endregion

#pragma region Copy Assignment Operator Tests

        TEST_METHOD(CopyAssignmentOperatorTest)
        {
            Assert::IsTrue(std::is_copy_assignable_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_copy_assignable_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_copy_assignable_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_copy_assignable_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_copy_assignable_v<compressed_base<cant_construct_empty>>);
            Assert::IsFalse(std::is_copy_assignable_v<compressed_base<cant_construct_empty_final>>);
            Assert::IsFalse(std::is_copy_assignable_v<compressed_base<cant_construct_non_empty>>);
            Assert::IsFalse(std::is_copy_assignable_v<compressed_base<cant_construct_non_empty_final>>);

            compressed_base<object_counter> obj;
            compressed_base<object_counter> objCopy;
            Assert::AreEqual(2u, object_counter::instance_count);

            objCopy = obj;
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::copy_count);
        }

        TEST_METHOD(CopyAssignmentOperatorNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_nothrow_copy_assignable_v<compressed_base<throwing_empty>>);
            Assert::IsFalse(std::is_nothrow_copy_assignable_v<compressed_base<throwing_empty_final>>);
            Assert::IsFalse(std::is_nothrow_copy_assignable_v<compressed_base<throwing_non_empty>>);
            Assert::IsFalse(std::is_nothrow_copy_assignable_v<compressed_base<throwing_non_empty_final>>);
        }

#pragma endregion

#pragma region Copy Conversion Assignment Operator Tests

        TEST_METHOD(CopyConversionAssignmentOperatorTest)
        {
            Assert::IsTrue(std::is_assignable_v<compressed_base<to_empty>, const compressed_base<from_empty>&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<to_empty_final>, const compressed_base<from_empty_final>&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<to_non_empty>, const compressed_base<from_non_empty>&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<to_non_empty_final>, const compressed_base<from_non_empty_final>&>);

            Assert::IsFalse(std::is_assignable_v<compressed_base<from_empty>, const compressed_base<to_empty>&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<from_empty_final>, const compressed_base<to_empty_final>&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<from_non_empty>, const compressed_base<to_non_empty>&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<from_non_empty_final>, const compressed_base<to_non_empty_final>&>);

            struct derived : public object_counter {};
            compressed_base<derived> d;
            compressed_base<object_counter> obj;
            Assert::AreEqual(2u, object_counter::instance_count);

            obj = d;
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::copy_count);
        }

        TEST_METHOD(CopyConversionAssignmentOperatorNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<to_empty>, const compressed_base<from_empty>&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<to_empty_final>, const compressed_base<from_empty_final>&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<to_non_empty>, const compressed_base<from_non_empty>&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<to_non_empty_final>, const compressed_base<from_non_empty_final>&>);

            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_to_empty>, const compressed_base<from_empty>&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_to_empty_final>, const compressed_base<from_empty_final>&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_to_non_empty>, const compressed_base<from_non_empty>&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_to_non_empty_final>, const compressed_base<from_non_empty_final>&>);
        }

#pragma endregion

#pragma region Move Assignment Operator Tests

        TEST_METHOD(MoveAssignmentOperatorTest)
        {
            Assert::IsTrue(std::is_move_assignable_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_move_assignable_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_move_assignable_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_move_assignable_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_move_assignable_v<compressed_base<cant_construct_empty>>);
            Assert::IsFalse(std::is_move_assignable_v<compressed_base<cant_construct_empty_final>>);
            Assert::IsFalse(std::is_move_assignable_v<compressed_base<cant_construct_non_empty>>);
            Assert::IsFalse(std::is_move_assignable_v<compressed_base<cant_construct_non_empty_final>>);

            compressed_base<object_counter> obj;
            compressed_base<object_counter> objCopy;
            Assert::AreEqual(2u, object_counter::instance_count);

            objCopy = std::move(obj);
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::move_count);
        }

        TEST_METHOD(MoveAssignmentOperatorNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_move_assignable_v<compressed_base<empty>>);
            Assert::IsTrue(std::is_nothrow_move_assignable_v<compressed_base<empty_final>>);
            Assert::IsTrue(std::is_nothrow_move_assignable_v<compressed_base<non_empty>>);
            Assert::IsTrue(std::is_nothrow_move_assignable_v<compressed_base<non_empty_final>>);

            Assert::IsFalse(std::is_nothrow_move_assignable_v<compressed_base<throwing_empty>>);
            Assert::IsFalse(std::is_nothrow_move_assignable_v<compressed_base<throwing_empty_final>>);
            Assert::IsFalse(std::is_nothrow_move_assignable_v<compressed_base<throwing_non_empty>>);
            Assert::IsFalse(std::is_nothrow_move_assignable_v<compressed_base<throwing_non_empty_final>>);
        }

#pragma endregion

#pragma region Move Conversion Assignment Operator Tests

        TEST_METHOD(MoveConversionAssignmentOperatorTest)
        {
            Assert::IsTrue(std::is_assignable_v<compressed_base<to_empty>, compressed_base<from_empty>&&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<to_empty_final>, compressed_base<from_empty_final>&&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<to_non_empty>, compressed_base<from_non_empty>&&>);
            Assert::IsTrue(std::is_assignable_v<compressed_base<to_non_empty_final>, compressed_base<from_non_empty_final>&&>);

            Assert::IsFalse(std::is_assignable_v<compressed_base<from_empty>, compressed_base<to_empty>&&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<from_empty_final>, compressed_base<to_empty_final>&&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<from_non_empty>, compressed_base<to_non_empty>&&>);
            Assert::IsFalse(std::is_assignable_v<compressed_base<from_non_empty_final>, compressed_base<to_non_empty_final>&&>);

            struct derived : public object_counter {};
            compressed_base<derived> d;
            compressed_base<object_counter> obj;
            Assert::AreEqual(2u, object_counter::instance_count);

            obj = std::move(d);
            Assert::AreEqual(2u, object_counter::instance_count);
            Assert::AreEqual(2u, object_counter::constructed_count);
            Assert::AreEqual(1u, object_counter::move_count);
        }

        TEST_METHOD(MoveConversionAssignmentOperatorNoexceptTest)
        {
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<to_empty>, compressed_base<from_empty>&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<to_empty_final>, compressed_base<from_empty_final>&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<to_non_empty>, compressed_base<from_non_empty>&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<compressed_base<to_non_empty_final>, compressed_base<from_non_empty_final>&&>);

            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_to_empty>, compressed_base<from_empty>&&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_to_empty_final>, compressed_base<from_empty_final>&&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_to_non_empty>, compressed_base<from_non_empty>&&>);
            Assert::IsFalse(std::is_nothrow_assignable_v<compressed_base<throwing_to_non_empty_final>, compressed_base<from_non_empty_final>&&>);
        }

#pragma endregion

#pragma endregion



        TEST_METHOD(SwapTest)
        {
            compressed_base<non_empty> a;
            compressed_base<non_empty> b;
            a.value().value = 42;
            b.value().value = 8;
            a.swap(b);

            Assert::AreEqual(8, a.value().value);
            Assert::AreEqual(42, b.value().value);
        }
    };
}
