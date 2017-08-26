/*
 * Duncan Horn
 *
 * UniqueTests.cpp
 *
 * Tests for the dhorn unique class
 */
#include "stdafx.h"

#include <dhorn/unique.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(UniqueTests)
    {
#pragma region Test Types

        struct base
        {
        };

        struct derived :
            public base
        {
        };

#pragma endregion



#pragma region Test Traits

        // Traits type that is guaranteed to be empty. Useful for size tests...
        template <typename Ty>
        struct empty_traits
        {
            // Decay arrays to pointers
            using value_type = std::decay_t<Ty>;

            static constexpr bool is_valid(const value_type&) { return true; }
            static constexpr value_type default_value() noexcept(std::is_nothrow_default_constructible_v<value_type>)
            {
                return value_type{};
            }

            constexpr void operator()(const value_type&) {}
        };

        template <typename Ty>
        using unique_empty = unique<Ty, empty_traits<Ty>>;

        class empty_int_traits : public empty_traits<int> {};
        using unique_empty_int = unique<int, empty_int_traits>;



        template <typename Ty>
        struct throwing_traits
        {
            throwing_traits() {}
            throwing_traits(const throwing_traits&) {}
            throwing_traits(throwing_traits&&) {}

            // Decay arrays to pointers
            using value_type = std::decay_t<Ty>;

            static constexpr bool is_valid(const value_type&) { return true; }
            static constexpr value_type default_value() noexcept(std::is_nothrow_default_constructible_v<value_type>)
            {
                return value_type{};
            }

            constexpr void operator()(const value_type&) {}
        };



        template <typename Ty>
        struct non_constructible_traits
        {
            non_constructible_traits() = delete;
            non_constructible_traits(const non_constructible_traits&) = delete;
            non_constructible_traits(non_constructible_traits&&) = delete;

            // Decay arrays to pointers
            using value_type = std::decay_t<Ty>;

            static constexpr bool is_valid(const value_type&) { return true; }
            static constexpr value_type default_value() noexcept(std::is_nothrow_default_constructible_v<value_type>)
            {
                return value_type{};
            }

            constexpr void operator()(const value_type&) {}
        };



        template <typename Ty>
        struct non_empty_traits
        {
            // Decay arrays to pointers
            using value_type = std::decay_t<Ty>;

            static constexpr bool is_valid(const value_type&) { return true; }
            static constexpr value_type default_value() noexcept(std::is_nothrow_default_constructible_v<value_type>)
            {
                return value_type{};
            }

            constexpr void operator()(const value_type&) {}

            // Hold a `Ty` as the member. This makes our math easier (multiply by two) and eliminates our need to worry
            // about padding
            value_type value;
        };

        template <typename Ty>
        using unique_non_empty = unique<Ty, non_empty_traits<Ty>>;



        template <typename CharT>
        struct unique_basic_string_traits
        {
            using value_type = std::basic_string<CharT>;

            static constexpr bool valid(const value_type& value) noexcept
            {
                return !value.empty();
            }

            static constexpr value_type default_value() noexcept(std::is_nothrow_default_constructible_v<value_type>)
            {
                return value_type{};
            }

            void operator()(value_type&) noexcept
            {
                // Strings delete themselves
            }
        };

        template <typename CharT>
        using unique_basic_string = unique<std::basic_string<CharT>, unique_basic_string_traits<CharT>>;



        template <typename Ty, std::size_t Size, Ty Value = Ty{}>
        struct clear_buffer_unique_traits
        {
            using value_type = Ty*;

            static constexpr bool valid(value_type)
            {
                // Should never be given a null value, so always valid
                return true;
            }

            [[noreturn]] static constexpr value_type default_value()
            {
                Assert::Fail("Did not expect default construction");
            }

            void operator()(value_type value)
            {
                for (std::size_t i = 0; i < Size; ++i)
                {
                    value[i] = Value;
                }
            }
        };

        template <typename Ty, std::size_t Size, Ty Value = Ty{}>
        using unique_buffer = unique<Ty, clear_buffer_unique_traits<Ty, Size, Value>>;



        class binary_value
        {
        public:

            constexpr bool is_valid() const noexcept
            {
                return this->_valid;
            }

            static constexpr binary_value valid() noexcept
            {
                return binary_value(true);
            }

            static constexpr binary_value invalid() noexcept
            {
                return binary_value(false);
            }

        private:

            constexpr binary_value(bool valid) :
                _valid(valid)
            {
            }

            bool _valid;
        };

        struct binary_value_traits
        {
            using value_type = binary_value;

            static constexpr bool valid(value_type value)
            {
                return value.is_valid();
            }

            static constexpr value_type default_value()
            {
                return binary_value::invalid();
            }

            void operator()(value_type /*value*/)
            {
            }
        };

        using unique_binary_value = unique<binary_value, binary_value_traits>;

#pragma endregion



#pragma region Size Tests

        TEST_METHOD(EmptyTraitsSizeTest)
        {
            Assert::AreEqual(sizeof(int), sizeof(unique_empty<int>));
            Assert::AreEqual(sizeof(std::string), sizeof(unique_empty<std::string>));

            Assert::AreEqual(sizeof(int*), sizeof(unique_empty<int*>));
            Assert::AreEqual(sizeof(int*), sizeof(unique_empty<int[]>));
        }

        TEST_METHOD(NonEmptyTraitsSizeTest)
        {
            Assert::AreEqual(sizeof(int) * 2, sizeof(unique_non_empty<int>));
            Assert::AreEqual(sizeof(std::string) * 2, sizeof(unique_non_empty<std::string>));

            Assert::AreEqual(sizeof(int*) * 2, sizeof(unique_non_empty<int*>));
            Assert::AreEqual(sizeof(int*) * 2, sizeof(unique_non_empty<int[]>));
        }

        TEST_METHOD(UniquePtrSizeTest)
        {
            Assert::AreEqual(sizeof(void*), sizeof(unique_ptr<int>));
            Assert::AreEqual(sizeof(void*), sizeof(unique_ptr<std::string>));

            Assert::AreEqual(sizeof(void*), sizeof(unique_ptr<int[]>));
            Assert::AreEqual(sizeof(void*), sizeof(unique_ptr<std::string[]>));
        }

#pragma endregion



#pragma region Traits Tests

        template <typename Ty>
        struct has_value_type_traits
        {
            using value_type = std::unique_ptr<Ty>;

            static constexpr bool is_valid(const value_type& value) { return value; }
            static constexpr value_type default_value() { return {}; }
            constexpr void operator()(const value_type&) {}
        };

        template <typename Ty>
        struct no_value_type_traits
        {
            static constexpr bool is_valid(const Ty&) { return true; }
            static constexpr Ty default_value() { return {}; }
            constexpr void operator()(const Ty&) {}
        };

        TEST_METHOD(ValueTypeAliasTest)
        {
            Assert::IsTrue(std::is_same_v<int, unique<int, no_value_type_traits<int>>::value_type>);
            Assert::IsTrue(std::is_same_v<std::unique_ptr<int>, unique<int, has_value_type_traits<int>>::value_type>);

            Assert::IsTrue(std::is_same_v<int*, unique<int*>::value_type>);
            Assert::IsTrue(std::is_same_v<int*, unique<int(*)[]>::value_type>);

            Assert::IsTrue(std::is_same_v<int*, unique_ptr<int>::value_type>);
            Assert::IsTrue(std::is_same_v<int*, unique_ptr<int[]>::value_type>);
        }

#pragma endregion



#pragma region Constructor/Destructor Tests

        TEST_METHOD(DefaultConstructionTest)
        {
            Assert::IsTrue(std::is_default_constructible_v<unique_empty<int>>);
            Assert::IsTrue(std::is_default_constructible_v<unique_ptr<int>>);
            Assert::IsTrue(std::is_default_constructible_v<unique_ptr<int[]>>);

            Assert::IsTrue(std::is_default_constructible_v<unique_binary_value>);

            // TODO: Comment on why the traits is SFINAE'd
            Assert::IsFalse(std::is_default_constructible_v<unique<int, non_constructible_traits<int>>>);

            // We can't test with std::is_default_constructible_v since SFINAE is not involved (it only fails when
            // trying to instantiate the function template), but the following should fail to compile:
            //unique<int, empty_traits<int>&> x;

            unique_empty<int> uniqueInt;
            unique_ptr<int> intPtr;
            unique_ptr<int[]> intArray;
            unique_binary_value uniqueBinaryValue;
        }

        TEST_METHOD(DefaultConstructorNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_default_constructible_v<unique_empty<int>>);
            Assert::IsTrue(std::is_nothrow_default_constructible_v<unique_empty<std::string>>);
            Assert::IsTrue(std::is_nothrow_default_constructible_v<unique_ptr<int>>);
            Assert::IsTrue(std::is_nothrow_default_constructible_v<unique_ptr<int[]>>);

            struct throwing_type
            {
                throwing_type() {}
            };

            Assert::IsFalse(std::is_nothrow_default_constructible_v<unique_empty<throwing_type>>);
            Assert::IsFalse(std::is_nothrow_default_constructible_v<unique<int, throwing_traits<int>>>);
        }

        TEST_METHOD(ValueConstructionTest)
        {
            // Constructing a unique<int, ...> should follow int's construction rules
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, const int>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, char>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<int>, int*>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<int>, std::string>);

            // const int should be constructible with int
            Assert::IsTrue(std::is_constructible_v<unique_empty<const int>, int>);

            unique_empty<const int> uniqueInt(8);

            // Constructing a unique<std::string, ...> should follow std::string's construction rules
            Assert::IsTrue(std::is_constructible_v<unique_empty<std::string>, char*>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<std::string>, const char*>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<std::string>, const std::string&>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<std::string>, std::string&&>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<std::string>, const wchar_t*>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<std::string>, const std::wstring&>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<std::string>, std::wstring&&>);

            unique_empty<std::string> uniqueString("foo");

            // Constructing a unique_ptr should follow the same construction rules as std::unique_ptr
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int>, int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int>, int* const>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<int>, const int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int>, std::nullptr_t>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int>, int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int>, const int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int>, std::nullptr_t>);

            Assert::IsTrue(std::is_constructible_v<unique_ptr<base>, derived*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<base>, const derived*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<base>, std::nullptr_t>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base>, derived*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base>, const derived*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base>, std::nullptr_t>);

            Assert::IsFalse(std::is_constructible_v<unique_ptr<derived>, base*>);

            unique_ptr<int> uniqueIntPtr(nullptr);
            unique_ptr<int> uniqueIntPtr2(new int());
            unique_ptr<base> uniqueBasePtr(new derived());

            // Constructing a unique_ptr<T[]> should follow the same construction rules as std::unique_ptr<T[]>
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int[]>, int*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<int[]>, const int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int[]>, std::nullptr_t>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int[]>, int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int[]>, const int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int[]>, std::nullptr_t>);

            Assert::IsTrue(std::is_constructible_v<unique_ptr<base[]>, base*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<base[]>, const base*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<base[]>, std::nullptr_t>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base[]>, base*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base[]>, const base*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base[]>, std::nullptr_t>);

            Assert::IsFalse(std::is_constructible_v<unique_ptr<base[]>, derived*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<base[]>, const derived*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<const base[]>, derived*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<const base[]>, const derived*>);

            Assert::IsFalse(std::is_constructible_v<unique_ptr<derived[]>, base*>);

            unique_ptr<int[]> uniqueIntArray(nullptr);
            unique_ptr<int[]> uniqueIntArray2(new int[4]);
            unique_ptr<base[]> uniqueBaseArray(new base[8]);
        }

        TEST_METHOD(ConstructWithTraitsTest)
        {
            empty_traits<int> emptyIntTraits;
            unique_traits<int*> uniqueIntPtrTraits;
            unique_traits<int(*)[]> uniqueIntArrayTraits;

            // Value
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int, empty_traits<int>>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int, empty_traits<int>&>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int, const empty_traits<int>&>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int, empty_traits<int>&&>);

            unique_empty<int> uniqueIntValue1(0, emptyIntTraits);
            unique_empty<int> uniqueIntValue2(0, empty_traits<int>{});

            // Reference
            Assert::IsFalse(std::is_constructible_v<unique<int, empty_traits<int>&>, int, empty_traits<int>>);
            Assert::IsTrue(std::is_constructible_v<unique<int, empty_traits<int>&>, int, empty_traits<int>&>);
            Assert::IsFalse(std::is_constructible_v<unique<int, empty_traits<int>&>, int, const empty_traits<int>&>);
            Assert::IsFalse(std::is_constructible_v<unique<int, empty_traits<int>&>, int, empty_traits<int>&&>);

            unique<int, empty_traits<int>&> uniqueIntValue3(8, emptyIntTraits);

            // Pointer
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int>, int*, unique_traits<int*>>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int>, int*, const unique_traits<int*>&>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<int>, int*, unique_traits<int(*)[]>>);

            Assert::IsTrue(std::is_constructible_v<unique_ptr<base>, derived*, unique_traits<base*>>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<base>, derived*, const unique_traits<base*>&>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<base>, derived*, unique_traits<base(*)[]>>);

            unique_ptr<int> uniquePtr1(new int(), unique_traits<int*>{});
            unique_ptr<int> uniquePtr2(new int(), uniqueIntPtrTraits);

            // Array
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int[]>, int*, unique_traits<int(*)[]>>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int[]>, int*, const unique_traits<int(*)[]>&>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<int[]>, int*, unique_traits<int*>>);

            Assert::IsFalse(std::is_constructible_v<unique_ptr<base[]>, derived*, unique_traits<base(*)[]>>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<base[]>, derived*, const unique_traits<base(*)[]>&>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<base[]>, derived*, unique_traits<base*>>);

            unique_ptr<int[]> uniqueArray1(new int(), unique_traits<int(*)[]>{});
            unique_ptr<int[]> uniqueArray2(new int(), uniqueIntArrayTraits);
        }

        TEST_METHOD(MoveConstructionTest)
        {
            // Value
            Assert::IsTrue(std::is_move_constructible_v<unique_empty<int>>);

            unique_empty<int> uniqueInt(8);
            unique_empty<int> uniqueIntCopy(std::move(uniqueInt));
            Assert::AreEqual(uniqueInt.get(), 0);
            Assert::AreEqual(uniqueIntCopy.get(), 8);

            // References would assign through, so testing move construction is kind of pointless... In fact, it
            // probably wouldn't even compile since default_value wouldn't make sense

            // Reference Traits
            Assert::IsTrue(std::is_move_constructible_v<unique<int, unique_empty<int>&>>);

            empty_traits<int> traits;
            unique<int, empty_traits<int>&> uniqueIntRef(8, traits);
            unique<int, empty_traits<int>&> uniqueIntRefCopy(std::move(uniqueIntRef));
            Assert::AreEqual(uniqueIntRef.get(), 0);
            Assert::AreEqual(uniqueIntRefCopy.get(), 8);

            // Pointer
            Assert::IsTrue(std::is_move_constructible_v<unique_ptr<int>>);

            unique_ptr<int> ptr1(new int());
            unique_ptr<int> ptr2(std::move(ptr1));
            Assert::IsNull(ptr1.get());
            Assert::IsNotNull(ptr2.get());

            // Array
            Assert::IsTrue(std::is_move_constructible_v<unique_ptr<int[]>>);

            unique_ptr<int[]> array1(new int[8]);
            unique_ptr<int[]> array2(std::move(array1));
            Assert::IsNull(array1.get());
            Assert::IsNotNull(array2.get());
        }

        TEST_METHOD(MoveConversionConstructionTest)
        {
            // empty_int_traits should be implicitly convertible to empty_traits<int>
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, unique_empty_int&&>);
            Assert::IsFalse(std::is_constructible_v<unique_empty_int, unique_empty<int>&&>);

            unique_empty_int from(8);
            unique_empty<int> to(std::move(from));
            Assert::AreEqual(0, from.get());
            Assert::AreEqual(8, to.get());

            // Should not be able to convert from arrays to pointers and vice versa
            Assert::IsFalse(std::is_constructible_v<unique_ptr<int>, unique_ptr<int[]>&&>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<int[]>, unique_ptr<int>&&>);
        }

        TEST_METHOD(CannotCopyTest)
        {
            Assert::IsFalse(std::is_copy_constructible_v<unique_empty<int>>);
            Assert::IsFalse(std::is_copy_constructible_v<unique_ptr<int>>);
            Assert::IsFalse(std::is_copy_constructible_v<unique_ptr<int[]>>);
        }

        TEST_METHOD(DestructorTest)
        {
            object_counter::reset();

            {
                unique_empty<object_counter> a;
                unique_ptr<object_counter> b(new object_counter());
                unique_ptr<object_counter[]> c(new object_counter[8]);

                Assert::AreEqual(10u, object_counter::instance_count);
                Assert::AreEqual(0u, object_counter::copy_count);
            }

            Assert::AreEqual(0u, object_counter::instance_count);
            Assert::AreEqual(0u, object_counter::copy_count);
        }

#pragma endregion



#pragma region Member Function Tests

        TEST_METHOD(ReleaseTest)
        {
            {
                object_counter::reset();
                unique_empty<object_counter> uniqueCounter;
                auto counter = uniqueCounter.release();
                Assert::AreEqual(2u, object_counter::instance_count);
                Assert::AreEqual(0u, object_counter::copy_count);
            }
            Assert::AreEqual(0u, object_counter::instance_count);

            {
                object_counter::reset();
                unique_ptr<object_counter> ptr(new object_counter());
                auto counter = ptr.release();
                Assert::AreEqual(1u, object_counter::instance_count);
                Assert::AreEqual(0u, object_counter::copy_count);

                delete counter;
                Assert::AreEqual(0u, object_counter::instance_count);
            }
            Assert::AreEqual(0u, object_counter::instance_count);

            {
                object_counter::reset();
                unique_ptr<object_counter[]> ptr(new object_counter[10]);
                auto counter = ptr.release();
                Assert::AreEqual(10u, object_counter::instance_count);
                Assert::AreEqual(0u, object_counter::copy_count);

                delete[] counter;
                Assert::AreEqual(0u, object_counter::instance_count);
            }
            Assert::AreEqual(0u, object_counter::instance_count);
        }

#pragma endregion
    };
}
