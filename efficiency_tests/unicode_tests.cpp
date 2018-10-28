/*
 * Duncan Horn
 */

#include <benchmark/benchmark.h>
#include <dhorn/unicode/iterator.h>

char ansi_string[] = "this is a relatively long ansi string that should be long enough to negate any setup or function"
    "calls. I want this to be pretty long, so here's some repeated ANSII text: foobarfoobarfoobarfoobarfoobarfoobar"
    "foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar"
    "foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar"
    "foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar"
    "foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar"
    "foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar"
    "foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar"
    "foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar"
    "foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar"
    "foobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobarfoobar";

void StringLength_Strlen_Test(benchmark::State& state)
{
    std::size_t size = 0;

    for ([[maybe_unused]] auto _ : state)
    {
        size += std::strlen(ansi_string);
    }

    benchmark::DoNotOptimize(size);
}
BENCHMARK(StringLength_Strlen_Test);

void StringLength_Utf8Iterator_Test(benchmark::State& state)
{
    std::size_t size = 0;

    for ([[maybe_unused]] auto _ : state)
    {
        dhorn::unicode::iterator itr(ansi_string);
        while (*itr++) ++size;
    }

    benchmark::DoNotOptimize(size);
}
BENCHMARK(StringLength_Utf8Iterator_Test);

void StringLength_TraitsLength_Test(benchmark::State& state)
{
    std::size_t size = 0;

    for ([[maybe_unused]] auto _ : state)
    {
        size += dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_8>::length(ansi_string).code_points;
    }

    benchmark::DoNotOptimize(size);
}
BENCHMARK(StringLength_TraitsLength_Test);

void StringLength_CodePointSize_Test(benchmark::State& state)
{
    std::size_t size = 0;

    for ([[maybe_unused]] auto _ : state)
    {
        std::size_t s = 0;
        while (ansi_string[s])
        {
            s += dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_8>::code_point_size(ansi_string[s]);
        }
        size += s;
    }

    benchmark::DoNotOptimize(size);
}
BENCHMARK(StringLength_CodePointSize_Test);

void FindCharacter_Strchr_Test(benchmark::State& state)
{
    for ([[maybe_unused]] auto _ : state)
    {
        auto ptr = std::strchr(ansi_string, '=');
        benchmark::DoNotOptimize(ptr);
    }
}
BENCHMARK(FindCharacter_Strchr_Test);

void FindCharacter_Utf8Iterator_Test(benchmark::State& state)
{
    for ([[maybe_unused]] auto _ : state)
    {
        dhorn::unicode::iterator itr(ansi_string);
        while (true)
        {
            auto ch = *itr;
            if (!ch || (ch == '='))
            {
                break;
            }
            ++itr;
        }
        benchmark::DoNotOptimize(itr);
    }
}
BENCHMARK(FindCharacter_Utf8Iterator_Test);

void FindCharacter_CodePointSize_Test(benchmark::State& state)
{
    for ([[maybe_unused]] auto _ : state)
    {
        std::size_t i = 0;
        while (ansi_string[i] && (ansi_string[i] != '='))
        {
            i += dhorn::unicode::encoding_traits<dhorn::unicode::encoding::utf_8>::code_point_size(ansi_string[i]);
        }
        benchmark::DoNotOptimize(i);
    }
}
BENCHMARK(FindCharacter_CodePointSize_Test);
