/*
 * Duncan Horn
 */

#include <benchmark/benchmark.h>
#include <dhorn/math/vector.h>

#define DECLARE_TEST_FOR_SIZE(Name, Traits, Size) \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<char, Size, Traits<char>>); \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<short, Size, Traits<short>>); \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<std::int32_t, Size, Traits<std::int32_t>>); \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<std::int64_t, Size, Traits<std::int64_t>>); \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<float, Size, Traits<float>>); \
BENCHMARK_TEMPLATE(Name, dhorn::math::vector<double, Size, Traits<double>>);

#define DECLARE_TEST_FOR_TRAITS(Name, Traits) \
DECLARE_TEST_FOR_SIZE(Name, Traits, 2) \
DECLARE_TEST_FOR_SIZE(Name, Traits, 3) \
DECLARE_TEST_FOR_SIZE(Name, Traits, 4) \
DECLARE_TEST_FOR_SIZE(Name, Traits, 5) \
DECLARE_TEST_FOR_SIZE(Name, Traits, 6) \
DECLARE_TEST_FOR_SIZE(Name, Traits, 7) \
DECLARE_TEST_FOR_SIZE(Name, Traits, 8)

#define DECLARE_TEST(Name) \
DECLARE_TEST_FOR_TRAITS(Name, dhorn::math::nointrin_traits) \
DECLARE_TEST_FOR_TRAITS(Name, dhorn::math::sse2_traits)

template <typename VectorType>
void Vector_DefaultConstruction(benchmark::State& state)
{
    for (auto _ : state)
    {
        // Should NOP in all cases
        VectorType v;
        benchmark::DoNotOptimize(v);
    }
}
//DECLARE_TEST(Vector_DefaultConstruction);

template <typename VectorType>
void Vector_Zero(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(VectorType::zero());
    }
}
DECLARE_TEST(Vector_Zero);
