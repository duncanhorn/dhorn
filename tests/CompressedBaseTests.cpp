/*
 * Duncan Horn
 *
 * CompressedBaseTests.cpp
 *
 * Tests for the compressed_base.h header
 */

#include <dhorn/compressed_base.h>
#include <dhorn/utility.h>
#include <gtest/gtest.h>

#include "object_counter.h"

using namespace dhorn::literals;

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

struct throwing_empty
{
    throwing_empty() {}
    throwing_empty(const throwing_empty&) {}
    throwing_empty& operator=(const throwing_empty&) { return *this; }
    throwing_empty& operator=(throwing_empty&&) { return *this; }
};

struct throwing_empty_final final
{
    throwing_empty_final() {}
    throwing_empty_final(const throwing_empty_final&) {}
    throwing_empty_final& operator=(const throwing_empty_final&) { return *this; }
    throwing_empty_final& operator=(throwing_empty_final&&) { return *this; }
};

struct throwing_non_empty
{
    throwing_non_empty() {}
    throwing_non_empty(const throwing_non_empty&) {}
    throwing_non_empty& operator=(const throwing_non_empty&) { return *this; }
    throwing_non_empty& operator=(throwing_non_empty&&) { return *this; }

    int value = 42;
};

struct throwing_non_empty_final final
{
    throwing_non_empty_final() {}
    throwing_non_empty_final(const throwing_non_empty_final&) {}
    throwing_non_empty_final& operator=(const throwing_non_empty_final&) { return *this; }
    throwing_non_empty_final& operator=(throwing_non_empty_final&&) { return *this; }

    int value = 42;
};

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

struct CompressedBaseTests : public testing::Test
{
    void SetUp() override
    {
        dhorn::tests::object_counter::reset();
    }

    void TearDown() override
    {
        ASSERT_EQ(0_sz, dhorn::tests::object_counter::instance_count);
    }
};

TEST_F(CompressedBaseTests, DefaultConstructionTest)
{
    ASSERT_TRUE(std::is_default_constructible_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_default_constructible_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_default_constructible_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_default_constructible_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_default_constructible_v<dhorn::compressed_base<cant_construct_empty>>);
    ASSERT_FALSE(std::is_default_constructible_v<dhorn::compressed_base<cant_construct_empty_final>>);
    ASSERT_FALSE(std::is_default_constructible_v<dhorn::compressed_base<cant_construct_non_empty>>);
    ASSERT_FALSE(std::is_default_constructible_v<dhorn::compressed_base<cant_construct_non_empty_final>>);

    dhorn::compressed_base<non_empty> nonEmpty;
    ASSERT_EQ(42, nonEmpty.value().value);

    dhorn::compressed_base<non_empty_final> nonEmptyFinal;
    ASSERT_EQ(42, nonEmptyFinal.value().value);

    dhorn::compressed_base<dhorn::tests::object_counter> obj;
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::constructed_count);
}

TEST_F(CompressedBaseTests, DefaultConstructionNoexceptTest)
{
    ASSERT_TRUE(std::is_nothrow_default_constructible_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_nothrow_default_constructible_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_nothrow_default_constructible_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_nothrow_default_constructible_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_nothrow_default_constructible_v<dhorn::compressed_base<throwing_empty>>);
    ASSERT_FALSE(std::is_nothrow_default_constructible_v<dhorn::compressed_base<throwing_empty_final>>);
    ASSERT_FALSE(std::is_nothrow_default_constructible_v<dhorn::compressed_base<throwing_non_empty>>);
    ASSERT_FALSE(std::is_nothrow_default_constructible_v<dhorn::compressed_base<throwing_non_empty_final>>);
}

TEST_F(CompressedBaseTests, ValueCopyConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<empty>, const empty&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<empty_final>, const empty_final&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<non_empty>, const non_empty&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<non_empty_final>, const non_empty_final&>));

    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<cant_construct_empty>, const cant_construct_empty&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<cant_construct_empty_final>, const cant_construct_empty_final&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<cant_construct_non_empty>, const cant_construct_non_empty&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<cant_construct_non_empty_final>, const cant_construct_non_empty_final&>));

    dhorn::tests::object_counter cnt;
    dhorn::compressed_base<dhorn::tests::object_counter> obj(cnt);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedBaseTests, ValueCopyConstructionNoexceptTest)
{
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<empty>, const empty&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<empty_final>, const empty_final&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<non_empty>, const non_empty&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<non_empty_final>, const non_empty_final&>));

    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_empty>, const throwing_empty&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_empty_final>, const throwing_empty_final&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_non_empty>, const throwing_non_empty&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_non_empty_final>, const throwing_non_empty_final&>));
}

TEST_F(CompressedBaseTests, ValueMoveConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<empty>, empty&&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<empty_final>, empty_final&&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<non_empty>, non_empty&&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<non_empty_final>, non_empty_final&&>));

    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<cant_construct_empty>, cant_construct_empty&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<cant_construct_empty_final>, cant_construct_empty_final&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<cant_construct_non_empty>, cant_construct_non_empty&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<cant_construct_non_empty_final>, cant_construct_non_empty_final&&>));

    dhorn::compressed_base<dhorn::tests::object_counter> obj(dhorn::tests::object_counter{});
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::move_count);
}

TEST_F(CompressedBaseTests, ValueMoveConstructionNoexceptTest)
{
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<empty>, empty&&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<empty_final>, empty_final&&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<non_empty>, non_empty&&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<non_empty_final>, non_empty_final&&>));

    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_empty>, throwing_empty&&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_empty_final>, throwing_empty_final&&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_non_empty>, throwing_non_empty&&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_non_empty_final>, throwing_non_empty_final&&>));
}

TEST_F(CompressedBaseTests, CopyConstructionTest)
{
    ASSERT_TRUE(std::is_copy_constructible_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_copy_constructible_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_copy_constructible_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_copy_constructible_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_copy_constructible_v<dhorn::compressed_base<cant_construct_empty>>);
    ASSERT_FALSE(std::is_copy_constructible_v<dhorn::compressed_base<cant_construct_empty_final>>);
    ASSERT_FALSE(std::is_copy_constructible_v<dhorn::compressed_base<cant_construct_non_empty>>);
    ASSERT_FALSE(std::is_copy_constructible_v<dhorn::compressed_base<cant_construct_non_empty_final>>);

    dhorn::compressed_base<dhorn::tests::object_counter> obj;
    dhorn::compressed_base<dhorn::tests::object_counter> objCopy(obj);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedBaseTests, CopyConstructionNoexceptTest)
{
    ASSERT_TRUE(std::is_nothrow_copy_constructible_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_nothrow_copy_constructible_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_nothrow_copy_constructible_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_nothrow_copy_constructible_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_nothrow_copy_constructible_v<dhorn::compressed_base<throwing_empty>>);
    ASSERT_FALSE(std::is_nothrow_copy_constructible_v<dhorn::compressed_base<throwing_empty_final>>);
    ASSERT_FALSE(std::is_nothrow_copy_constructible_v<dhorn::compressed_base<throwing_non_empty>>);
    ASSERT_FALSE(std::is_nothrow_copy_constructible_v<dhorn::compressed_base<throwing_non_empty_final>>);
}

TEST_F(CompressedBaseTests, CopyConversionConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<to_empty>, const dhorn::compressed_base<from_empty>&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<to_empty_final>, const dhorn::compressed_base<from_empty_final>&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<to_non_empty>, const dhorn::compressed_base<from_non_empty>&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<to_non_empty_final>, const dhorn::compressed_base<from_non_empty_final>&>));

    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<from_empty>, const dhorn::compressed_base<to_empty>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<from_empty_final>, const dhorn::compressed_base<to_empty_final>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<from_non_empty>, const dhorn::compressed_base<to_non_empty>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<from_non_empty_final>, const dhorn::compressed_base<to_non_empty_final>&>));

    struct derived : public dhorn::tests::object_counter {};
    dhorn::compressed_base<derived> d;
    dhorn::compressed_base<dhorn::tests::object_counter> obj(d);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedBaseTests, CopyConversionConstructionNoexceptTest)
{
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<to_empty>, const dhorn::compressed_base<from_empty>&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<to_empty_final>, const dhorn::compressed_base<from_empty_final>&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<to_non_empty>, const dhorn::compressed_base<from_non_empty>&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<to_non_empty_final>, const dhorn::compressed_base<from_non_empty_final>&>));

    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_to_empty>, const dhorn::compressed_base<from_empty>&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_to_empty_final>, const dhorn::compressed_base<from_empty_final>&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_to_non_empty>, const dhorn::compressed_base<from_non_empty>&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_to_non_empty_final>, const dhorn::compressed_base<from_non_empty_final>&>));
}

TEST_F(CompressedBaseTests, MoveConstructionTest)
{
    ASSERT_TRUE(std::is_move_constructible_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_move_constructible_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_move_constructible_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_move_constructible_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_move_constructible_v<dhorn::compressed_base<cant_construct_empty>>);
    ASSERT_FALSE(std::is_move_constructible_v<dhorn::compressed_base<cant_construct_empty_final>>);
    ASSERT_FALSE(std::is_move_constructible_v<dhorn::compressed_base<cant_construct_non_empty>>);
    ASSERT_FALSE(std::is_move_constructible_v<dhorn::compressed_base<cant_construct_non_empty_final>>);

    dhorn::compressed_base<dhorn::tests::object_counter> obj;
    dhorn::compressed_base<dhorn::tests::object_counter> objCopy(std::move(obj));
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::move_count);
}

TEST_F(CompressedBaseTests, MoveConstructionNoexceptTest)
{
    ASSERT_TRUE(std::is_nothrow_move_constructible_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_nothrow_move_constructible_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_nothrow_move_constructible_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_nothrow_move_constructible_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_nothrow_move_constructible_v<dhorn::compressed_base<throwing_empty>>);
    ASSERT_FALSE(std::is_nothrow_move_constructible_v<dhorn::compressed_base<throwing_empty_final>>);
    ASSERT_FALSE(std::is_nothrow_move_constructible_v<dhorn::compressed_base<throwing_non_empty>>);
    ASSERT_FALSE(std::is_nothrow_move_constructible_v<dhorn::compressed_base<throwing_non_empty_final>>);
}

TEST_F(CompressedBaseTests, MoveConversionConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<to_empty>, dhorn::compressed_base<from_empty>&&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<to_empty_final>, dhorn::compressed_base<from_empty_final>&&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<to_non_empty>, dhorn::compressed_base<from_non_empty>&&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<to_non_empty_final>, dhorn::compressed_base<from_non_empty_final>&&>));

    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<from_empty>, dhorn::compressed_base<to_empty>&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<from_empty_final>, dhorn::compressed_base<to_empty_final>&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<from_non_empty>, dhorn::compressed_base<to_non_empty>&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<from_non_empty_final>, dhorn::compressed_base<to_non_empty_final>&&>));

    struct derived : public dhorn::tests::object_counter {};
    dhorn::compressed_base<derived> d;
    dhorn::compressed_base<dhorn::tests::object_counter> obj(std::move(d));
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::move_count);
}

TEST_F(CompressedBaseTests, MoveConversionConstructionNoexceptTest)
{
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<to_empty>, dhorn::compressed_base<from_empty>&&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<to_empty_final>, dhorn::compressed_base<from_empty_final>&&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<to_non_empty>, dhorn::compressed_base<from_non_empty>&&>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<to_non_empty_final>, dhorn::compressed_base<from_non_empty_final>&&>));

    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_to_empty>, dhorn::compressed_base<from_empty>&&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_to_empty_final>, dhorn::compressed_base<from_empty_final>&&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_to_non_empty>, dhorn::compressed_base<from_non_empty>&&>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<throwing_to_non_empty_final>, dhorn::compressed_base<from_non_empty_final>&&>));
}

TEST_F(CompressedBaseTests, EmplaceConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_empty>, char*, int>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_empty>, std::string>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_empty_final>, char*, int>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_empty_final>, std::string>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty>, char*, int>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty>, std::string>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty_final>, char*, int>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty_final>, std::string>));

    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<construct_empty>, const char*, int>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<construct_empty_final>, const char*, int>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty>, const char*, int>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty_final>, const char*, int>));

    struct test
    {
        test(const dhorn::tests::object_counter& obj) : obj(obj) {}
        test(dhorn::tests::object_counter&& obj) : obj(std::move(obj)) {}

        dhorn::tests::object_counter obj;
    };

    dhorn::tests::object_counter o;
    dhorn::compressed_base<test> a(dhorn::tests::object_counter{});
    dhorn::compressed_base<test> b(o);
    ASSERT_EQ(3_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::copy_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::move_count);
}

TEST_F(CompressedBaseTests, EmplaceConstructionNoexceptTest)
{
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<construct_empty>, char*, int>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<construct_empty>, std::string>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<construct_empty_final>, char*, int>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<construct_empty_final>, std::string>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<construct_non_empty>, char*, int>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<construct_non_empty>, std::string>));
    ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::compressed_base<construct_non_empty_final>, char*, int>));
    ASSERT_FALSE((std::is_nothrow_constructible_v<dhorn::compressed_base<construct_non_empty_final>, std::string>));
}

TEST_F(CompressedBaseTests, TupleConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_empty>, std::tuple<char*, int>>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_empty>, std::tuple<std::string>>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_empty_final>, std::tuple<char*, int>>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_empty_final>, std::tuple<std::string>>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty>, std::tuple<char*, int>>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty>, std::tuple<std::string>>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty_final>, std::tuple<char*, int>>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty_final>, std::tuple<std::string>>));

    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<construct_empty>, std::tuple<const char*, int>>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<construct_empty_final>, std::tuple<const char*, int>>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty>, std::tuple<const char*, int>>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::compressed_base<construct_non_empty_final>, std::tuple<const char*, int>>));

    struct test
    {
        test(const dhorn::tests::object_counter& obj) : obj(obj) {}
        test(dhorn::tests::object_counter&& obj) : obj(std::move(obj)) {}

        dhorn::tests::object_counter obj;
    };

    dhorn::tests::object_counter o;
    dhorn::compressed_base<test> a(std::make_tuple(dhorn::tests::object_counter{})); // move (twice)
    dhorn::compressed_base<test> b(std::forward_as_tuple(o)); // copy
    ASSERT_EQ(3_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::copy_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::move_count);
}

//TEST_F(CompressedBaseTests, TupleConstructionNoexceptTest)
//{
//}

TEST_F(CompressedBaseTests, ValueCopyAssignmentOperatorTest)
{
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<empty>, const empty&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<empty_final>, const empty_final&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<non_empty>, const non_empty&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<non_empty_final>, const non_empty_final&>));

    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<cant_construct_empty>, const cant_construct_empty&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<cant_construct_empty_final>, const cant_construct_empty_final&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<cant_construct_non_empty>, const cant_construct_non_empty&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<cant_construct_non_empty_final>, const cant_construct_non_empty_final&>));

    dhorn::tests::object_counter cnt;
    dhorn::compressed_base<dhorn::tests::object_counter> obj;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);

    obj = cnt;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedBaseTests, ValueCopyAssignmentOperatorNoexceptTest)
{
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<empty>, const empty&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<empty_final>, const empty_final&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<non_empty>, const non_empty&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<non_empty_final>, const non_empty_final&>));

    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_empty>, const throwing_empty&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_empty_final>, const throwing_empty_final&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_non_empty>, const throwing_non_empty&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_non_empty_final>, const throwing_non_empty_final&>));
}

TEST_F(CompressedBaseTests, ValueMoveAssignmentOperatorTest)
{
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<empty>, empty&&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<empty_final>, empty_final&&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<non_empty>, non_empty&&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<non_empty_final>, non_empty_final&&>));

    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<cant_construct_empty>, cant_construct_empty&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<cant_construct_empty_final>, cant_construct_empty_final&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<cant_construct_non_empty>, cant_construct_non_empty&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<cant_construct_non_empty_final>, cant_construct_non_empty_final&&>));

    dhorn::compressed_base<dhorn::tests::object_counter> obj;
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::instance_count);

    obj = dhorn::tests::object_counter{};
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::move_count);
}

TEST_F(CompressedBaseTests, ValueMoveAssignmentOperatorNoexceptTest)
{
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<empty>, empty&&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<empty_final>, empty_final&&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<non_empty>, non_empty&&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<non_empty_final>, non_empty_final&&>));

    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_empty>, throwing_empty&&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_empty_final>, throwing_empty_final&&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_non_empty>, throwing_non_empty&&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_non_empty_final>, throwing_non_empty_final&&>));
}

TEST_F(CompressedBaseTests, CopyAssignmentOperatorTest)
{
    ASSERT_TRUE(std::is_copy_assignable_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_copy_assignable_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_copy_assignable_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_copy_assignable_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_copy_assignable_v<dhorn::compressed_base<cant_construct_empty>>);
    ASSERT_FALSE(std::is_copy_assignable_v<dhorn::compressed_base<cant_construct_empty_final>>);
    ASSERT_FALSE(std::is_copy_assignable_v<dhorn::compressed_base<cant_construct_non_empty>>);
    ASSERT_FALSE(std::is_copy_assignable_v<dhorn::compressed_base<cant_construct_non_empty_final>>);

    dhorn::compressed_base<dhorn::tests::object_counter> obj;
    dhorn::compressed_base<dhorn::tests::object_counter> objCopy;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);

    objCopy = obj;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedBaseTests, CopyAssignmentOperatorNoexceptTest)
{
    ASSERT_TRUE(std::is_nothrow_copy_assignable_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_nothrow_copy_assignable_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_nothrow_copy_assignable_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_nothrow_copy_assignable_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_nothrow_copy_assignable_v<dhorn::compressed_base<throwing_empty>>);
    ASSERT_FALSE(std::is_nothrow_copy_assignable_v<dhorn::compressed_base<throwing_empty_final>>);
    ASSERT_FALSE(std::is_nothrow_copy_assignable_v<dhorn::compressed_base<throwing_non_empty>>);
    ASSERT_FALSE(std::is_nothrow_copy_assignable_v<dhorn::compressed_base<throwing_non_empty_final>>);
}

TEST_F(CompressedBaseTests, CopyConversionAssignmentOperatorTest)
{
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<to_empty>, const dhorn::compressed_base<from_empty>&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<to_empty_final>, const dhorn::compressed_base<from_empty_final>&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<to_non_empty>, const dhorn::compressed_base<from_non_empty>&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<to_non_empty_final>, const dhorn::compressed_base<from_non_empty_final>&>));

    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<from_empty>, const dhorn::compressed_base<to_empty>&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<from_empty_final>, const dhorn::compressed_base<to_empty_final>&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<from_non_empty>, const dhorn::compressed_base<to_non_empty>&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<from_non_empty_final>, const dhorn::compressed_base<to_non_empty_final>&>));

    struct derived : public dhorn::tests::object_counter {};
    dhorn::compressed_base<derived> d;
    dhorn::compressed_base<dhorn::tests::object_counter> obj;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);

    obj = d;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::copy_count);
}

TEST_F(CompressedBaseTests, CopyConversionAssignmentOperatorNoexceptTest)
{
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<to_empty>, const dhorn::compressed_base<from_empty>&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<to_empty_final>, const dhorn::compressed_base<from_empty_final>&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<to_non_empty>, const dhorn::compressed_base<from_non_empty>&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<to_non_empty_final>, const dhorn::compressed_base<from_non_empty_final>&>));

    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_to_empty>, const dhorn::compressed_base<from_empty>&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_to_empty_final>, const dhorn::compressed_base<from_empty_final>&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_to_non_empty>, const dhorn::compressed_base<from_non_empty>&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_to_non_empty_final>, const dhorn::compressed_base<from_non_empty_final>&>));
}

TEST_F(CompressedBaseTests, MoveAssignmentOperatorTest)
{
    ASSERT_TRUE(std::is_move_assignable_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_move_assignable_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_move_assignable_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_move_assignable_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_move_assignable_v<dhorn::compressed_base<cant_construct_empty>>);
    ASSERT_FALSE(std::is_move_assignable_v<dhorn::compressed_base<cant_construct_empty_final>>);
    ASSERT_FALSE(std::is_move_assignable_v<dhorn::compressed_base<cant_construct_non_empty>>);
    ASSERT_FALSE(std::is_move_assignable_v<dhorn::compressed_base<cant_construct_non_empty_final>>);

    dhorn::compressed_base<dhorn::tests::object_counter> obj;
    dhorn::compressed_base<dhorn::tests::object_counter> objCopy;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);

    objCopy = std::move(obj);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::move_count);
}

TEST_F(CompressedBaseTests, MoveAssignmentOperatorNoexceptTest)
{
    ASSERT_TRUE(std::is_nothrow_move_assignable_v<dhorn::compressed_base<empty>>);
    ASSERT_TRUE(std::is_nothrow_move_assignable_v<dhorn::compressed_base<empty_final>>);
    ASSERT_TRUE(std::is_nothrow_move_assignable_v<dhorn::compressed_base<non_empty>>);
    ASSERT_TRUE(std::is_nothrow_move_assignable_v<dhorn::compressed_base<non_empty_final>>);

    ASSERT_FALSE(std::is_nothrow_move_assignable_v<dhorn::compressed_base<throwing_empty>>);
    ASSERT_FALSE(std::is_nothrow_move_assignable_v<dhorn::compressed_base<throwing_empty_final>>);
    ASSERT_FALSE(std::is_nothrow_move_assignable_v<dhorn::compressed_base<throwing_non_empty>>);
    ASSERT_FALSE(std::is_nothrow_move_assignable_v<dhorn::compressed_base<throwing_non_empty_final>>);
}

TEST_F(CompressedBaseTests, MoveConversionAssignmentOperatorTest)
{
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<to_empty>, dhorn::compressed_base<from_empty>&&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<to_empty_final>, dhorn::compressed_base<from_empty_final>&&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<to_non_empty>, dhorn::compressed_base<from_non_empty>&&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::compressed_base<to_non_empty_final>, dhorn::compressed_base<from_non_empty_final>&&>));

    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<from_empty>, dhorn::compressed_base<to_empty>&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<from_empty_final>, dhorn::compressed_base<to_empty_final>&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<from_non_empty>, dhorn::compressed_base<to_non_empty>&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::compressed_base<from_non_empty_final>, dhorn::compressed_base<to_non_empty_final>&&>));

    struct derived : public dhorn::tests::object_counter {};
    dhorn::compressed_base<derived> d;
    dhorn::compressed_base<dhorn::tests::object_counter> obj;
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);

    obj = std::move(d);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::instance_count);
    ASSERT_EQ(2_sz, dhorn::tests::object_counter::constructed_count);
    ASSERT_EQ(1_sz, dhorn::tests::object_counter::move_count);
}

TEST_F(CompressedBaseTests, MoveConversionAssignmentOperatorNoexceptTest)
{
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<to_empty>, dhorn::compressed_base<from_empty>&&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<to_empty_final>, dhorn::compressed_base<from_empty_final>&&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<to_non_empty>, dhorn::compressed_base<from_non_empty>&&>));
    ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::compressed_base<to_non_empty_final>, dhorn::compressed_base<from_non_empty_final>&&>));

    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_to_empty>, dhorn::compressed_base<from_empty>&&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_to_empty_final>, dhorn::compressed_base<from_empty_final>&&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_to_non_empty>, dhorn::compressed_base<from_non_empty>&&>));
    ASSERT_FALSE((std::is_nothrow_assignable_v<dhorn::compressed_base<throwing_to_non_empty_final>, dhorn::compressed_base<from_non_empty_final>&&>));
}

TEST_F(CompressedBaseTests, SwapTest)
{
    dhorn::compressed_base<non_empty> a;
    dhorn::compressed_base<non_empty> b;
    a.value().value = 42;
    b.value().value = 8;
    a.swap(b);

    ASSERT_EQ(8, a.value().value);
    ASSERT_EQ(42, b.value().value);
}
