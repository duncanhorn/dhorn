/*
 * Duncan Horn
 */

#include <algorithm>
#include <benchmark/benchmark.h>
#include <dhorn/utility.h>

namespace details
{
    template <typename IntSeq, std::size_t Size, typename IntSeq::value_type Value, typename = void>
    struct expand_seq;

    template <typename Ty, Ty... ExistingValues, std::size_t Size, Ty Value>
    struct expand_seq<std::integer_sequence<Ty, ExistingValues...>, Size, Value, std::enable_if_t<sizeof...(ExistingValues) == Size>>
    {
        using type = std::integer_sequence<Ty, ExistingValues...>;
    };

    template <typename Ty, Ty... ExistingValues, std::size_t Size, Ty Value>
    struct expand_seq<std::integer_sequence<Ty, ExistingValues...>, Size, Value, std::enable_if_t<sizeof...(ExistingValues) != Size>>
    {
        static_assert(sizeof...(ExistingValues) < Size);
        using type = typename expand_seq<std::integer_sequence<Ty, ExistingValues..., Value>, Size, Value>::type;
    };
}

template <typename IntSeq, std::size_t Size, typename IntSeq::value_type Value = 0>
struct expand_seq :
    public details::expand_seq<IntSeq, Size, Value>
{
};

template <typename IntSeq, std::size_t Size, typename IntSeq::value_type Value = 0>
using expand_seq_t = typename expand_seq<IntSeq, Size, Value>::type;



#pragma region Default Construction

template <std::size_t Size>
void VectorBaseline_DefaultConstruction_Int(benchmark::State& state)
{
    for (auto _ : state)
    {
        __m128i v0;
        benchmark::DoNotOptimize(v0);

        if constexpr (Size >= 2)
        {
            __m128i v1;
            benchmark::DoNotOptimize(v1);
        }

        if constexpr (Size >= 3)
        {
            __m128i v2;
            benchmark::DoNotOptimize(v2);
        }

        if constexpr (Size >= 4)
        {
            __m128i v3;
            benchmark::DoNotOptimize(v3);
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Int, 1);
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Int, 2);
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Int, 3);
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Int, 4);

template <std::size_t Size>
void VectorBaseline_DefaultConstruction_Float(benchmark::State& state)
{
    for (auto _ : state)
    {
        __m128 v0;
        benchmark::DoNotOptimize(v0);

        if constexpr (Size >= 2)
        {
            __m128 v1;
            benchmark::DoNotOptimize(v1);
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Float, 1);
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Float, 2);

template <std::size_t Size>
void VectorBaseline_DefaultConstruction_Double(benchmark::State& state)
{
    for (auto _ : state)
    {
        __m128d v0;
        benchmark::DoNotOptimize(v0);

        if constexpr (Size >= 2)
        {
            __m128d v1;
            benchmark::DoNotOptimize(v1);
        }

        if constexpr (Size >= 3)
        {
            __m128d v2;
            benchmark::DoNotOptimize(v2);
        }

        if constexpr (Size >= 4)
        {
            __m128d v3;
            benchmark::DoNotOptimize(v3);
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Double, 1);
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Double, 2);
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Double, 3);
BENCHMARK_TEMPLATE(VectorBaseline_DefaultConstruction_Double, 4);

#pragma endregion



#pragma region Constant Value Construction

template <char... Values>
__m128i VectorBaseline_ConstructConstantValues_Char_Helper(std::integer_sequence<char, Values...>)
{
    return _mm_setr_epi8(Values...);
}

template <std::size_t Size>
void VectorBaseline_ConstructConstantValues_Char(benchmark::State& state)
{
    for (auto _ : state)
    {
        using seq = expand_seq_t<std::make_integer_sequence<char, Size>, 16>;
        benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Char_Helper(seq{}));
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Char, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Char, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Char, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Char, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Char, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Char, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Char, 8);

template <short... Values>
__m128i VectorBaseline_ConstructConstantValues_Short_Helper(std::integer_sequence<short, Values...>)
{
    return _mm_setr_epi16(Values...);
}

template <std::size_t Size>
void VectorBaseline_ConstructConstantValues_Short(benchmark::State& state)
{
    for (auto _ : state)
    {
        using seq = expand_seq_t<std::make_integer_sequence<short, Size>, 8>;
        benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Short_Helper(seq{}));
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Short, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Short, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Short, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Short, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Short, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Short, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Short, 8);

template <std::int32_t... Values>
__m128i VectorBaseline_ConstructConstantValues_Int32_Helper(std::integer_sequence<std::int32_t, Values...>)
{
    return _mm_setr_epi32(Values...);
}

template <std::size_t Size>
void VectorBaseline_ConstructConstantValues_Int32(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(4), Size);
        constexpr auto SecondSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(4), Size - 4);

        using seq0 = expand_seq_t<std::make_integer_sequence<std::int32_t, FirstSize>, 4>;
        using seq1 = expand_seq_t<dhorn::increment_integer_sequence_t<std::make_integer_sequence<std::int32_t, SecondSize>, 4>, 4>;

        benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Int32_Helper(seq0{}));
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Int32_Helper(seq1{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int32, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int32, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int32, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int32, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int32, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int32, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int32, 8);

template <std::int64_t... Values>
__m128i VectorBaseline_ConstructConstantValues_Int64_Helper(std::integer_sequence<std::int64_t, Values...>)
{
    return _mm_set_epi64x(Values...);
}

template <std::size_t Size>
void VectorBaseline_ConstructConstantValues_Int64(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(2), Size);
        constexpr auto SecondSize = (Size <= 2) ? 0 : std::min(static_cast<std::size_t>(2), Size - 2);
        constexpr auto ThirdSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(2), Size - 4);
        constexpr auto FourthSize = (Size <= 6) ? 0 : std::min(static_cast<std::size_t>(2), Size - 6);

        using seq0 = dhorn::reverse_integer_sequence_t<expand_seq_t<std::make_integer_sequence<std::int64_t, FirstSize>, 2>>;
        using seq1 = dhorn::reverse_integer_sequence_t<expand_seq_t<dhorn::increment_integer_sequence_t<std::make_integer_sequence<std::int64_t, SecondSize>, 2>, 2>>;
        using seq2 = dhorn::reverse_integer_sequence_t<expand_seq_t<dhorn::increment_integer_sequence_t<std::make_integer_sequence<std::int64_t, ThirdSize>, 4>, 2>>;
        using seq3 = dhorn::reverse_integer_sequence_t<expand_seq_t<dhorn::increment_integer_sequence_t<std::make_integer_sequence<std::int64_t, FourthSize>, 6>, 2>>;

        benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Int64_Helper(seq0{}));
        if constexpr (Size > 2)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Int64_Helper(seq1{}));
        }
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Int64_Helper(seq2{}));
        }
        if constexpr (Size > 6)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Int64_Helper(seq3{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int64, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int64, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int64, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int64, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int64, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int64, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Int64, 8);

template <int... Values>
__m128 VectorBaseline_ConstructConstantValues_Float_Helper(std::integer_sequence<int, Values...>)
{
    return _mm_setr_ps(static_cast<float>(Values)...);
}

template <std::size_t Size>
void VectorBaseline_ConstructConstantValues_Float(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(4), Size);
        constexpr auto SecondSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(4), Size - 4);

        using seq0 = expand_seq_t<std::make_integer_sequence<int, FirstSize>, 4>;
        using seq1 = expand_seq_t<dhorn::increment_integer_sequence_t<std::make_integer_sequence<int, SecondSize>, 4>, 4>;

        benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Float_Helper(seq0{}));
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Float_Helper(seq1{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Float, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Float, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Float, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Float, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Float, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Float, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Float, 8);

template <int... Values>
__m128d VectorBaseline_ConstructConstantValues_Double_Helper(std::integer_sequence<int, Values...>)
{
    return _mm_setr_pd(static_cast<double>(Values)...);
}

template <std::size_t Size>
void VectorBaseline_ConstructConstantValues_Double(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(2), Size);
        constexpr auto SecondSize = (Size <= 2) ? 0 : std::min(static_cast<std::size_t>(2), Size - 2);
        constexpr auto ThirdSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(2), Size - 4);
        constexpr auto FourthSize = (Size <= 6) ? 0 : std::min(static_cast<std::size_t>(2), Size - 6);

        using seq0 = expand_seq_t<std::make_integer_sequence<int, FirstSize>, 2>;
        using seq1 = expand_seq_t<dhorn::increment_integer_sequence_t<std::make_integer_sequence<int, SecondSize>, 2>, 2>;
        using seq2 = expand_seq_t<dhorn::increment_integer_sequence_t<std::make_integer_sequence<int, ThirdSize>, 4>, 2>;
        using seq3 = expand_seq_t<dhorn::increment_integer_sequence_t<std::make_integer_sequence<int, FourthSize>, 6>, 2>;

        benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Double_Helper(seq0{}));
        if constexpr (Size > 2)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Double_Helper(seq1{}));
        }
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Double_Helper(seq2{}));
        }
        if constexpr (Size > 6)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructConstantValues_Double_Helper(seq3{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Double, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Double, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Double, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Double, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Double, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Double, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructConstantValues_Double, 8);

#pragma endregion



#pragma region Non-Constant Value Construction

template <std::size_t... Indices, char... Zero>
__m128i VectorBaseline_ConstructNonConstantValues_Char_Helper(
    std::index_sequence<Indices...>,
    std::integer_sequence<char, Zero...>)
{
    static volatile char values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    return _mm_setr_epi8(values[Indices]..., Zero...);
}

template <std::size_t Size>
void VectorBaseline_ConstructNonConstantValues_Char(benchmark::State& state)
{
    for (auto _ : state)
    {
        using seq = std::make_index_sequence<Size>;
        using zero = expand_seq_t<std::integer_sequence<char>, 16 - Size>;
        benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Char_Helper(seq{}, zero{}));
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Char, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Char, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Char, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Char, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Char, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Char, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Char, 8);

template <std::size_t... Indices, short... Zero>
__m128i VectorBaseline_ConstructNonConstantValues_Short_Helper(
    std::index_sequence<Indices...>,
    std::integer_sequence<short, Zero...>)
{
    static volatile short values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    return _mm_setr_epi16(values[Indices]..., Zero...);
}

template <std::size_t Size>
void VectorBaseline_ConstructNonConstantValues_Short(benchmark::State& state)
{
    for (auto _ : state)
    {
        using seq = std::make_index_sequence<Size>;
        using zero = expand_seq_t<std::integer_sequence<short>, 8 - Size>;
        benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Short_Helper(seq{}, zero{}));
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Short, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Short, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Short, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Short, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Short, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Short, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Short, 8);

template <std::size_t... Indices, std::int32_t... Zero>
__m128i VectorBaseline_ConstructNonConstantValues_Int32_Helper(
    std::index_sequence<Indices...>,
    std::integer_sequence<std::int32_t, Zero...>)
{
    static volatile std::int32_t values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    return _mm_setr_epi32(values[Indices]..., Zero...);
}

template <std::size_t Size>
void VectorBaseline_ConstructNonConstantValues_Int32(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(4), Size);
        constexpr auto SecondSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(4), Size - 4);

        using seq0 = std::make_index_sequence<FirstSize>;
        using seq1 = dhorn::increment_integer_sequence_t<std::make_index_sequence<SecondSize>, 4>;

        using zero0 = expand_seq_t<std::integer_sequence<std::int32_t>, 4 - FirstSize>;
        using zero1 = expand_seq_t<std::integer_sequence<std::int32_t>, 4 - SecondSize>;

        benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Int32_Helper(seq0{}, zero0{}));
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Int32_Helper(seq1{}, zero1{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int32, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int32, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int32, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int32, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int32, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int32, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int32, 8);

template <std::size_t... Indices, std::int64_t... Zero>
__m128i VectorBaseline_ConstructNonConstantValues_Int64_Helper(
    std::index_sequence<Indices...>,
    std::integer_sequence<std::int64_t, Zero...>)
{
    static volatile std::int64_t values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    return _mm_set_epi64x(Zero..., values[Indices]...);
}

template <std::size_t Size>
void VectorBaseline_ConstructNonConstantValues_Int64(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(2), Size);
        constexpr auto SecondSize = (Size <= 2) ? 0 : std::min(static_cast<std::size_t>(2), Size - 2);
        constexpr auto ThirdSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(2), Size - 4);
        constexpr auto FourthSize = (Size <= 6) ? 0 : std::min(static_cast<std::size_t>(2), Size - 6);

        using seq0 = dhorn::make_reverse_index_sequence<FirstSize>;
        using seq1 = dhorn::increment_integer_sequence_t<dhorn::make_reverse_index_sequence<SecondSize>, 2>;
        using seq2 = dhorn::increment_integer_sequence_t<dhorn::make_reverse_index_sequence<ThirdSize>, 4>;
        using seq3 = dhorn::increment_integer_sequence_t<dhorn::make_reverse_index_sequence<FourthSize>, 6>;

        using zero0 = expand_seq_t<std::integer_sequence<std::int64_t>, 2 - FirstSize>;
        using zero1 = expand_seq_t<std::integer_sequence<std::int64_t>, 2 - SecondSize>;
        using zero2 = expand_seq_t<std::integer_sequence<std::int64_t>, 2 - ThirdSize>;
        using zero3 = expand_seq_t<std::integer_sequence<std::int64_t>, 2 - FourthSize>;

        benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Int64_Helper(seq0{}, zero0{}));
        if constexpr (Size > 2)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Int64_Helper(seq1{}, zero1{}));
        }
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Int64_Helper(seq2{}, zero2{}));
        }
        if constexpr (Size > 6)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Int64_Helper(seq3{}, zero3{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int64, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int64, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int64, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int64, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int64, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int64, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Int64, 8);

template <std::size_t... Indices, int... Zero>
__m128 VectorBaseline_ConstructNonConstantValues_Float_Helper(
    std::index_sequence<Indices...>,
    std::integer_sequence<int, Zero...>)
{
    static volatile float values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    return _mm_setr_ps(values[Indices]..., Zero...);
}

template <std::size_t Size>
void VectorBaseline_ConstructNonConstantValues_Float(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(4), Size);
        constexpr auto SecondSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(4), Size - 4);

        using seq0 = std::make_index_sequence<FirstSize>;
        using seq1 = dhorn::increment_integer_sequence_t<std::make_index_sequence<SecondSize>, 4>;

        using zero0 = expand_seq_t<std::integer_sequence<int>, 4 - FirstSize>;
        using zero1 = expand_seq_t<std::integer_sequence<int>, 4 - SecondSize>;

        benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Float_Helper(seq0{}, zero0{}));
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Float_Helper(seq1{}, zero1{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Float, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Float, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Float, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Float, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Float, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Float, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Float, 8);

template <std::size_t... Indices, int... Zero>
__m128d VectorBaseline_ConstructNonConstantValues_Double_Helper(
    std::index_sequence<Indices...>,
    std::integer_sequence<int, Zero...>)
{
    static volatile double values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    return _mm_setr_pd(values[Indices]..., Zero...);
}

template <std::size_t Size>
void VectorBaseline_ConstructNonConstantValues_Double(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(2), Size);
        constexpr auto SecondSize = (Size <= 2) ? 0 : std::min(static_cast<std::size_t>(2), Size - 2);
        constexpr auto ThirdSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(2), Size - 4);
        constexpr auto FourthSize = (Size <= 6) ? 0 : std::min(static_cast<std::size_t>(2), Size - 6);

        using seq0 = std::make_index_sequence<FirstSize>;
        using seq1 = dhorn::increment_integer_sequence_t<std::make_index_sequence<SecondSize>, 2>;
        using seq2 = dhorn::increment_integer_sequence_t<std::make_index_sequence<ThirdSize>, 4>;
        using seq3 = dhorn::increment_integer_sequence_t<std::make_index_sequence<FourthSize>, 6>;

        using zero0 = expand_seq_t<std::integer_sequence<int>, 2 - FirstSize>;
        using zero1 = expand_seq_t<std::integer_sequence<int>, 2 - SecondSize>;
        using zero2 = expand_seq_t<std::integer_sequence<int>, 2 - ThirdSize>;
        using zero3 = expand_seq_t<std::integer_sequence<int>, 2 - FourthSize>;

        benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Double_Helper(seq0{}, zero0{}));
        if constexpr (Size > 2)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Double_Helper(seq1{}, zero1{}));
        }
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Double_Helper(seq2{}, zero2{}));
        }
        if constexpr (Size > 6)
        {
            benchmark::DoNotOptimize(VectorBaseline_ConstructNonConstantValues_Double_Helper(seq3{}, zero3{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Double, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Double, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Double, 4);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Double, 5);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Double, 6);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Double, 7);
BENCHMARK_TEMPLATE(VectorBaseline_ConstructNonConstantValues_Double, 8);

#pragma endregion



#pragma region Zero Construct

template <std::size_t Size>
void VectorBaseline_ZeroConstruction_Int(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(_mm_setzero_si128());

        if constexpr (Size >= 2)
        {
            benchmark::DoNotOptimize(_mm_setzero_si128());
        }

        if constexpr (Size >= 3)
        {
            benchmark::DoNotOptimize(_mm_setzero_si128());
        }

        if constexpr (Size >= 4)
        {
            benchmark::DoNotOptimize(_mm_setzero_si128());
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Int, 1);
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Int, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Int, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Int, 4);

template <std::size_t Size>
void VectorBaseline_ZeroConstruction_Float(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(_mm_setzero_ps());

        if constexpr (Size >= 2)
        {
            benchmark::DoNotOptimize(_mm_setzero_ps());
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Float, 1);
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Float, 2);

template <std::size_t Size>
void VectorBaseline_ZeroConstruction_Double(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(_mm_setzero_pd());

        if constexpr (Size >= 2)
        {
            benchmark::DoNotOptimize(_mm_setzero_pd());
        }

        if constexpr (Size >= 3)
        {
            benchmark::DoNotOptimize(_mm_setzero_pd());
        }

        if constexpr (Size >= 4)
        {
            benchmark::DoNotOptimize(_mm_setzero_pd());
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Double, 1);
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Double, 2);
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Double, 3);
BENCHMARK_TEMPLATE(VectorBaseline_ZeroConstruction_Double, 4);

#pragma endregion



#pragma region Splat Constant

template <char Value, char... Values, char... Zero>
__m128i VectorBaseline_SplatConstant_Char_Helper(
    std::integer_sequence<char, Value, Values...>,
    std::integer_sequence<char, Zero...>)
{
    if constexpr (sizeof...(Zero) == 0)
    {
        return _mm_set1_epi8(Value);
    }
    else
    {
        return _mm_setr_epi8(Value, Values..., Zero...);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatConstant_Char(benchmark::State& state)
{
    for (auto _ : state)
    {
        using seq = expand_seq_t<std::integer_sequence<char>, Size, 42>;
        using zero = expand_seq_t<std::integer_sequence<char>, 16 - Size>;
        benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Char_Helper(seq{}, zero{}));
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Char, 2);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Char, 3);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Char, 4);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Char, 5);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Char, 6);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Char, 7);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Char, 8);

template <short Value, short... Values, short... Zero>
__m128i VectorBaseline_SplatConstant_Short_Helper(
    std::integer_sequence<short, Value, Values...>,
    std::integer_sequence<short, Zero...>)
{
    if constexpr (sizeof...(Zero) == 0)
    {
        return _mm_set1_epi16(Value);
    }
    else
    {
        return _mm_setr_epi16(Value, Values..., Zero...);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatConstant_Short(benchmark::State& state)
{
    for (auto _ : state)
    {
        using seq = expand_seq_t<std::integer_sequence<short>, Size, 42>;
        using zero = expand_seq_t<std::integer_sequence<short>, 8 - Size>;
        benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Short_Helper(seq{}, zero{}));
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Short, 2);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Short, 3);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Short, 4);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Short, 5);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Short, 6);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Short, 7);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Short, 8);

template <std::int32_t Value, std::int32_t... Values, std::int32_t... Zero>
__m128i VectorBaseline_SplatConstant_Int32_Helper(
    std::integer_sequence<std::int32_t, Value, Values...>,
    std::integer_sequence<std::int32_t, Zero...>)
{
    if constexpr (sizeof...(Zero) == 0)
    {
        return _mm_set1_epi32(Value);
    }
    else
    {
        return _mm_setr_epi32(Value, Values..., Zero...);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatConstant_Int32(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(4), Size);
        constexpr auto SecondSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(4), Size - 4);

        using seq0 = expand_seq_t<std::integer_sequence<std::int32_t>, FirstSize, 42>;
        using seq1 = expand_seq_t<std::integer_sequence<std::int32_t>, SecondSize, 42>;

        using zero0 = expand_seq_t<std::integer_sequence<std::int32_t>, 4 - FirstSize>;
        using zero1 = expand_seq_t<std::integer_sequence<std::int32_t>, 4 - SecondSize>;

        benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Int32_Helper(seq0{}, zero0{}));
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Int32_Helper(seq1{}, zero1{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int32, 2);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int32, 3);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int32, 4);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int32, 5);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int32, 6);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int32, 7);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int32, 8);

template <std::int64_t Value, std::int64_t... Values, std::int64_t... Zero>
__m128i VectorBaseline_SplatConstant_Int64_Helper(
    std::integer_sequence<std::int64_t, Value, Values...>,
    std::integer_sequence<std::int64_t, Zero...>)
{
    if constexpr (sizeof...(Zero) == 0)
    {
        return _mm_set1_epi64x(Value);
    }
    else
    {
        return _mm_set_epi64x(Zero..., Value, Values...);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatConstant_Int64(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(2), Size);
        constexpr auto SecondSize = (Size <= 2) ? 0 : std::min(static_cast<std::size_t>(2), Size - 2);
        constexpr auto ThirdSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(2), Size - 4);
        constexpr auto FourthSize = (Size <= 6) ? 0 : std::min(static_cast<std::size_t>(2), Size - 6);

        using seq0 = expand_seq_t<std::integer_sequence<std::int64_t>, FirstSize, 42>;
        using seq1 = expand_seq_t<std::integer_sequence<std::int64_t>, SecondSize, 42>;
        using seq2 = expand_seq_t<std::integer_sequence<std::int64_t>, ThirdSize, 42>;
        using seq3 = expand_seq_t<std::integer_sequence<std::int64_t>, FourthSize, 42>;

        using zero0 = expand_seq_t<std::integer_sequence<std::int64_t>, 2 - FirstSize>;
        using zero1 = expand_seq_t<std::integer_sequence<std::int64_t>, 2 - SecondSize>;
        using zero2 = expand_seq_t<std::integer_sequence<std::int64_t>, 2 - ThirdSize>;
        using zero3 = expand_seq_t<std::integer_sequence<std::int64_t>, 2 - FourthSize>;

        benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Int64_Helper(seq0{}, zero0{}));
        if constexpr (Size > 2)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Int64_Helper(seq1{}, zero1{}));
        }
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Int64_Helper(seq2{}, zero2{}));
        }
        if constexpr (Size > 6)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Int64_Helper(seq3{}, zero3{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int64, 2);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int64, 3);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int64, 4);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int64, 5);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int64, 6);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int64, 7);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Int64, 8);

template <int Value, int... Values, int... Zero>
__m128 VectorBaseline_SplatConstant_Float_Helper(
    std::integer_sequence<int, Value, Values...>,
    std::integer_sequence<int, Zero...>)
{
    if constexpr (sizeof...(Zero) == 0)
    {
        return _mm_set1_ps(Value);
    }
    else
    {
        return _mm_setr_ps(Value, Values..., Zero...);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatConstant_Float(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(4), Size);
        constexpr auto SecondSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(4), Size - 4);

        using seq0 = expand_seq_t<std::integer_sequence<int>, FirstSize, 42>;
        using seq1 = expand_seq_t<std::integer_sequence<int>, SecondSize, 42>;

        using zero0 = expand_seq_t<std::integer_sequence<int>, 4 - FirstSize>;
        using zero1 = expand_seq_t<std::integer_sequence<int>, 4 - SecondSize>;

        benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Float_Helper(seq0{}, zero0{}));
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Float_Helper(seq1{}, zero1{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Float, 2);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Float, 3);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Float, 4);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Float, 5);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Float, 6);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Float, 7);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Float, 8);

template <int Value, int... Values, int... Zero>
__m128d VectorBaseline_SplatConstant_Double_Helper(
    std::integer_sequence<int, Value, Values...>,
    std::integer_sequence<int, Zero...>)
{
    if constexpr (sizeof...(Zero) == 0)
    {
        return _mm_set1_pd(Value);
    }
    else
    {
        return _mm_setr_pd(Value, Values..., Zero...);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatConstant_Double(benchmark::State& state)
{
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(2), Size);
        constexpr auto SecondSize = (Size <= 2) ? 0 : std::min(static_cast<std::size_t>(2), Size - 2);
        constexpr auto ThirdSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(2), Size - 4);
        constexpr auto FourthSize = (Size <= 6) ? 0 : std::min(static_cast<std::size_t>(2), Size - 6);

        using seq0 = expand_seq_t<std::integer_sequence<int>, FirstSize, 42>;
        using seq1 = expand_seq_t<std::integer_sequence<int>, SecondSize, 42>;
        using seq2 = expand_seq_t<std::integer_sequence<int>, ThirdSize, 42>;
        using seq3 = expand_seq_t<std::integer_sequence<int>, FourthSize, 42>;

        using zero0 = expand_seq_t<std::integer_sequence<int>, 2 - FirstSize>;
        using zero1 = expand_seq_t<std::integer_sequence<int>, 2 - SecondSize>;
        using zero2 = expand_seq_t<std::integer_sequence<int>, 2 - ThirdSize>;
        using zero3 = expand_seq_t<std::integer_sequence<int>, 2 - FourthSize>;

        benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Double_Helper(seq0{}, zero0{}));
        if constexpr (Size > 2)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Double_Helper(seq1{}, zero1{}));
        }
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Double_Helper(seq2{}, zero2{}));
        }
        if constexpr (Size > 6)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatConstant_Double_Helper(seq3{}, zero3{}));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Double, 2);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Double, 3);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Double, 4);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Double, 5);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Double, 6);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Double, 7);
BENCHMARK_TEMPLATE(VectorBaseline_SplatConstant_Double, 8);

#pragma endregion



#pragma region Splat Non-Constant

template <std::size_t Size>
void VectorBaseline_SplatNonConstant_Char(benchmark::State& state)
{
    const auto value = static_cast<char>(state.range(0));
    for (auto _ : state)
    {
        if constexpr (Size == 16)
        {
            benchmark::DoNotOptimize(_mm_set1_epi8(value));
        }
        else
        {
            benchmark::DoNotOptimize(_mm_setr_epi8(
                (Size >= 1) ? value : 0,
                (Size >= 2) ? value : 0,
                (Size >= 3) ? value : 0,
                (Size >= 4) ? value : 0,
                (Size >= 5) ? value : 0,
                (Size >= 6) ? value : 0,
                (Size >= 7) ? value : 0,
                (Size >= 8) ? value : 0,
                (Size >= 9) ? value : 0,
                (Size >= 10) ? value : 0,
                (Size >= 11) ? value : 0,
                (Size >= 12) ? value : 0,
                (Size >= 13) ? value : 0,
                (Size >= 14) ? value : 0,
                (Size >= 15) ? value : 0,
                (Size >= 16) ? value : 0));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Char, 2)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Char, 3)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Char, 4)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Char, 5)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Char, 6)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Char, 7)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Char, 8)->Arg(1);

template <std::size_t Size>
void VectorBaseline_SplatNonConstant_Short(benchmark::State& state)
{
    const auto value = static_cast<short>(state.range(0));
    for (auto _ : state)
    {
        if constexpr (Size == 8)
        {
            benchmark::DoNotOptimize(_mm_set1_epi16(value));
        }
        else
        {
            benchmark::DoNotOptimize(_mm_setr_epi16(
                (Size >= 1) ? value : 0,
                (Size >= 2) ? value : 0,
                (Size >= 3) ? value : 0,
                (Size >= 4) ? value : 0,
                (Size >= 5) ? value : 0,
                (Size >= 6) ? value : 0,
                (Size >= 7) ? value : 0,
                (Size >= 8) ? value : 0));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Short, 2)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Short, 3)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Short, 4)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Short, 5)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Short, 6)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Short, 7)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Short, 8)->Arg(1);

template <std::size_t Size>
__m128i VectorBaseline_SplatNonConstant_Int32_Helper(std::int32_t value)
{
    if constexpr (Size == 4)
    {
        return _mm_set1_epi32(value);
    }
    else
    {
        return _mm_setr_epi32(
            (Size >= 1) ? value : 0,
            (Size >= 2) ? value : 0,
            (Size >= 3) ? value : 0,
            (Size >= 4) ? value : 0);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatNonConstant_Int32(benchmark::State& state)
{
    const auto value = state.range(0);
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(4), Size);
        constexpr auto SecondSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(4), Size - 4);

        benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Int32_Helper<FirstSize>(value));
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Int32_Helper<SecondSize>(value));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int32, 2)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int32, 3)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int32, 4)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int32, 5)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int32, 6)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int32, 7)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int32, 8)->Arg(1);

template <std::size_t Size>
__m128i VectorBaseline_SplatNonConstant_Int64_Helper(std::int64_t value)
{
    if constexpr (Size == 2)
    {
        return _mm_set1_epi64x(value);
    }
    else
    {
        return _mm_set_epi64x(
            (Size >= 2) ? value : 0,
            (Size >= 1) ? value : 0);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatNonConstant_Int64(benchmark::State& state)
{
    const auto value = state.range(0);
    for (auto _ : state)
    {
        constexpr std::size_t FirstSize = std::min(static_cast<std::size_t>(2), Size);
        constexpr std::size_t SecondSize = (Size <= 2) ? 0 : std::min(static_cast<std::size_t>(2), Size - 2);
        constexpr std::size_t ThirdSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(2), Size - 4);
        constexpr std::size_t FourthSize = (Size <= 6) ? 0 : std::min(static_cast<std::size_t>(2), Size - 6);

        benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Int64_Helper<FirstSize>(value));
        if constexpr (Size > 2)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Int64_Helper<SecondSize>(value));
        }
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Int64_Helper<ThirdSize>(value));
        }
        if constexpr (Size > 6)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Int64_Helper<FourthSize>(value));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int64, 2)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int64, 3)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int64, 4)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int64, 5)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int64, 6)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int64, 7)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Int64, 8)->Arg(1);

template <std::size_t Size>
__m128 VectorBaseline_SplatNonConstant_Float_Helper(float value)
{
    if constexpr (Size == 4)
    {
        return _mm_set1_ps(value);
    }
    else
    {
        return _mm_setr_ps(
            (Size >= 1) ? value : 0,
            (Size >= 2) ? value : 0,
            (Size >= 3) ? value : 0,
            (Size >= 4) ? value : 0);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatNonConstant_Float(benchmark::State& state)
{
    const auto value = static_cast<float>(state.range(0));
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(4), Size);
        constexpr auto SecondSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(4), Size - 4);

        benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Float_Helper<FirstSize>(value));
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Float_Helper<SecondSize>(value));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Float, 2)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Float, 3)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Float, 4)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Float, 5)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Float, 6)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Float, 7)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Float, 8)->Arg(1);

template <std::size_t Size>
__m128d VectorBaseline_SplatNonConstant_Double_Helper(double value)
{
    if constexpr (Size == 2)
    {
        return _mm_set1_pd(value);
    }
    else
    {
        return _mm_setr_pd(
            (Size >= 1) ? value : 0,
            (Size >= 2) ? value : 0);
    }
}

template <std::size_t Size>
void VectorBaseline_SplatNonConstant_Double(benchmark::State& state)
{
    const auto value = static_cast<double>(state.range(0));
    for (auto _ : state)
    {
        constexpr auto FirstSize = std::min(static_cast<std::size_t>(2), Size);
        constexpr auto SecondSize = (Size <= 2) ? 0 : std::min(static_cast<std::size_t>(2), Size - 2);
        constexpr auto ThirdSize = (Size <= 4) ? 0 : std::min(static_cast<std::size_t>(2), Size - 4);
        constexpr auto FourthSize = (Size <= 6) ? 0 : std::min(static_cast<std::size_t>(2), Size - 6);

        benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Double_Helper<FirstSize>(value));
        if constexpr (Size > 2)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Double_Helper<SecondSize>(value));
        }
        if constexpr (Size > 4)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Double_Helper<ThirdSize>(value));
        }
        if constexpr (Size > 6)
        {
            benchmark::DoNotOptimize(VectorBaseline_SplatNonConstant_Double_Helper<FourthSize>(value));
        }
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Double, 2)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Double, 3)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Double, 4)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Double, 5)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Double, 6)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Double, 7)->Arg(1);
BENCHMARK_TEMPLATE(VectorBaseline_SplatNonConstant_Double, 8)->Arg(1);

#pragma endregion



#pragma region Addition

template <std::size_t Size>
void VectorBaseline_Addition_Char(benchmark::State& state)
{
    const auto c = _mm_set1_epi8(1);
    auto v = _mm_set1_epi8(1);
    for (auto _ : state)
    {
        v = _mm_add_epi8(v, c);
    }

    benchmark::DoNotOptimize(v);
}
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Char, 1);

template <std::size_t Size>
void VectorBaseline_Addition_Short(benchmark::State& state)
{
    const auto c = _mm_set1_epi16(1);
    auto v = _mm_set1_epi16(1);
    for (auto _ : state)
    {
        v = _mm_add_epi16(v, c);
    }

    benchmark::DoNotOptimize(v);
}
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Short, 1);

template <std::size_t Size>
void VectorBaseline_Addition_Int32(benchmark::State& state)
{
    const auto c = _mm_set1_epi32(1);
    auto v0 = _mm_set1_epi32(1);
    auto v1 = _mm_set1_epi32(2);
    for (auto _ : state)
    {
        v0 = _mm_add_epi32(v0, c);
        if constexpr (Size >= 2)
        {
            v1 = _mm_add_epi32(v1, c);
        }
    }

    benchmark::DoNotOptimize(v0);
    if constexpr (Size >= 2)
    {
        benchmark::DoNotOptimize(v1);
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Int32, 1);
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Int32, 2);

template <std::size_t Size>
void VectorBaseline_Addition_Int64(benchmark::State& state)
{
    const auto c = _mm_set1_epi64x(1);
    auto v0 = _mm_set1_epi64x(1);
    auto v1 = _mm_set1_epi64x(2);
    auto v2 = _mm_set1_epi64x(3);
    auto v3 = _mm_set1_epi64x(4);
    for (auto _ : state)
    {
        v0 = _mm_add_epi64(v0, c);
        if constexpr (Size >= 2)
        {
            v1 = _mm_add_epi64(v1, c);
        }
        if constexpr (Size >= 3)
        {
            v2 = _mm_add_epi64(v2, c);
        }
        if constexpr (Size >= 4)
        {
            v3 = _mm_add_epi64(v3, c);
        }
    }

    benchmark::DoNotOptimize(v0);
    if constexpr (Size >= 2)
    {
        benchmark::DoNotOptimize(v1);
    }
    if constexpr (Size >= 3)
    {
        benchmark::DoNotOptimize(v2);
    }
    if constexpr (Size >= 4)
    {
        benchmark::DoNotOptimize(v3);
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Int64, 1);
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Int64, 2);
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Int64, 3);
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Int64, 4);

template <std::size_t Size>
void VectorBaseline_Addition_Float(benchmark::State& state)
{
    const auto c = _mm_set1_ps(1);
    auto v0 = _mm_set1_ps(1);
    auto v1 = _mm_set1_ps(2);
    for (auto _ : state)
    {
        v0 = _mm_add_ps(v0, c);
        if constexpr (Size >= 2)
        {
            v1 = _mm_add_ps(v1, c);
        }
    }

    benchmark::DoNotOptimize(v0);
    if constexpr (Size >= 2)
    {
        benchmark::DoNotOptimize(v1);
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Float, 1);
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Float, 2);

template <std::size_t Size>
void VectorBaseline_Addition_Double(benchmark::State& state)
{
    const auto c = _mm_set1_pd(1);
    auto v0 = _mm_set1_pd(1);
    auto v1 = _mm_set1_pd(2);
    auto v2 = _mm_set1_pd(3);
    auto v3 = _mm_set1_pd(4);
    for (auto _ : state)
    {
        v0 = _mm_add_pd(v0, c);
        if constexpr (Size >= 2)
        {
            v1 = _mm_add_pd(v1, c);
        }
        if constexpr (Size >= 3)
        {
            v2 = _mm_add_pd(v2, c);
        }
        if constexpr (Size >= 4)
        {
            v3 = _mm_add_pd(v3, c);
        }
    }

    benchmark::DoNotOptimize(v0);
    if constexpr (Size >= 2)
    {
        benchmark::DoNotOptimize(v1);
    }
    if constexpr (Size >= 3)
    {
        benchmark::DoNotOptimize(v2);
    }
    if constexpr (Size >= 4)
    {
        benchmark::DoNotOptimize(v3);
    }
}
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Double, 1);
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Double, 2);
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Double, 3);
BENCHMARK_TEMPLATE(VectorBaseline_Addition_Double, 4);

#pragma endregion
