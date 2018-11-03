/*
 * Duncan Horn
 *
 * StringTests.cpp
 *
 * Tests for string.h
 */

#include <dhorn/string.h>
#include <gtest/gtest.h>

TEST(StringTests, TrimEmptyStringTest)
{
    std::string str;
    str = dhorn::trim(str);
    ASSERT_TRUE(str.empty());
    ASSERT_EQ(static_cast<std::size_t>(0), str.size());

    std::wstring wstr;
    wstr = dhorn::trim(wstr);
    ASSERT_TRUE(wstr.empty());
    ASSERT_EQ(static_cast<std::size_t>(0), wstr.size());
}

TEST(StringTests, TrimAllWhitespaceTest)
{
    std::string str("    \t\n");
    str = dhorn::trim(str);
    ASSERT_TRUE(str.empty());
    ASSERT_EQ(static_cast<std::size_t>(0), str.size());

    std::wstring wstr(L"    \t\n");
    wstr = dhorn::trim(wstr);
    ASSERT_TRUE(wstr.empty());
    ASSERT_EQ(static_cast<std::size_t>(0), wstr.size());
}

TEST(StringTests, TrimSingleWordTest)
{
    std::string str("foo");
    str = dhorn::trim(str);
    ASSERT_TRUE(str == "foo");

    std::wstring wstr(L"foo");
    wstr = dhorn::trim(wstr);
    ASSERT_TRUE(wstr == L"foo");
}

TEST(StringTests, TrimSentenceTest)
{
    std::string str("foo bar");
    str = dhorn::trim(str);
    ASSERT_TRUE(str == "foo bar");

    std::wstring wstr(L"foo bar");
    wstr = dhorn::trim(wstr);
    ASSERT_TRUE(wstr == L"foo bar");
}

TEST(StringTests, TrimLeadingWhitespaceTest)
{
    std::string str("   \t\nfoo");
    str = dhorn::trim(str);
    ASSERT_TRUE(str == "foo");

    std::wstring wstr(L"   \t\nfoo");
    wstr = dhorn::trim(wstr);
    ASSERT_TRUE(wstr == L"foo");
}

TEST(StringTests, TrimTrailingWhitespaceTest)
{
    std::string str("foo   \t\n");
    str = dhorn::trim(str);
    ASSERT_TRUE(str == "foo");

    std::wstring wstr(L"foo   \t\n");
    wstr = dhorn::trim(wstr);
    ASSERT_TRUE(wstr == L"foo");
}

TEST(StringTests, CustomTrimTest)
{
    auto values = { 'a', 'e', 'i', 'o', 'u' };

    std::string str("aeioufoo baruoiea");
    str = dhorn::trim(str, std::begin(values), std::end(values));
    ASSERT_TRUE(str == "foo bar");

    std::wstring wstr(L"aeioufoo baruoiea");
    wstr = dhorn::trim(wstr, std::begin(values), std::end(values));
    ASSERT_TRUE(wstr == L"foo bar");
}

TEST(StringTests, StartsWithIteratorTest)
{
    std::string str = "foobar";
    std::string foo = "foo";
    std::string bar = "bar";

    ASSERT_TRUE(dhorn::starts_with(str.begin(), str.end(), foo.begin(), foo.end()));
    ASSERT_FALSE(dhorn::starts_with(str.begin(), str.end(), bar.begin(), bar.end()));

    ASSERT_TRUE(dhorn::starts_with(str.begin(), str.end(), str.begin(), str.end()));
    ASSERT_FALSE(dhorn::starts_with(foo.begin(), foo.end(), str.begin(), str.end()));

    std::string abc = "abc";
    std::string abd = "abd";
    ASSERT_FALSE(dhorn::starts_with(abc.begin(), abc.end(), abd.begin(), abd.end()));
    ASSERT_FALSE(dhorn::starts_with(abd.begin(), abd.end(), abc.begin(), abc.end()));

    std::string empty;
    ASSERT_TRUE(dhorn::starts_with(str.begin(), str.end(), empty.begin(), empty.end()));
    ASSERT_FALSE(dhorn::starts_with(empty.begin(), empty.end(), str.begin(), str.end()));
    ASSERT_TRUE(dhorn::starts_with(empty.begin(), empty.end(), empty.begin(), empty.end()));
}

TEST(StringTests, StartsWithStringIteratorTest)
{
    std::string str = "foobar";
    std::string foo = "foo";
    std::string bar = "bar";

    ASSERT_TRUE(dhorn::starts_with(str, foo.begin(), foo.end()));
    ASSERT_FALSE(dhorn::starts_with(str, bar.begin(), bar.end()));
    ASSERT_TRUE(dhorn::starts_with(str, str.begin(), str.end()));
    ASSERT_FALSE(dhorn::starts_with(foo, str.begin(), str.end()));

    std::string_view strView = "foobar";
    ASSERT_TRUE(dhorn::starts_with(strView, foo.begin(), foo.end()));
    ASSERT_FALSE(dhorn::starts_with(strView, bar.begin(), bar.end()));
    ASSERT_TRUE(dhorn::starts_with(strView, str.begin(), str.end()));

    dhorn::const_null_terminated_string nullStr = "foobar";
    ASSERT_TRUE(dhorn::starts_with(nullStr, foo.begin(), foo.end()));
    ASSERT_FALSE(dhorn::starts_with(nullStr, bar.begin(), bar.end()));
    ASSERT_TRUE(dhorn::starts_with(nullStr, str.begin(), str.end()));
}

TEST(StringTests, StartsWithStringTest)
{
    std::string str = "foobar";
    std::string foo = "foo";
    std::string bar = "bar";

    ASSERT_TRUE(dhorn::starts_with(str, foo));
    ASSERT_FALSE(dhorn::starts_with(str, bar));
    ASSERT_TRUE(dhorn::starts_with(str, str));
    ASSERT_FALSE(dhorn::starts_with(foo, str));

    std::string_view strView = "foobar";
    ASSERT_TRUE(dhorn::starts_with(strView, foo));
    ASSERT_FALSE(dhorn::starts_with(strView, bar));
    ASSERT_TRUE(dhorn::starts_with(strView, str));
    ASSERT_TRUE(dhorn::starts_with(str, strView));

    dhorn::const_null_terminated_string nullStr = "foobar";
    ASSERT_TRUE(dhorn::starts_with(nullStr, foo));
    ASSERT_FALSE(dhorn::starts_with(nullStr, bar));
    ASSERT_TRUE(dhorn::starts_with(nullStr, str));
    ASSERT_TRUE(dhorn::starts_with(str, nullStr));
    ASSERT_TRUE(dhorn::starts_with(nullStr, strView));
    ASSERT_TRUE(dhorn::starts_with(strView, nullStr));
}

TEST(StringTests, StartsWithStringStringLiteralTest)
{
    std::string str = "foobar";
    char foo[] = "foo";
    const char* bar = "bar";

    ASSERT_TRUE(dhorn::starts_with(str, foo));
    ASSERT_FALSE(dhorn::starts_with(str, bar));

    std::string_view strView = "foobar";
    ASSERT_TRUE(dhorn::starts_with(strView, foo));
    ASSERT_FALSE(dhorn::starts_with(strView, bar));

    dhorn::const_null_terminated_string nullStr = "foobar";
    ASSERT_TRUE(dhorn::starts_with(nullStr, foo));
    ASSERT_FALSE(dhorn::starts_with(nullStr, bar));
}

TEST(StringTests, StartsWithStringLiteralIteratorTest)
{
    char str[] = "foobar";
    const char* constStr = "foobar";
    std::string foo = "foo";
    std::string bar = "bar";
    std::string foobar = "foobar";

    ASSERT_TRUE(dhorn::starts_with(str, foo.begin(), foo.end()));
    ASSERT_FALSE(dhorn::starts_with(str, bar.begin(), bar.end()));
    ASSERT_TRUE(dhorn::starts_with(str, foobar.begin(), foobar.end()));

    ASSERT_TRUE(dhorn::starts_with(constStr, foo.begin(), foo.end()));
    ASSERT_FALSE(dhorn::starts_with(constStr, bar.begin(), bar.end()));
    ASSERT_TRUE(dhorn::starts_with(constStr, foobar.begin(), foobar.end()));
}

TEST(StringTests, StartsWithStringLiteralStringTest)
{
    char str[] = "foobar";
    const char* constStr = "foobar";
    std::string foo = "foo";
    std::string bar = "bar";
    std::string foobar = "foobar";

    ASSERT_TRUE(dhorn::starts_with(str, foo));
    ASSERT_FALSE(dhorn::starts_with(str, bar));
    ASSERT_TRUE(dhorn::starts_with(str, foobar));

    ASSERT_TRUE(dhorn::starts_with(constStr, foo));
    ASSERT_FALSE(dhorn::starts_with(constStr, bar));
    ASSERT_TRUE(dhorn::starts_with(constStr, foobar));
}

TEST(StringTests, StartsWithStringLiteralTest)
{
    char str[] = "foobar";
    const char* constStr = "foobar";
    char foo[] = "foo";
    const char* bar = "bar";

    ASSERT_TRUE(dhorn::starts_with(str, foo));
    ASSERT_FALSE(dhorn::starts_with(str, bar));
    ASSERT_TRUE(dhorn::starts_with(str, constStr));

    ASSERT_TRUE(dhorn::starts_with(constStr, foo));
    ASSERT_FALSE(dhorn::starts_with(constStr, bar));
    ASSERT_TRUE(dhorn::starts_with(constStr, str));
}

TEST(NullTerminatedStringTests, AssignmentTest)
{
    // NOTE: Strictly a "does it compile" test
    char fooString[] = "foo";
    dhorn::null_terminated_string str1;
    dhorn::null_terminated_string str2 = fooString;
    str1 = str2;

    dhorn::basic_null_terminated_string<const char> constStr;
    constStr = str1;
    constStr = static_cast<const dhorn::null_terminated_string>(str2);

    dhorn::basic_null_terminated_string<const char> otherConstStr;
    otherConstStr = constStr;

    str1 = const_cast<char*>("foobar");
    constStr = const_cast<char*>("foobar");
    constStr = static_cast<const char*>("foobar");
}

TEST(NullTerminatedStringTests, IndexOperatorTest)
{
    dhorn::const_null_terminated_string str = "foo";
    ASSERT_EQ('f', str[0]);
    ASSERT_EQ('o', str[1]);
    ASSERT_EQ('o', str[2]);
    ASSERT_EQ('\0', str[3]);
}

TEST(NullTerminatedStringTests, EmptyTest)
{
    dhorn::const_null_terminated_string str;
    ASSERT_TRUE(str.empty());

    str = "";
    ASSERT_TRUE(str.empty());

    str = "a";
    ASSERT_FALSE(str.empty());
}

TEST(NullTerminatedStringTests, IterationTest)
{
    char testString[] = "foobar";
    std::string str;
    for (auto ch : dhorn::null_terminated_string(testString))
    {
        str.push_back(ch);
    }

    ASSERT_TRUE(str == testString);
}

template <typename CharT>
void BasicIterationTestImpl(CharT* str)
{
    std::basic_string<std::remove_const_t<CharT>> result;
    std::for_each(
        dhorn::basic_null_terminated_string_iterator<CharT>(str),
        dhorn::basic_null_terminated_string_iterator<CharT>(),
        [&](CharT value)
    {
        result.push_back(value);
    });

    ASSERT_TRUE(str == result);
}

TEST(NullTerminatedStringTests, BasicIterationTest)
{
    BasicIterationTestImpl("foo");
    BasicIterationTestImpl(L"foo");
    BasicIterationTestImpl(u8"foo");
    BasicIterationTestImpl(u"foo");
    BasicIterationTestImpl(U"foo");
}

TEST(NullTerminatedStringTests, IteratorConstConversionTest)
{
    // Strictly a "does it compile" test
    char fooString[] = "foo";
    dhorn::basic_null_terminated_string_iterator<char> itr(fooString);
    [[maybe_unused]] dhorn::basic_null_terminated_string_iterator<const char> constItr = itr;
}

TEST(NullTerminatedStringTests, IteratorConstComparisonTest)
{
    char str[] = "foo";
    dhorn::basic_null_terminated_string_iterator<char> itr(str);
    dhorn::basic_null_terminated_string_iterator<const char> constItr(str);

    for (; itr != dhorn::null_terminated_string_iterator(); ++itr, ++constItr)
    {
        ASSERT_TRUE(itr == constItr);
        ASSERT_TRUE(constItr == itr);

        ASSERT_FALSE(itr != constItr);
        ASSERT_FALSE(constItr != itr);
    }
}
