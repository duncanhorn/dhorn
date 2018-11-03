/*
 * Duncan Horn
 *
 * UnicodeEncodingTests.cpp
 *
 * Tests for the unicode/encoding.h functions and types
 */

#include <dhorn/unicode/encoding.h>
#include <dhorn/utility.h>
#include <gtest/gtest.h>

using namespace dhorn::literals;

TEST(UnicodeEncodingTests, PlaneTests)
{
    for (char32_t plane = 0; plane <= 16; ++plane)
    {
        for (char32_t value = 1; value <= 0xFFFF; value <<= 1)
        {
            char32_t ch = (plane << 16) | value;
            ASSERT_EQ(dhorn::unicode::plane(ch), static_cast<std::size_t>(plane));
        }
    }
}

TEST(UnicodeEncodingTests, IsHighSurrogateTest)
{
    // 1101'10XX'XXXX'XXXX
    char32_t highSurrogateMask = 0xD800;
    for (char32_t value = 1; value <= 0x03FF; value <<= 1)
    {
        char32_t ch = highSurrogateMask | value;
        ASSERT_TRUE(dhorn::unicode::is_high_surrogate(ch));
    }

    char32_t lowSurrogateMask = 0xDC00;
    for (char32_t value = 1; value <= 0x03FF; value <<= 1)
    {
        char32_t ch = lowSurrogateMask | value;
        ASSERT_FALSE(dhorn::unicode::is_high_surrogate(ch));
    }

    for (char32_t ch = 0; ch < 256; ++ch)
    {
        ASSERT_FALSE(dhorn::unicode::is_high_surrogate(ch));
    }
}

TEST(UnicodeEncodingTests, IsLowSurrogateTest)
{
    // 1101'11XX'XXXX'XXXX
    char32_t highSurrogateMask = 0xD800;
    for (char32_t value = 1; value <= 0x03FF; value <<= 1)
    {
        char32_t ch = highSurrogateMask | value;
        ASSERT_FALSE(dhorn::unicode::is_low_surrogate(ch));
    }

    char32_t lowSurrogateMask = 0xDC00;
    for (char32_t value = 1; value <= 0x03FF; value <<= 1)
    {
        char32_t ch = lowSurrogateMask | value;
        ASSERT_TRUE(dhorn::unicode::is_low_surrogate(ch));
    }

    for (char32_t ch = 0; ch < 256; ++ch)
    {
        ASSERT_FALSE(dhorn::unicode::is_low_surrogate(ch));
    }
}

// TODO? IsNonCharacter, IsPrivateUse, IsControlCode?

TEST(UnicodeEncodingTests, IsValidCodePointTest)
{
    for (char32_t ch = 1; ch < 0x11'0000; ch <<= 1)
    {
        ASSERT_TRUE(dhorn::unicode::is_valid_code_point(ch));
    }

    for (char32_t ch = 0x11'0000; ch; ch <<= 1)
    {
        ASSERT_FALSE(dhorn::unicode::is_valid_code_point(ch));
    }
}

template <typename Traits, typename CharTy, std::size_t Size>
void DoReadCodePointSizeTest(const CharTy (&str)[Size], std::size_t ExpectedSize = Size - 1)
{
    // NOTE: Minus one for null character
    ASSERT_EQ(ExpectedSize, Traits::code_point_size(str[0]));
}

template <typename CheckedTraits>
void DoInvalidWriteCodePointSizeTest()
{
    // Invalid writes are common among all encodings
    ASSERT_EQ(0_sz, CheckedTraits::code_point_size(U'\x0000D800'));
    ASSERT_EQ(0_sz, CheckedTraits::code_point_size(U'\x0000DBFF'));
    ASSERT_EQ(0_sz, CheckedTraits::code_point_size(U'\x0000DC00'));
    ASSERT_EQ(0_sz, CheckedTraits::code_point_size(U'\x0000DFFF'));
    ASSERT_EQ(0_sz, CheckedTraits::code_point_size(U'\x00110000'));
    ASSERT_EQ(0_sz, CheckedTraits::code_point_size(U'\xFFFFFFFF'));
}

template <typename Traits, typename CharTy, std::size_t Size>
void DoIsInitialCodeUnitTest(const CharTy (&str)[Size])
{
    ASSERT_TRUE(Traits::is_initial_code_unit(str[0]));

    for (std::size_t i = 1; i < Size - 1; ++i)
    {
        ASSERT_FALSE(Traits::is_initial_code_unit(str[i]));
    }

    // Null character should also be in initial code unit
    ASSERT_TRUE(Traits::is_initial_code_unit(str[Size - 1]));
}

template <typename Traits, typename CharTy, std::size_t Size>
void DoReadCodePointTest(const CharTy (&str)[Size], char32_t expected, std::size_t expectedBytesRead = Size - 1)
{
    auto [ch, ptr] = Traits::read(str);
    ASSERT_EQ(expected, ch);
    ASSERT_TRUE(ptr == (str + expectedBytesRead));
}

template <typename Traits, typename CharTy, std::size_t Size>
void DoWriteCodePointTest(char32_t ch, const CharTy (&expected)[Size], std::size_t expectedSize = Size - 1)
{
    typename Traits::value_type buffer[Traits::max_code_point_size];
    auto ptr = Traits::write(buffer, ch);

    ASSERT_EQ(expectedSize, static_cast<std::size_t>(std::distance(buffer, ptr)));
    for (std::size_t i = 0; i < expectedSize; ++i)
    {
        ASSERT_EQ(expected[i], buffer[i]);
    }
}

template <typename Traits, typename CharTy, std::size_t Size>
void DoLengthTest(
    const CharTy(&str)[Size],
    std::size_t expectedCodePoints,
    std::size_t expectedCodeUnits = Size - 1)
{
    auto result = Traits::length(str);
    ASSERT_EQ(expectedCodePoints, result.code_points);
    if (expectedCodePoints != dhorn::unicode::npos)
    {
        ASSERT_EQ(expectedCodeUnits, result.code_units);
    }
}

struct Utf8EncodingTraitsTests : testing::Test
{
    using traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_8>;
    using checked_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_8, true>;
};

TEST_F(Utf8EncodingTraitsTests, ReadCodePointSizeTest)
{
    // Just do the ones that are interesting...
    DoReadCodePointSizeTest<traits>(u8"", 1);
    DoReadCodePointSizeTest<checked_traits>(u8"", 1);
    DoReadCodePointSizeTest<traits>(u8"\u007F");
    DoReadCodePointSizeTest<checked_traits>(u8"\u007F");

    DoReadCodePointSizeTest<traits>(u8"\u0080");
    DoReadCodePointSizeTest<checked_traits>(u8"\u0080");
    DoReadCodePointSizeTest<traits>(u8"\u07FF");
    DoReadCodePointSizeTest<checked_traits>(u8"\u07FF");

    DoReadCodePointSizeTest<traits>(u8"\u0800");
    DoReadCodePointSizeTest<checked_traits>(u8"\u0800");
    DoReadCodePointSizeTest<traits>(u8"\uFFFF");
    DoReadCodePointSizeTest<checked_traits>(u8"\uFFFF");

    DoReadCodePointSizeTest<traits>(u8"\U00010000");
    DoReadCodePointSizeTest<checked_traits>(u8"\U00010000");
    DoReadCodePointSizeTest<traits>(u8"\U0010FFFF");
    DoReadCodePointSizeTest<checked_traits>(u8"\U0010FFFF");

    // At most the first four leading bits should be one
    ASSERT_EQ(0_sz, checked_traits::code_point_size('\xF8'));
    ASSERT_EQ(0_sz, checked_traits::code_point_size('\xFC'));
    ASSERT_EQ(0_sz, checked_traits::code_point_size('\xFE'));
    ASSERT_EQ(0_sz, checked_traits::code_point_size('\xFF'));

    // And it can't start with '10'
    ASSERT_EQ(0_sz, checked_traits::code_point_size('\x80'));
    ASSERT_EQ(0_sz, checked_traits::code_point_size('\xBF'));
}

TEST_F(Utf8EncodingTraitsTests, WriteCodePointSizeTest)
{
    // NOTE: Write size will always be the same, regardless of endianness, so no need to test excessively
    ASSERT_EQ(1_sz, checked_traits::code_point_size(U'\x00000000'));
    ASSERT_EQ(1_sz, checked_traits::code_point_size(U'\x0000007F'));

    ASSERT_EQ(2_sz, checked_traits::code_point_size(U'\x00000080'));
    ASSERT_EQ(2_sz, checked_traits::code_point_size(U'\x000007FF'));

    ASSERT_EQ(3_sz, checked_traits::code_point_size(U'\x00000800'));
    ASSERT_EQ(3_sz, checked_traits::code_point_size(U'\x0000FFFF'));

    ASSERT_EQ(4_sz, checked_traits::code_point_size(U'\x00010000'));
    ASSERT_EQ(4_sz, checked_traits::code_point_size(U'\x0010FFFF'));

    DoInvalidWriteCodePointSizeTest<checked_traits>();
}

TEST_F(Utf8EncodingTraitsTests, IsInitialCodeUnitTest)
{
    // Just do the ones that are interesting...
    DoIsInitialCodeUnitTest<traits>(u8"");
    DoIsInitialCodeUnitTest<checked_traits>(u8"");
    DoIsInitialCodeUnitTest<traits>(u8"\u007F");
    DoIsInitialCodeUnitTest<checked_traits>(u8"\u007F");

    DoIsInitialCodeUnitTest<traits>(u8"\u0080");
    DoIsInitialCodeUnitTest<checked_traits>(u8"\u0080");
    DoIsInitialCodeUnitTest<traits>(u8"\u07FF");
    DoIsInitialCodeUnitTest<checked_traits>(u8"\u07FF");

    DoIsInitialCodeUnitTest<traits>(u8"\u0800");
    DoIsInitialCodeUnitTest<checked_traits>(u8"\u0800");
    DoIsInitialCodeUnitTest<traits>(u8"\uFFFF");
    DoIsInitialCodeUnitTest<checked_traits>(u8"\uFFFF");

    DoIsInitialCodeUnitTest<traits>(u8"\U00010000");
    DoIsInitialCodeUnitTest<checked_traits>(u8"\U00010000");
    DoIsInitialCodeUnitTest<traits>(u8"\U0010FFFF");
    DoIsInitialCodeUnitTest<checked_traits>(u8"\U0010FFFF");
}

TEST_F(Utf8EncodingTraitsTests, ReadCodePointTest)
{
    DoReadCodePointTest<traits>(u8"", U'\u0000', 1);
    DoReadCodePointTest<checked_traits>(u8"", U'\u0000', 1);
    DoReadCodePointTest<traits>(u8"\u007F", U'\u007F');
    DoReadCodePointTest<checked_traits>(u8"\u007F", U'\u007F');

    DoReadCodePointTest<traits>(u8"\u0080", U'\u0080');
    DoReadCodePointTest<checked_traits>(u8"\u0080", U'\u0080');
    DoReadCodePointTest<traits>(u8"\u07FF", U'\u07FF');
    DoReadCodePointTest<checked_traits>(u8"\u07FF", U'\u07FF');

    DoReadCodePointTest<traits>(u8"\u0800", U'\u0800');
    DoReadCodePointTest<checked_traits>(u8"\u0800", U'\u0800');
    DoReadCodePointTest<traits>(u8"\uFFFF", U'\uFFFF');
    DoReadCodePointTest<checked_traits>(u8"\uFFFF", U'\uFFFF');

    DoReadCodePointTest<traits>(u8"\U00010000", U'\U00010000');
    DoReadCodePointTest<checked_traits>(u8"\U00010000", U'\U00010000');
    DoReadCodePointTest<traits>(u8"\U0010FFFF", U'\U0010FFFF');
    DoReadCodePointTest<checked_traits>(u8"\U0010FFFF", U'\U0010FFFF');

    // Invalid leading code units should not advance the pointer
    DoReadCodePointTest<checked_traits>("\xF8", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_traits>("\xFC", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_traits>("\xFE", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_traits>("\xFF", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_traits>("\x80", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_traits>("\xBF", dhorn::unicode::eof, 0);

    // We should stop when we encounter a character that doesn't start with '10' past the first character
    DoReadCodePointTest<checked_traits>("\xF7\xC0", dhorn::unicode::eof, 1);
    DoReadCodePointTest<checked_traits>("\xF0\xBF\x00", dhorn::unicode::eof, 2);
    DoReadCodePointTest<checked_traits>("\xF5\x80\xA0\x7F", dhorn::unicode::eof, 3);

    // UTF-8 can encode invalid characters. Unfortunately, we won't know about it until we're done reading the
    // whole code point
    DoReadCodePointTest<checked_traits>("\xED\xA0\x80", dhorn::unicode::eof, 3);
    DoReadCodePointTest<checked_traits>("\xED\xBF\xBF", dhorn::unicode::eof, 3);
}

TEST_F(Utf8EncodingTraitsTests, WriteCodePointTest)
{
    DoWriteCodePointTest<traits>(U'\u0000', u8"", 1);
    DoWriteCodePointTest<checked_traits>(U'\u0000', u8"", 1);
    DoWriteCodePointTest<traits>(U'\u007F', u8"\u007F");
    DoWriteCodePointTest<checked_traits>(U'\u007F', u8"\u007F");

    DoWriteCodePointTest<traits>(U'\u0080', u8"\u0080");
    DoWriteCodePointTest<checked_traits>(U'\u0080', u8"\u0080");
    DoWriteCodePointTest<traits>(U'\u07FF', u8"\u07FF");
    DoWriteCodePointTest<checked_traits>(U'\u07FF', u8"\u07FF");

    DoWriteCodePointTest<traits>(U'\u0800', u8"\u0800");
    DoWriteCodePointTest<checked_traits>(U'\u0800', u8"\u0800");
    DoWriteCodePointTest<traits>(U'\uFFFF', u8"\uFFFF");
    DoWriteCodePointTest<checked_traits>(U'\uFFFF', u8"\uFFFF");

    DoWriteCodePointTest<traits>(U'\U00010000', u8"\U00010000");
    DoWriteCodePointTest<checked_traits>(U'\U00010000', u8"\U00010000");
    DoWriteCodePointTest<traits>(U'\U0010FFFF', u8"\U0010FFFF");
    DoWriteCodePointTest<checked_traits>(U'\U0010FFFF', u8"\U0010FFFF");

    // Invalid characters should write no data
    DoWriteCodePointTest<checked_traits>(U'\xD800', u8"");
    DoWriteCodePointTest<checked_traits>(U'\xDFFF', u8"");
    DoWriteCodePointTest<checked_traits>(U'\x110000', u8"");
}

TEST_F(Utf8EncodingTraitsTests, LengthTest)
{
    DoLengthTest<traits>(u8"", 0);
    DoLengthTest<checked_traits>(u8"", 0);
    DoLengthTest<traits>(u8"\u007F", 1);
    DoLengthTest<checked_traits>(u8"\u007F", 1);

    DoLengthTest<traits>(u8"\u0080", 1);
    DoLengthTest<checked_traits>(u8"\u0080", 1);
    DoLengthTest<traits>(u8"\u07FF", 1);
    DoLengthTest<checked_traits>(u8"\u07FF", 1);

    DoLengthTest<traits>(u8"\u0800", 1);
    DoLengthTest<checked_traits>(u8"\u0800", 1);
    DoLengthTest<traits>(u8"\uFFFF", 1);
    DoLengthTest<checked_traits>(u8"\uFFFF", 1);

    DoLengthTest<traits>(u8"\U00010000", 1);
    DoLengthTest<checked_traits>(u8"\U00010000", 1);
    DoLengthTest<traits>(u8"\U0010FFFF", 1);
    DoLengthTest<checked_traits>(u8"\U0010FFFF", 1);

    // Invalid leading code units should give error
    DoLengthTest<checked_traits>("\xF8", dhorn::unicode::npos);
    DoLengthTest<checked_traits>("\xFC", dhorn::unicode::npos);
    DoLengthTest<checked_traits>("\xFE", dhorn::unicode::npos);
    DoLengthTest<checked_traits>("\xFF", dhorn::unicode::npos);
    DoLengthTest<checked_traits>("\x80", dhorn::unicode::npos);
    DoLengthTest<checked_traits>("\xBF", dhorn::unicode::npos);

    // Continuation bytes should start with '10'
    DoLengthTest<checked_traits>("\xF7\xC0", dhorn::unicode::npos);
    DoLengthTest<checked_traits>("\xF0\xBF\x00", dhorn::unicode::npos);
    DoLengthTest<checked_traits>("\xF5\x80\xA0\x7F", dhorn::unicode::npos);

    // UTF-8 can encode invalid characters, but we should detect that
    DoLengthTest<checked_traits>("\xED\xA0\x80", dhorn::unicode::npos);
    DoLengthTest<checked_traits>("\xED\xBF\xBF", dhorn::unicode::npos);
}

struct Utf16EncodingTraitsTests : testing::Test
{
    using traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_16>;
    using le_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_16le>;
    using be_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_16be>;

    using checked_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_16, true>;
    using checked_le_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_16le, true>;
    using checked_be_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_16be, true>;
};

TEST_F(Utf16EncodingTraitsTests, ReadCodePointSizeTest)
{
    // Just do the ones that are interesting...
    DoReadCodePointSizeTest<traits>(u"", 1);
    DoReadCodePointSizeTest<checked_traits>(u"", 1);
    DoReadCodePointSizeTest<traits>(u"\uFFFF");
    DoReadCodePointSizeTest<checked_traits>(u"\uFFFF");

    DoReadCodePointSizeTest<traits>(u"\U00010000");
    DoReadCodePointSizeTest<checked_traits>(u"\U00010000");
    DoReadCodePointSizeTest<traits>(u"\U0010FFFF");
    DoReadCodePointSizeTest<checked_traits>(u"\U0010FFFF");

    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    ASSERT_EQ(2_sz, le_traits::code_point_size(u'\xD800'));
    ASSERT_EQ(2_sz, checked_le_traits::code_point_size(u'\xD800'));
    ASSERT_EQ(2_sz, le_traits::code_point_size(u'\xDBFF'));
    ASSERT_EQ(2_sz, checked_le_traits::code_point_size(u'\xDBFF'));

    ASSERT_EQ(2_sz, be_traits::code_point_size(u'\x00D8'));
    ASSERT_EQ(2_sz, checked_be_traits::code_point_size(u'\x00D8'));
    ASSERT_EQ(2_sz, be_traits::code_point_size(u'\xFFDB'));
    ASSERT_EQ(2_sz, checked_be_traits::code_point_size(u'\xFFDB'));

    // The only invalid starts to utf-16 are low surrogates
    ASSERT_EQ(0_sz, checked_traits::code_point_size(u'\xDC00'));
    ASSERT_EQ(0_sz, checked_traits::code_point_size(u'\xDFFF'));

    ASSERT_EQ(0_sz, checked_le_traits::code_point_size(u'\xDC00'));
    ASSERT_EQ(0_sz, checked_le_traits::code_point_size(u'\xDFFF'));

    ASSERT_EQ(0_sz, checked_be_traits::code_point_size(u'\x00DC'));
    ASSERT_EQ(0_sz, checked_be_traits::code_point_size(u'\xFFDF'));
}

TEST_F(Utf16EncodingTraitsTests, WriteCodePointSizeTest)
{
    // NOTE: Write size will always be the same, regardless of endianness, so no need to test excessively
    ASSERT_EQ(1_sz, checked_traits::code_point_size(U'\x00000000'));
    ASSERT_EQ(1_sz, checked_traits::code_point_size(U'\x0000FFFF'));

    ASSERT_EQ(2_sz, checked_traits::code_point_size(U'\x00010000'));
    ASSERT_EQ(2_sz, checked_traits::code_point_size(U'\x0010FFFF'));

    DoInvalidWriteCodePointSizeTest<checked_traits>();
}

TEST_F(Utf16EncodingTraitsTests, IsInitialCodeUnitTest)
{
    // Just do the ones that are interesting...
    DoIsInitialCodeUnitTest<traits>(u"");
    DoIsInitialCodeUnitTest<checked_traits>(u"");
    DoIsInitialCodeUnitTest<traits>(u"\uFFFF");
    DoIsInitialCodeUnitTest<checked_traits>(u"\uFFFF");

    DoIsInitialCodeUnitTest<traits>(u"\U00010000");
    DoIsInitialCodeUnitTest<checked_traits>(u"\U00010000");
    DoIsInitialCodeUnitTest<traits>(u"\U0010FFFF");
    DoIsInitialCodeUnitTest<checked_traits>(u"\U0010FFFF");

    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    ASSERT_TRUE(le_traits::is_initial_code_unit(u'\xD800'));
    ASSERT_TRUE(checked_le_traits::is_initial_code_unit(u'\xD800'));
    ASSERT_TRUE(le_traits::is_initial_code_unit(u'\xDBFF'));
    ASSERT_TRUE(checked_le_traits::is_initial_code_unit(u'\xDBFF'));

    ASSERT_TRUE(be_traits::is_initial_code_unit(u'\x00D8'));
    ASSERT_TRUE(checked_be_traits::is_initial_code_unit(u'\x00D8'));
    ASSERT_TRUE(be_traits::is_initial_code_unit(u'\xFFDB'));
    ASSERT_TRUE(checked_be_traits::is_initial_code_unit(u'\xFFDB'));
}

TEST_F(Utf16EncodingTraitsTests, ReadCodePointTest)
{
    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    DoReadCodePointTest<le_traits>(u"", U'\u0000', 1);
    DoReadCodePointTest<checked_le_traits>(u"", U'\u0000', 1);
    DoReadCodePointTest<le_traits>(u"\uFFFF", U'\uFFFF');
    DoReadCodePointTest<checked_le_traits>(u"\uFFFF", U'\uFFFF');

    DoReadCodePointTest<be_traits>(u"", U'\u0000', 1);
    DoReadCodePointTest<checked_be_traits>(u"", U'\u0000', 1);
    DoReadCodePointTest<be_traits>(u"\uFFFF", U'\uFFFF');
    DoReadCodePointTest<checked_be_traits>(u"\uFFFF", U'\uFFFF');

    DoReadCodePointTest<le_traits>(u"\U00010000", U'\U00010000');
    DoReadCodePointTest<checked_le_traits>(u"\U00010000", U'\U00010000');
    DoReadCodePointTest<le_traits>(u"\U0010FFFF", U'\U0010FFFF');
    DoReadCodePointTest<checked_le_traits>(u"\U0010FFFF", U'\U0010FFFF');

    DoReadCodePointTest<be_traits>(u"\x00D8\x00DC", U'\U00010000');
    DoReadCodePointTest<checked_be_traits>(u"\x00D8\x00DC", U'\U00010000');
    DoReadCodePointTest<be_traits>(u"\xFFDB\xFFDF", U'\U0010FFFF');
    DoReadCodePointTest<checked_be_traits>(u"\xFFDB\xFFDF", U'\U0010FFFF');

    // Shouldn't read any characters if the first code unit is a low surrogate
    DoReadCodePointTest<checked_le_traits>(u"\xDC00", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_be_traits>(u"\xFFDF", dhorn::unicode::eof, 0);

    // Should ignore second character if it's not a low surrogate
    DoReadCodePointTest<checked_le_traits>(u"\xD800\x0000", dhorn::unicode::eof, 1);
    DoReadCodePointTest<checked_be_traits>(u"\x00D8\xFFDB", dhorn::unicode::eof, 1);
}

TEST_F(Utf16EncodingTraitsTests, WriteCodePointTest)
{
    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    DoWriteCodePointTest<le_traits>(U'\u0000', u"", 1);
    DoWriteCodePointTest<checked_le_traits>(U'\u0000', u"", 1);
    DoWriteCodePointTest<le_traits>(U'\uFFFF', u"\uFFFF");
    DoWriteCodePointTest<checked_le_traits>(U'\uFFFF', u"\uFFFF");

    DoWriteCodePointTest<be_traits>(U'\u0000', u"", 1);
    DoWriteCodePointTest<checked_be_traits>(U'\u0000', u"", 1);
    DoWriteCodePointTest<be_traits>(U'\uFFFF', u"\uFFFF");
    DoWriteCodePointTest<checked_be_traits>(U'\uFFFF', u"\uFFFF");

    DoWriteCodePointTest<le_traits>(U'\U00010000', u"\U00010000");
    DoWriteCodePointTest<checked_le_traits>(U'\U00010000', u"\U00010000");
    DoWriteCodePointTest<le_traits>(U'\U0010FFFF', u"\U0010FFFF");
    DoWriteCodePointTest<checked_le_traits>(U'\U0010FFFF', u"\U0010FFFF");

    DoWriteCodePointTest<be_traits>(U'\U00010000', u"\x00D8\x00DC");
    DoWriteCodePointTest<checked_be_traits>(U'\U00010000', u"\x00D8\x00DC");
    DoWriteCodePointTest<be_traits>(U'\U0010FFFF', u"\xFFDB\xFFDF");
    DoWriteCodePointTest<checked_be_traits>(U'\U0010FFFF', u"\xFFDB\xFFDF");

    // Invalid characters should write no data
    DoWriteCodePointTest<checked_le_traits>(U'\xD800', u"");
    DoWriteCodePointTest<checked_be_traits>(U'\xD800', u"");
    DoWriteCodePointTest<checked_le_traits>(U'\xDFFF', u"");
    DoWriteCodePointTest<checked_be_traits>(U'\xDFFF', u"");
    DoWriteCodePointTest<checked_le_traits>(U'\x110000', u"");
    DoWriteCodePointTest<checked_be_traits>(U'\x110000', u"");
}

TEST_F(Utf16EncodingTraitsTests, LengthTest)
{
    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    DoLengthTest<le_traits>(u"", 0);
    DoLengthTest<checked_le_traits>(u"", 0);
    DoLengthTest<le_traits>(u"\uFFFF", 1);
    DoLengthTest<checked_le_traits>(u"\uFFFF", 1);

    DoLengthTest<be_traits>(u"", 0);
    DoLengthTest<checked_be_traits>(u"", 0);
    DoLengthTest<be_traits>(u"\uFFFF", 1);
    DoLengthTest<checked_be_traits>(u"\uFFFF", 1);

    DoLengthTest<le_traits>(u"\U00010000", 1);
    DoLengthTest<checked_le_traits>(u"\U00010000", 1);
    DoLengthTest<le_traits>(u"\U0010FFFF", 1);
    DoLengthTest<checked_le_traits>(u"\U0010FFFF", 1);

    DoLengthTest<be_traits>(u"\x00D8\x00DC", 1);
    DoLengthTest<checked_be_traits>(u"\x00D8\x00DC", 1);
    DoLengthTest<be_traits>(u"\xFFDB\xFFDF", 1);
    DoLengthTest<checked_be_traits>(u"\xFFDB\xFFDF", 1);

    // Shouldn't read any characters if the first code unit is a low surrogate
    DoLengthTest<checked_le_traits>(u"\xDC00", dhorn::unicode::npos);
    DoLengthTest<checked_be_traits>(u"\xFFDF", dhorn::unicode::npos);

    // Should ignore second character if it's not a low surrogate
    DoLengthTest<checked_le_traits>(u"\xD800\x0000", dhorn::unicode::npos);
    DoLengthTest<checked_be_traits>(u"\x00D8\xFFDB", dhorn::unicode::npos);
}

struct Utf32EncodingTraitsTests : testing::Test
{
    using traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_32>;
    using le_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_32le>;
    using be_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_32be>;

    using checked_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_32, true>;
    using checked_le_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_32le, true>;
    using checked_be_traits = dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_32be, true>;
};

TEST_F(Utf32EncodingTraitsTests, ReadCodePointSizeTest)
{
    // Size is always one, so nothing is interesting here...
    DoReadCodePointSizeTest<traits>(U"", 1);
    DoReadCodePointSizeTest<checked_traits>(U"", 1);
    DoReadCodePointSizeTest<traits>(U"\U0010FFFF");
    DoReadCodePointSizeTest<checked_traits>(U"\U0010FFFF");

    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    ASSERT_EQ(1_sz, le_traits::code_point_size(U'\x0010FFFF'));
    ASSERT_EQ(1_sz, checked_le_traits::code_point_size(U'\x0010FFFF'));
    ASSERT_EQ(1_sz, be_traits::code_point_size(U'\xFFFF1000'));
    ASSERT_EQ(1_sz, checked_be_traits::code_point_size(U'\xFFFF1000'));

    // The only invalid utf-32 characters are those outside the range and low/high surrogates
    ASSERT_EQ(0_sz, checked_traits::code_point_size(U'\x0000D800'));
    ASSERT_EQ(0_sz, checked_traits::code_point_size(U'\x0000DBFF'));
    ASSERT_EQ(0_sz, checked_traits::code_point_size(U'\x0000DC00'));
    ASSERT_EQ(0_sz, checked_traits::code_point_size(U'\x0000DFFF'));
    ASSERT_EQ(0_sz, checked_traits::code_point_size(U'\x00110000'));

    ASSERT_EQ(0_sz, checked_le_traits::code_point_size(U'\x0000D800'));
    ASSERT_EQ(0_sz, checked_le_traits::code_point_size(U'\x0000DBFF'));
    ASSERT_EQ(0_sz, checked_le_traits::code_point_size(U'\x0000DC00'));
    ASSERT_EQ(0_sz, checked_le_traits::code_point_size(U'\x0000DFFF'));
    ASSERT_EQ(0_sz, checked_le_traits::code_point_size(U'\x00110000'));

    ASSERT_EQ(0_sz, checked_be_traits::code_point_size(U'\x00D80000'));
    ASSERT_EQ(0_sz, checked_be_traits::code_point_size(U'\xFFDB0000'));
    ASSERT_EQ(0_sz, checked_be_traits::code_point_size(U'\x00DC0000'));
    ASSERT_EQ(0_sz, checked_be_traits::code_point_size(U'\xFFDF0000'));
    ASSERT_EQ(0_sz, checked_be_traits::code_point_size(U'\x00001100'));
}

TEST_F(Utf32EncodingTraitsTests, WriteCodePointSizeTest)
{
    // NOTE: Write size will always be the same, regardless of endianness, so no need to test excessively
    ASSERT_EQ(1_sz, checked_traits::code_point_size(U'\x00000000'));
    ASSERT_EQ(1_sz, checked_traits::code_point_size(U'\x0010FFFF'));

    DoInvalidWriteCodePointSizeTest<checked_traits>();
}

TEST_F(Utf32EncodingTraitsTests, IsInitialCodeUnitTest)
{
    // Size is always one, so nothing is interesting here...
    DoIsInitialCodeUnitTest<traits>(U"");
    DoIsInitialCodeUnitTest<checked_traits>(U"");
    DoIsInitialCodeUnitTest<traits>(U"\U0010FFFF");
    DoIsInitialCodeUnitTest<checked_traits>(U"\U0010FFFF");

    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    ASSERT_TRUE(le_traits::is_initial_code_unit(U'\x0010FFFF'));
    ASSERT_TRUE(checked_le_traits::is_initial_code_unit(U'\x0010FFFF'));
    ASSERT_TRUE(be_traits::is_initial_code_unit(U'\xFFFF1000'));
    ASSERT_TRUE(checked_be_traits::is_initial_code_unit(U'\xFFFF1000'));
}

TEST_F(Utf32EncodingTraitsTests, ReadCodePointTest)
{
    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    DoReadCodePointTest<le_traits>(U"", U'\u0000', 1);
    DoReadCodePointTest<checked_le_traits>(U"", U'\u0000', 1);
    DoReadCodePointTest<le_traits>(U"\U0010FFFF", U'\U0010FFFF');
    DoReadCodePointTest<checked_le_traits>(U"\U0010FFFF", U'\U0010FFFF');

    DoReadCodePointTest<be_traits>(U"", U'\u0000', 1);
    DoReadCodePointTest<checked_be_traits>(U"", U'\u0000', 1);
    DoReadCodePointTest<be_traits>(U"\xFFFF1000", U'\U0010FFFF');
    DoReadCodePointTest<checked_be_traits>(U"\xFFFF1000", U'\U0010FFFF');

    // Shouldn't read any input when the character is not valid
    DoReadCodePointTest<checked_le_traits>(U"\x0000D800", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_be_traits>(U"\x00D80000", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_le_traits>(U"\x0000DFFF", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_be_traits>(U"\xFFDF0000", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_le_traits>(U"\x00110000", dhorn::unicode::eof, 0);
    DoReadCodePointTest<checked_be_traits>(U"\x00001100", dhorn::unicode::eof, 0);
}

TEST_F(Utf32EncodingTraitsTests, WriteCodePointTest)
{
    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    DoWriteCodePointTest<le_traits>(U'\u0000', U"", 1);
    DoWriteCodePointTest<checked_le_traits>(U'\u0000', U"", 1);
    DoWriteCodePointTest<le_traits>(U'\U0010FFFF', U"\U0010FFFF");
    DoWriteCodePointTest<checked_le_traits>(U'\U0010FFFF', U"\U0010FFFF");

    DoWriteCodePointTest<be_traits>(U'\u0000', U"", 1);
    DoWriteCodePointTest<checked_be_traits>(U'\u0000', U"", 1);
    DoWriteCodePointTest<be_traits>(U'\U0010FFFF', U"\xFFFF1000");
    DoWriteCodePointTest<checked_be_traits>(U'\U0010FFFF', U"\xFFFF1000");

    // Invalid characters should write no data
    DoWriteCodePointTest<checked_le_traits>(U'\xD800', U"");
    DoWriteCodePointTest<checked_be_traits>(U'\xD800', U"");
    DoWriteCodePointTest<checked_le_traits>(U'\xDFFF', U"");
    DoWriteCodePointTest<checked_be_traits>(U'\xDFFF', U"");
    DoWriteCodePointTest<checked_le_traits>(U'\x110000', U"");
    DoWriteCodePointTest<checked_be_traits>(U'\x110000', U"");
}

TEST_F(Utf32EncodingTraitsTests, LengthTest)
{
    // TODO: Assuming little endian for now...
    static_assert(std::is_same_v<traits, le_traits>);
    DoLengthTest<le_traits>(U"", 0);
    DoLengthTest<checked_le_traits>(U"", 0);
    DoLengthTest<le_traits>(U"\U0010FFFF", 1);
    DoLengthTest<checked_le_traits>(U"\U0010FFFF", 1);

    DoLengthTest<be_traits>(U"", 0);
    DoLengthTest<checked_be_traits>(U"", 0);
    DoLengthTest<be_traits>(U"\xFFFF1000", 1);
    DoLengthTest<checked_be_traits>(U"\xFFFF1000", 1);

    // Shouldn't read any input when the character is not valid
    DoLengthTest<checked_le_traits>(U"\x0000D800", dhorn::unicode::npos);
    DoLengthTest<checked_be_traits>(U"\x00D80000", dhorn::unicode::npos);
    DoLengthTest<checked_le_traits>(U"\x0000DFFF", dhorn::unicode::npos);
    DoLengthTest<checked_be_traits>(U"\xFFDF0000", dhorn::unicode::npos);
    DoLengthTest<checked_le_traits>(U"\x00110000", dhorn::unicode::npos);
    DoLengthTest<checked_be_traits>(U"\x00001100", dhorn::unicode::npos);
}
